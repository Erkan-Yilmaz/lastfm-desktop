
#include <QShortcut>
#include <QMenu>
#include <QMovie>

#include "lib/unicorn/UnicornSettings.h"

#include "../Application.h"
#include "../Services/RadioService.h"
#include "../Services/ScrobbleService.h"

#include "PlaybackControlsWidget.h"
#include "ui_PlaybackControlsWidget.h"

PlaybackControlsWidget::PlaybackControlsWidget(QWidget *parent) :
    StylableWidget(parent),
    ui(new Ui::PlaybackControlsWidget),
    m_scrobbleTrack( false )
{
    ui->setupUi(this);

    ui->play->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->ban->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->love->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->skip->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->play->setAttribute( Qt::WA_MacNoClickThrough );
    ui->ban->setAttribute( Qt::WA_MacNoClickThrough );
    ui->love->setAttribute( Qt::WA_MacNoClickThrough );
    ui->skip->setAttribute( Qt::WA_MacNoClickThrough );

    // If the actions are triggered we should do something
    // love is dealt with by the application
    connect( aApp->banAction(), SIGNAL(triggered(bool)), SLOT(onBanClicked()) );
    connect( aApp->playAction(), SIGNAL(triggered(bool)), SLOT(onPlayClicked(bool)) );
    connect( aApp->skipAction(), SIGNAL(triggered(bool)), SLOT(onSkipClicked()) );

    m_playAction = new QAction( tr( "Play" ), aApp );
    connect( m_playAction, SIGNAL(triggered(bool)), aApp->playAction(), SLOT(trigger()) );
    connect( aApp->playAction(), SIGNAL(toggled(bool)), m_playAction, SLOT(setChecked(bool)) );

    // make sure this widget updates if the actions are changed elsewhere
    connect( aApp->loveAction(), SIGNAL(changed()), SLOT(onActionsChanged()) );
    connect( aApp->banAction(), SIGNAL(changed()), SLOT(onActionsChanged()) );
    connect( aApp->playAction(), SIGNAL(changed()), SLOT(onActionsChanged()) );
    connect( aApp->skipAction(), SIGNAL(changed()), SLOT(onActionsChanged()) );

    connect( &RadioService::instance(), SIGNAL(tuningIn(RadioStation)), SLOT(onTuningIn(RadioStation)));
    connect( &RadioService::instance(), SIGNAL(error(int,QVariant)), SLOT(onError(int, QVariant)));

    connect( &ScrobbleService::instance(), SIGNAL(trackStarted(Track,Track)), SLOT(onTrackStarted(Track,Track)) );
    connect( &ScrobbleService::instance(), SIGNAL(stopped()), SLOT(onStopped()));
    connect( &ScrobbleService::instance(), SIGNAL(scrobblingOnChanged(bool)), SLOT(update()));

    onActionsChanged();

    // if our buttons are pressed we should trigger the actions
    connect( ui->love, SIGNAL(clicked()), aApp->loveAction(), SLOT(trigger()));
    connect( ui->ban, SIGNAL(clicked()), aApp->banAction(), SLOT(trigger()));
    connect( ui->play, SIGNAL(clicked()), aApp->playAction(), SLOT(trigger()));
    connect( ui->skip, SIGNAL(clicked()), aApp->skipAction(), SLOT(trigger()));
}

void
PlaybackControlsWidget::addToMenu( QMenu& menu )
{
    menu.addAction( m_playAction );

    menu.addSeparator();

    menu.addAction( aApp->skipAction() );

    menu.addSeparator();

    menu.addAction( aApp->loveAction() );
    menu.addAction( aApp->banAction() );

    menu.addSeparator();

    menu.addAction( aApp->tagAction() );
    menu.addAction( aApp->shareAction() );

    menu.addSeparator();

    menu.addAction( tr( "Volume Up" ), &RadioService::instance(), SLOT(volumeUp()), QKeySequence( Qt::CTRL + Qt::Key_Up ));
    menu.addAction( tr( "Volume Down" ), &RadioService::instance(), SLOT(volumeDown()), QKeySequence( Qt::CTRL + Qt::Key_Down ));
    menu.addAction( tr( "Mute" ), &RadioService::instance(), SLOT(mute()), QKeySequence( Qt::CTRL + Qt::ALT + Qt::Key_Down ));
}


PlaybackControlsWidget::~PlaybackControlsWidget()
{
    delete ui;
}


