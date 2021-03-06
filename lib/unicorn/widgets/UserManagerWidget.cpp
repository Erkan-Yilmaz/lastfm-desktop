#include "UserManagerWidget.h"

#include "lib/unicorn/widgets/AvatarWidget.h"
#include "lib/unicorn/dialogs/LoginContinueDialog.h"
#include "lib/unicorn/dialogs/LoginDialog.h"
#include "lib/unicorn/LoginProcess.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/unicorn/UnicornApplication.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/UnicornSession.h"

#include <lastfm/User.h>

#include <QApplication>
#include <QButtonGroup>
#include <QDebug>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

using lastfm::User;
UserRadioButton::UserRadioButton( const User& user )
{
    addWidget( ui.button = new QRadioButton() );
    ui.button->setObjectName( "button" );

    addWidget( ui.image = new AvatarWidget() );
    ui.image->setObjectName( "image" );

    addWidget( ui.username = new QLabel( user.name() ) );
    addWidget( ui.realName = new QLabel() );
    addWidget( ui.loggedIn = new QLabel() );
    ui.realName->setObjectName( "realname" );

    addStretch();

    addWidget( ui.remove = new QPushButton( tr("Remove") ));

    setUser( user );

    if( user.imageUrl( lastfm::Medium ).isEmpty() )
    {
        QNetworkReply* reply = User::getInfo( user.name() );
        connect( reply, SIGNAL(finished()), SLOT( onUserFetched()));
    }


    connect( ui.button, SIGNAL(clicked()), this, SIGNAL(clicked()) );
    connect( ui.remove, SIGNAL(clicked()), this, SIGNAL(remove()) );

    connect( qApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*)) );
}

void
UserRadioButton::onSessionChanged( unicorn::Session* session )
{
    if( ui.username->text() == session->userInfo().name() )
        ui.loggedIn->setText( tr( "(currently logged in)" ) );
    else
        ui.loggedIn->clear();
}

void
UserRadioButton::click()
{
    ui.button->click();
}

bool
UserRadioButton::isChecked() const
{
    return ui.button->isChecked();
}

void
UserRadioButton::setUser( const lastfm::User& user )
{
    ui.username->setText( user.name() );

    if( !user.realName().isEmpty() )
        ui.realName->setText( QString( " (%1)" ).arg( user.realName() ) );

    if( user == User() )
        ui.loggedIn->setText( tr( "(currently logged in)" ) );

    if ( !user.imageUrl( lastfm::Medium ).isEmpty() )
            ui.image->loadUrl( user.imageUrl( lastfm::Medium ) );

    ui.image->setHref( user.www() );
}

void 
UserRadioButton::onUserFetched()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( sender());
    Q_ASSERT( reply );

    XmlQuery lfm;
    if ( lfm.parse( reply->readAll() ) )
    {
        User user( lfm["user"] );
        setUser( user );
    }
    else
    {
        qDebug() << lfm.parseError().message() << lfm.parseError().enumValue();
    }
}


const QString 
UserRadioButton::user() const
{
    return ui.username->text();
}


UserManagerWidget::UserManagerWidget( QWidget* parent )
    :QWidget( parent )
{
    m_lcd = new LoginContinueDialog( this );
    connect( m_lcd, SIGNAL( accepted()), SLOT( onUserAdded()));

    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setSpacing( 10 );

    layout->addWidget( ui.groupBox = new QGroupBox() );

    ui.usersLayout = new QVBoxLayout( ui.groupBox );

    ui.groupBox->setTitle( tr( "Connected User Accounts:" ));

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    ui.addUserButton = new QPushButton( tr( "Add New User Account" ), this );

    buttonLayout->addWidget( ui.addUserButton );
    buttonLayout->addStretch( 1 );

    layout->addLayout( buttonLayout );

    layout->addStretch();

    QList<lastfm::User> roster = unicorn::Settings().userRoster();

    foreach( lastfm::User u, roster )
    {
        UserRadioButton* b = new UserRadioButton( u );
        add( b, false );
    }

    connect( ui.addUserButton, SIGNAL( clicked() ), SLOT( onAddUserClicked() ) );
}


UserManagerWidget::~UserManagerWidget()
{
}

void
UserManagerWidget::onAddUserClicked()
{
    LoginDialog* ld = new LoginDialog( this );
    ld->setWindowFlags( Qt::Sheet );
    ld->open();
    connect( ld, SIGNAL( accepted()), SLOT( onLoginDialogAccepted()) );
}


