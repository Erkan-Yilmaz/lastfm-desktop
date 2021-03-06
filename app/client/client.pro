TEMPLATE = app
TARGET = "Last.fm"
VERSION = 2.1.14
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
QT = core gui xml network sql phonon
CONFIG += lastfm unicorn listener logger
win32:LIBS += user32.lib
DEFINES += LASTFM_COLLAPSE_NAMESPACE

macx:LIBS += -framework Cocoa
win32:release {
        LIBS += -lAdvAPI32
}

include( ../../admin/include.qmake )

DEFINES += LASTFM_COLLAPSE_NAMESPACE
SOURCES -= LegacyTuner.cpp
HEADERS -= LegacyTuner.h

macx:ICON = ./audioscrobbler.icns
!win32:LIBS += -lz
win32:LIBS += shell32.lib User32.lib

RC_FILE = audioscrobbler.rc
unix:!mac {
        CONFIG += link_pkgconfig
        PKGCONFIG += libgpod-1.0
}

SUBDIRS=PrefPane

SOURCES += \
    AudioscrobblerSettings.cpp \
    Application.cpp \
    ActivityListModel.cpp \
    ImageTrack.cpp \
    StationSearch.cpp \
    ScrobSocket.cpp \
    MediaDevices/MediaDevice.cpp \
    MediaDevices/IpodDevice.cpp \
    MediaDevices/DeviceScrobbler.cpp \
    MainWindow.cpp \
    main.cpp \
    Settings/SettingsWidget.cpp \
    Settings/ScrobbleSettingsWidget.cpp \
    Settings/IpodSettingsWidget.cpp \
    Settings/AccountSettingsWidget.cpp \
    Settings/PreferencesDialog.cpp \
    Settings/AdvancedSettingsWidget.cpp \
    Settings/GeneralSettingsWidget.cpp \
    Services/ScrobbleService/StopWatch.cpp \
    Services/ScrobbleService/ScrobbleService.cpp \
    Services/RadioService/RadioService.cpp \
    Services/RadioService/RadioConnection.cpp \
    Dialogs/ScrobbleSetupDialog.cpp \
    Dialogs/DiagnosticsDialog.cpp \
    Bootstrapper/PluginBootstrapper.cpp \
    Bootstrapper/ITunesDevice/itunesdevice.cpp \
    Bootstrapper/iTunesBootstrapper.cpp \
    Bootstrapper/AbstractFileBootstrapper.cpp \
    Bootstrapper/AbstractBootstrapper.cpp \
    Widgets/ActivityListWidget.cpp \
    Widgets/RecentTracksWidget.cpp \
    Widgets/TitleBar.cpp \
    Widgets/TagFilterDialog.cpp \
    Widgets/StatusBar.cpp \
    Widgets/SideBar.cpp \
    Widgets/RadioWidget.cpp \
    Widgets/NothingPlayingWidget.cpp \
    Widgets/NowPlayingStackedWidget.cpp \
    Widgets/ProfileWidget.cpp \
    Widgets/FriendListWidget.cpp \
    Widgets/FriendWidget.cpp \
    Widgets/BioWidget.cpp \
    Widgets/MetadataWidget.cpp \
    Widgets/TagWidget.cpp \
    Widgets/CheckableDelegate.cpp \
    Widgets/TrackDelegate.cpp \
    Widgets/ShortcutEdit.cpp \
    Widgets/ProfileArtistWidget.cpp \
    Widgets/ScrobbleControls.cpp \
    Widgets/ProgressBar.cpp \
    Widgets/QuickStartWidget.cpp \
    Widgets/PointyArrow.cpp \
    Widgets/PlaybackControlsWidget.cpp \
    Widgets/PlayableItemWidget.cpp \
    Widgets/NowPlayingWidget.cpp \
    Widgets/ScrobblesWidget.cpp \
    Widgets/RefreshButton.cpp \
    Widgets/BackButton.cpp \
    Widgets/WidgetTextObject.cpp \
    Wizard/LoginPage.cpp \
    Wizard/BootstrapPage.cpp \
    Wizard/FirstRunWizard.cpp \
    Wizard/AccessPage.cpp \
    Wizard/TourMetadataPage.cpp \
    Wizard/PluginsPage.cpp \
    Wizard/TourRadioPage.cpp \
    Wizard/TourFinishPage.cpp \
    Wizard/PluginsInstallPage.cpp \
    Wizard/BootstrapProgressPage.cpp \
    Wizard/TourScrobblesPage.cpp \
    Wizard/TourLocationPage.cpp \
    Wizard/WizardPage.cpp \
    Widgets/ContextLabel.cpp \
    Widgets/SimilarArtistWidget.cpp \
    Widgets/PushButton.cpp \
    Dialogs/BetaDialog.cpp \
    Dialogs/CloseAppsDialog.cpp