void
PlaybackControlsWidget::setScrobbleTrack( bool scrobbleTrack )
{
    m_scrobbleTrack = scrobbleTrack;
    style()->polish( this );
    style()->polish( ui->details );
    style()->polish( ui->status );
    style()->polish( ui->device );
}

void
PlaybackControlsWidget::onActionsChanged()
{
   ui->love->setChecked( aApp->loveAction()->isChecked() );
   ui->ban->setChecked( aApp->banAction()->isChecked() );
   ui->play->setChecked( aApp->playAction()->isChecked() );
   ui->skip->setChecked( aApp->skipAction()->isChecked() );

   m_playAction->setText( aApp->playAction()->isChecked() ? tr( "Pause" ) : RadioService::instance().state() == Stopped ? tr( "Play" ) : tr( "Resume" ) );

   ui->love->setEnabled( aApp->loveAction()->isEnabled() );
   ui->ban->setEnabled( aApp->banAction()->isEnabled() );
   ui->play->setEnabled( aApp->playAction()->isEnabled() );
   ui->skip->setEnabled( aApp->skipAction()->isEnabled() );

   ui->love->setToolTip( ui->love->isChecked() ? tr("Unlove") : tr("Love") );
   ui->ban->setToolTip( tr("Ban") );
   ui->play->setToolTip( ui->play->isChecked() ? tr("Pause") : tr("Play") );
   ui->skip->setToolTip( tr("Skip") );

   ui->love->setText( ui->love->isChecked() ? tr("Unlove") : tr("Love") );
   ui->ban->setText( tr("Ban") );
   ui->play->setText( ui->play->isChecked() ? tr("Pause") : tr("Play") );
   ui->skip->setText( tr("Skip") );
}


void
PlaybackControlsWidget::onSpace()
{
    aApp->playAction()->trigger();
}


void
PlaybackControlsWidget::onPlayClicked( bool checked )
{
    if ( checked )
    {
        if ( RadioService::instance().state() == Stopped )
            RadioService::instance().play( RadioStation( "" ) );
        else
        {
            RadioService::instance().resume();
        }
    }
    else
    {
        RadioService::instance().pause();
    }
}

void
PlaybackControlsWidget::onSkipClicked()
{
    RadioService::instance().skip();
}


void
PlaybackControlsWidget::onLoveClicked( bool loved )
{
    MutableTrack track( RadioService::instance().currentTrack() );

    if ( loved )
        track.love();
    else
        track.unlove();

    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), ui->love, SLOT(setChecked(bool)));
}


void
PlaybackControlsWidget::onLoveTriggered( bool loved )
{
    ui->love->setChecked( loved );
    onLoveClicked( loved );
}


void
PlaybackControlsWidget::onBanClicked()
{
    QNetworkReply* banReply = MutableTrack( RadioService::instance().currentTrack() ).ban();
    connect(banReply, SIGNAL(finished()), SLOT(onBanFinished()));
}


void
PlaybackControlsWidget::onBanFinished()
{
    lastfm::XmlQuery lfm;
    lfm.parse( static_cast<QNetworkReply*>(sender())->readAll() );

    if ( lfm.attribute( "status" ) != "ok" )
    {
    }

    aApp->skipAction()->trigger();
}

void
PlaybackControlsWidget::setIconForRadio( const RadioStation& station )
{
    QString url = station.url();

    if ( url.startsWith("lastfm://user") )
    {
        if ( url.contains( "/friends" )
            || url.contains( "/neighbours" )
            || url.startsWith( "lastfm://users") )
            ui->icon->setPixmap( QPixmap( ":/control_bar_radio_friends.png" ) );
        if ( url.contains( "/library" )
             || url.contains( "/personal") )
            ui->icon->setPixmap( QPixmap( ":/control_bar_radio_library.png" ) );
        else if ( url.contains( "/mix" ) )
            ui->icon->setPixmap( QPixmap( ":/control_bar_radio_mix.png" ) );
        else if ( url.contains( "/recommended" ) )
            ui->icon->setPixmap( QPixmap( ":/control_bar_radio_rec.png" ) );

    }
    else if ( url.startsWith("lastfm://artist") )
        ui->icon->setPixmap( QPixmap( ":/control_bar_radio_artist.png" ) );
    else if ( url.startsWith("lastfm://tag")
                || url.startsWith( "lastfm://globaltags" ) )
        ui->icon->setPixmap( QPixmap( ":/control_bar_radio_tag.png" ) );
    else
        ui->icon->setPixmap( QPixmap( ":/control_bar_radio_library.png" ) );
}