void
UserManagerWidget::onLoginDialogAccepted()
{
    LoginDialog* ld = qobject_cast<LoginDialog*>(sender());
    Q_ASSERT( ld );

    if ( m_loginProcess )
    {
        delete m_loginProcess;
        m_loginProcess = 0;
    }

    m_loginProcess = new unicorn::LoginProcess( this );

    ld->deleteLater();


    connect( m_loginProcess, SIGNAL(gotSession(unicorn::Session*)), SLOT( onGotSession( unicorn::Session* ) ) );

    m_loginProcess->authenticate();


    m_lcd->setWindowFlags( Qt::Sheet );
    m_lcd->open();
}


void
UserManagerWidget::onGotSession( unicorn::Session* s )
{
    Q_UNUSED( s )
    m_lcd->accept();
}

void 
UserManagerWidget::onUserAdded()
{
    Q_ASSERT( m_loginProcess );

    unicorn::Session* s = qobject_cast<unicorn::Application*>( qApp )->currentSession();

    if ( !s )
    {
        m_loginProcess->cancel();
        m_loginProcess->showError();
        return;
    }

    bool alreadyAdded = false;
    foreach ( UserRadioButton* b, findChildren<UserRadioButton*>() )
    {
        if ( s->userInfo().name() == b->user() )
        {
            alreadyAdded = true;
            break;
        }
    }

    if ( !alreadyAdded )
    {
        User user( s->userInfo().name() );
        UserRadioButton* urb = new UserRadioButton( user );

        add( urb );
        if( ui.groupBox->layout()->count() <= 1 )
            urb->click();
    }
    else
    {
        QMessageBoxBuilder( this )
                    .setIcon( QMessageBox::Information )
                    .setTitle( tr( "User already added" ) )
                    .setText( tr( "This user has already been added." ) )
                    .exec();

    }
}

void
UserManagerWidget::onUserRemoved()
{
    UserRadioButton* urb = qobject_cast<UserRadioButton*>(sender());

    int result =
        QMessageBoxBuilder( parentWidget() ).setTitle( tr( "Removing %1" ).arg( urb->user() ) )
                                  .setText( tr( "Are you sure you want to remove this user? All user settings will be lost and you will need to re authenticate in order to scrobble in the future." ))
                                  .setIcon( QMessageBox::Question )
                                  .setButtons( QMessageBox::Yes | QMessageBox::No )
                                  .exec();

    if( result != QMessageBox::Yes )
        return;

    unicorn::Settings us;
    us.beginGroup( "Users" );
    us.remove( urb->user() );
    us.endGroup();

    if ( us.userRoster().count() == 0 )
    {
        us.setValue( SETTING_FIRST_RUN_WIZARD_COMPLETED, false );
        qobject_cast<unicorn::Application*>( qApp )->restart();
    }

    // if this is the currently logged in user we will have to choose someone else
    if( urb->ui.button->isChecked() )
    {
        for ( int i = 0 ; i < ui.usersLayout->count() ; ++i )
        {
            UserRadioButton* button = qobject_cast<UserRadioButton*>(ui.usersLayout->itemAt( i )->layout());

            if ( button->user() != urb->user() )
            {
                // This is the first user in the list that are not deleting!

                button->click();

                unicorn::Settings s;
                s.setValue( "Username", button->user() );

                unicorn::UserSettings us( button->user() );
                QString sessionKey = us.value( "SessionKey", "" ).toString();
                qobject_cast<unicorn::Application *>( qApp )->changeSession( button->user(), sessionKey, true );

                break;
            }
        }
    }

    // delete all the widgets, that this layout has
    QLayoutItem* item;
    while ( (item = urb->takeAt( 0 )) != 0  )
    {
        QWidget* widget = item->widget();
        if ( widget )
            widget->deleteLater();
    }

    urb->deleteLater();
}

void 
UserManagerWidget::add( UserRadioButton* urb, bool announce )
{
    ui.usersLayout->addLayout( urb );

    if ( urb->user() == User().name() )
        urb->click();

    connect( urb, SIGNAL(remove()), SLOT(onUserRemoved()));
    connect( urb, SIGNAL( clicked() ), this, SIGNAL( userChanged() ) );

    if ( announce )
        emit rosterUpdated();

    connect( urb, SIGNAL( destroyed(QObject*)), SIGNAL( rosterUpdated()));
}


UserRadioButton*
UserManagerWidget::checkedButton() const
{
    for ( int i = 0 ; i < ui.usersLayout->count() ; ++i )
    {
        UserRadioButton* button = qobject_cast<UserRadioButton*>(ui.usersLayout->itemAt( i )->layout());

        if ( button && button->isChecked() )
        {
            return button;
        }
    }

    return 0;
}