HEADERS += \
    ScrobSocket.h \
    ImageTrack.h \
    AudioscrobblerSettings.h \
    Application.h \
    ActivityListModel.h \
    MainWindow.h \
    StationSearch.h \
    Services/RadioService/RadioConnection.h \
    Services/ScrobbleService.h \
    Services/ScrobbleService/StopWatch.h \
    Services/ScrobbleService/ScrobbleService.h \
    Services/RadioService.h \
    Services/RadioService/RadioService.h \
    MediaDevices/MediaDevice.h \
    MediaDevices/IpodDevice.h \
    MediaDevices/DeviceScrobbler.h \
    Dialogs/ScrobbleSetupDialog.h \
    Dialogs/DiagnosticsDialog.h \
    Bootstrapper/PluginBootstrapper.h \
    Bootstrapper/ITunesDevice/MediaDeviceInterface.h \
    Bootstrapper/ITunesDevice/ITunesParser.h \
    Bootstrapper/ITunesDevice/itunesdevice.h \
    Bootstrapper/iTunesBootstrapper.h \
    Bootstrapper/AbstractFileBootstrapper.h \
    Bootstrapper/AbstractBootstrapper.h \
    Settings/SettingsWidget.h \
    Settings/ScrobbleSettingsWidget.h \
    Settings/PreferencesDialog.h \
    Settings/AdvancedSettingsWidget.h \
    Settings/GeneralSettingsWidget.h \
    Settings/IpodSettingsWidget.h \
    Settings/AccountSettingsWidget.h \
    Widgets/ShortcutEdit.h \
    Widgets/TitleBar.h \
    Widgets/TagFilterDialog.h \
    Widgets/StatusBar.h \
    Widgets/SideBar.h \
    Widgets/ScrobbleControls.h \
    Widgets/ProgressBar.h \
    Widgets/QuickStartWidget.h \
    Widgets/PointyArrow.h \
    Widgets/PlaybackControlsWidget.h \
    Widgets/PlayableItemWidget.h \
    Widgets/NowPlayingWidget.h \
    Widgets/ActivityListWidget.h \
    Widgets/RecentTracksWidget.h \
    Widgets/RadioWidget.h \
    Widgets/NothingPlayingWidget.h \
    Widgets/NowPlayingStackedWidget.h \
    Widgets/ProfileWidget.h \
    Widgets/FriendListWidget.h \
    Widgets/FriendWidget.h \
    Widgets/BioWidget.h \
    Widgets/MetadataWidget.h \
    Widgets/TagWidget.h \
    Widgets/CheckableDelegate.h \
    Widgets/TrackDelegate.h \
    Widgets/ProfileArtistWidget.h \
    Widgets/ScrobblesWidget.h \
    Widgets/RefreshButton.h \
    Widgets/BackButton.h \
    Widgets/WidgetTextObject.h \
    Wizard/AccessPage.h \
    Wizard/TourMetadataPage.h \
    Wizard/PluginsPage.h \
    Wizard/TourRadioPage.h \
    Wizard/TourFinishPage.h \
    Wizard/PluginsInstallPage.h \
    Wizard/BootstrapProgressPage.h \
    Wizard/TourScrobblesPage.h \
    Wizard/TourLocationPage.h \
    Wizard/LoginPage.h \
    Wizard/FirstRunWizard.h \
    Wizard/BootstrapPage.h \
    Wizard/WizardPage.h \
    Widgets/ContextLabel.h \
    Widgets/SimilarArtistWidget.h \
    Widgets/PushButton.h \
    Dialogs/BetaDialog.h \
    Dialogs/CloseAppsDialog.h


win32:HEADERS += Plugins/FooBar08PluginInfo.h \
                    Plugins/FooBar09PluginInfo.h \
                    Plugins/ITunesPluginInfo.h \
                    Plugins/WinampPluginInfo.h \
                    Plugins/WmpPluginInfo.h \
                    Plugins/PluginList.h \
                    Plugins/KillProcess.h \
                    Plugins/IPluginInfo.h

win32:SOURCES += Plugins/PluginList.cpp \
                    Plugins/IPluginInfo.cpp \
                    Plugins/FooBar08PluginInfo.cpp \
                    Plugins/FooBar09PluginInfo.cpp \
                    Plugins/ITunesPluginInfo.cpp \
                    Plugins/WinampPluginInfo.cpp \
                    Plugins/WmpPluginInfo.cpp

mac:HEADERS += CommandReciever/CommandReciever.h \
    Services/ITunesPluginInstaller.h \
    Services/ITunesPluginInstaller/ITunesPluginInstaller.h

mac:SOURCES += Services/ITunesPluginInstaller/ITunesPluginInstaller_mac.cpp

mac:OBJECTIVE_SOURCES += CommandReciever/CommandReciever.mm \
                        Dialogs/CloseAppsDialog_mac.mm \
                        Widgets/NothingPlayingWidget_mac.mm

FORMS += \
    Widgets/TagFilterDialog.ui \
    Widgets/PlaybackControlsWidget.ui \
    Dialogs/ScrobbleSetupDialog.ui \
    Dialogs/DiagnosticsDialog.ui \
    Widgets/MetadataWidget.ui \
    Settings/PreferencesDialog.ui \
    Settings/GeneralSettingsWidget.ui \
    Settings/AccountSettingsWidget.ui \
    Settings/IpodSettingsWidget.ui \
    Settings/ScrobbleSettingsWidget.ui \
    Settings/AdvancedSettingsWidget.ui \
    Wizard/FirstRunWizard.ui \
    Widgets/NothingPlayingWidget.ui \
    Widgets/FriendWidget.ui \
    Widgets/FriendListWidget.ui \
    Dialogs/BetaDialog.ui \
    Dialogs/CloseAppsDialog.ui

unix:!mac:HEADERS += MediaDevices/IpodDevice_linux.h
unix:!mac:SOURCES += MediaDevices/IpodDevice_linux.cpp

RESOURCES += \
    qrc/audioscrobbler.qrc
