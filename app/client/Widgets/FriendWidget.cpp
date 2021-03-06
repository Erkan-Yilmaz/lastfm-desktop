
#include <QHBoxLayout>
#include <QMovie>

#include <lastfm/RadioStation.h>

#include "lib/unicorn/widgets/AvatarWidget.h"
#include "lib/unicorn/widgets/Label.h"

#include "../Application.h"
#include "PlayableItemWidget.h"

#include "FriendWidget.h"
#include "ui_FriendWidget.h"



FriendWidget::FriendWidget( const lastfm::XmlQuery& user, QWidget* parent)
    :StylableWidget( parent ),
      ui( new Ui::FriendWidget ),
      m_user( user ),
      m_order( 0 - 1 ),
      m_listeningNow( false )
{   
    ui->setupUi( this );

    layout()->setAlignment( ui->avatar, Qt::AlignTop );

    m_movie = new QMovie( ":/icon_eq.gif", "GIF", this );
    m_movie->setCacheMode( QMovie::CacheAll );
    ui->equaliser->setMovie( m_movie );
    ui->equaliser->hide();

    update( user, -1 );

    QRegExp re( "/serve/(\\d*)s?/" );
    ui->avatar->loadUrl( user["image size=medium"].text().replace( re, "/serve/\\1s/" ), HttpImageWidget::ScaleNone );
    ui->avatar->setHref( user["url"].text() );

    ui->radio->setStation( RadioStation::library( User( user["name"].text() ) ), tr( "%2%1s Library Radio" ).arg( QChar( 0x2019 ), user["name"].text() ) );

    ui->avatar->setUser( m_user );
}

void
FriendWidget::resizeEvent( QResizeEvent* event )
{
    emit sizeChanged( event->size() );
}

void
FriendWidget::update( const lastfm::XmlQuery& user, unsigned int order )
{
    m_order = order;

    m_recentTrack.setTitle( user["recenttrack"]["name"].text() );
    m_recentTrack.setAlbum( user["recenttrack"]["album"]["name"].text() );
    m_recentTrack.setArtist( user["recenttrack"]["artist"]["name"].text() );
    m_recentTrack.setExtra( "playerName", user["scrobblesource"]["name"].text() );
    m_recentTrack.setExtra( "playerURL", user["scrobblesource"]["url"].text() );

    QString recentTrackDate = user["recenttrack"].attribute( "uts" );

    bool hasListened = m_recentTrack != lastfm::Track();
    ui->trackFrame->setVisible( hasListened );

    m_listeningNow = recentTrackDate.isEmpty() && hasListened;

    if ( !recentTrackDate.isEmpty() )
        m_recentTrack.setTimeStamp( QDateTime::fromTime_t( recentTrackDate.toUInt() ) );

    setDetails();
}

void
FriendWidget::setOrder( int order )
{
    m_order = order;
}

void
FriendWidget::setListeningNow( bool listeningNow )
{
    m_listeningNow = listeningNow;
}

bool
FriendWidget::operator<( const FriendWidget& that ) const
{
    // sort by most recently listened and then by name

    if ( this->m_listeningNow && !that.m_listeningNow )
        return true;

    if ( !this->m_listeningNow && that.m_listeningNow )
        return false;

    if ( this->m_listeningNow && that.m_listeningNow )
        return this->name().toLower() < that.name().toLower();

    if ( !this->m_recentTrack.timestamp().isNull() && that.m_recentTrack.timestamp().isNull() )
        return true;

    if ( this->m_recentTrack.timestamp().isNull() && !that.m_recentTrack.timestamp().isNull() )
        return false;

    if ( this->m_recentTrack.timestamp().isNull() && that.m_recentTrack.timestamp().isNull() )
        return this->name().toLower() < that.name().toLower();

    // both timestamps are valid!

    if ( this->m_recentTrack.timestamp() == that.m_recentTrack.timestamp() )
    {
        if ( this->m_order == that.m_order )
            return this->name().toLower() < that.name().toLower();

        return this->m_order < that.m_order;
    }

    // this is the other way around because a higher time means it's lower in the list
    return this->m_recentTrack.timestamp() > that.m_recentTrack.timestamp();
}


void
FriendWidget::setDetails()
{
    ui->userDetails->setText( m_user.getInfoString() );
    ui->username->setText( name() );
    ui->lastTrack->setText( m_recentTrack.toString() );

    if ( m_listeningNow )
    {
        // show the
        m_movie->start();
        ui->equaliser->show();

        ui->trackFrame->setObjectName( "nowListening" );
        style()->polish( ui->trackFrame );

        if ( !m_recentTrack.extra( "playerName" ).isEmpty() )
            ui->timestamp->setText( tr( "Scrobbling now from %1" ).arg( m_recentTrack.extra( "playerName" ) ) );
        else
            ui->timestamp->setText( tr( "Scrobbling now" ) );
    }
    else
    {
        m_movie->stop();
        ui->equaliser->hide();

        ui->trackFrame->setObjectName( "groupBox" );
        style()->polish( ui->trackFrame );

        ui->timestamp->setText( unicorn::Label::prettyTime( m_recentTrack.timestamp() )  );
    }
}

QString
FriendWidget::name() const
{
    return m_user.name();
}

QString
FriendWidget::realname() const
{
    return m_user.realName();
}