void
PlaybackControlsWidget::onTuningIn( const RadioStation& station )
{
    setScrobbleTrack( false );

    ui->status->setText( tr("Tuning...") );
    ui->device->setText( station.title() );

    ui->play->setChecked( true );
    aApp->playAction()->setChecked( true );

    if ( !m_movie )
    {
        m_movie = new QMovie( ":/loading_radio.gif", "GIF", this );
        m_movie->setCacheMode( QMovie::CacheAll );
    }

    ui->icon->setMovie( m_movie );
    m_movie->start();

    ui->progressBar->setTrack( Track() );

    aApp->playAction()->setEnabled( true );
    aApp->loveAction()->setEnabled( false );
    aApp->banAction()->setEnabled( false );
    aApp->skipAction()->setEnabled( false );
    aApp->tagAction()->setEnabled( false );
    aApp->shareAction()->setEnabled( false );

    ui->controls->show();
}

void
PlaybackControlsWidget::onTrackStarted( const Track& track, const Track& oldTrack )
{
    ui->progressBar->setTrack( track );

    if ( m_movie )
        m_movie->stop();

    if ( !track.isNull() )
    {
        disconnect( &RadioService::instance(), SIGNAL(tick(qint64)), this, SLOT(onTick(qint64)));
        disconnect( &ScrobbleService::instance(), SIGNAL(frameChanged(int)), ui->progressBar, SLOT(onFrameChanged(int)) );

        // you can love tag and share all tracks
        aApp->loveAction()->setEnabled( true );
        aApp->tagAction()->setEnabled( true );
        aApp->shareAction()->setEnabled( true );

        // play is always enabled as you should always
        // be able to start the radio
        aApp->playAction()->setEnabled( true );

        aApp->playAction()->setChecked( track.source() == Track::LastFmRadio );

        // can only ban and skip radio tracks
        aApp->banAction()->setEnabled( track.source() == Track::LastFmRadio );
        aApp->skipAction()->setEnabled( track.source() == Track::LastFmRadio );

        aApp->loveAction()->setChecked( track.isLoved() );

        ui->controls->setVisible( track.source() == Track::LastFmRadio );

        setScrobbleTrack( track.source() != Track::LastFmRadio  );

        if( track.source() == Track::LastFmRadio )
        {
            // A radio track!
            connect( track.signalProxy(), SIGNAL(loveToggled(bool)), ui->love, SLOT(setChecked(bool)));

            ui->status->setText( tr("Listening to...") );
            ui->device->setText( RadioService::instance().station().title() );

            connect( &RadioService::instance(), SIGNAL(tick(qint64)), SLOT(onTick(qint64)) );
        }
        else
        {
            // Not a radio track

            if ( track.extra( "playerId" ) == "spt" )
                ui->status->setText( tr("Listening to...") );
            else
                ui->status->setText( tr("Scrobbling from...") );

            ui->device->setText( track.extra( "playerName" ) );

            connect( &ScrobbleService::instance(), SIGNAL(frameChanged(int)), ui->progressBar, SLOT(onFrameChanged(int)) );
        }

        // Set the icon!
        QString id = track.extra( "playerId" );

        if ( id == "osx" || id == "itw" )
            ui->icon->setPixmap( QPixmap( ":/control_bar_scrobble_itunes.png" ) );
        else if (id == "foo")
            ui->icon->setPixmap( QPixmap( ":/control_bar_scrobble_foobar.png" ) );
        else if (id == "wa2")
            ui->icon->setPixmap( QPixmap( ":/control_bar_scrobble_winamp.png" ) );
        else if (id == "wmp")
            ui->icon->setPixmap( QPixmap( ":/control_bar_scrobble_wmp.png" ) );
        else if (id == "spt")
            ui->icon->setPixmap( QPixmap( ":/control_bar_scrobble_spotify.png" ) );
        else if (id == "ass")
            setIconForRadio( RadioService::instance().station() );
    }
}

void
PlaybackControlsWidget::onTick( qint64 tick )
{
    ui->progressBar->onFrameChanged( tick );
}

void
PlaybackControlsWidget::onError( int error, const QVariant& errorText )
{
}

void
PlaybackControlsWidget::onStopped()
{
    ui->progressBar->setTrack( Track() );
    aApp->playAction()->setChecked( false );

    aApp->playAction()->setEnabled( true );
    aApp->loveAction()->setEnabled( false );
    aApp->banAction()->setEnabled( false );
    aApp->skipAction()->setEnabled( false );
    aApp->tagAction()->setEnabled( false );
    aApp->shareAction()->setEnabled( false );
}

