/*
   Copyright 2010 Last.fm Ltd.
      - Primarily authored by Jono Cole, Michael Coffey, and William Viana

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ui_IpodSettingsWidget.h"
#include "IpodSettingsWidget.h"

#ifdef Q_WS_X11
#include "../MediaDevices/IpodDevice_linux.h"
#endif

#include "../MediaDevices/IpodDevice.h"

#include "lib/unicorn/UnicornSettings.h"

#include <lastfm/User.h>

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QTreeWidget>
#include <QVBoxLayout>

enum
{
    IpodColumnUser,
    IpodColumnDeviceName,
    IpodColumnScrobble,
    IpodColumnAlwaysAsk,
    IpodColumnCount
};

IpodSettingsWidget::IpodSettingsWidget( QWidget* parent )
    : SettingsWidget( parent ),
      ui( new Ui::IpodSettingsWidget )
{
    ui->setupUi( this );

    connect( ui->clearAssociations, SIGNAL( clicked() ), this, SLOT( clearIpodAssociations() ) );
    connect( ui->removeAssociation, SIGNAL( clicked() ), this, SLOT( removeIpodAssociation() ) );

    ui->iPodAssociations->header()->setResizeMode( QHeaderView::ResizeToContents );

    QStringList headerLabels;
    headerLabels.append( tr( "User" ) );
    headerLabels.append( tr( "Device Name" ) );
    headerLabels.append( tr( "Scrobble" ) );
    headerLabels.append( tr( "Always Ask" ) );
    ui->iPodAssociations->setHeaderLabels( headerLabels );

    populateIpodAssociations();

    // do these after populating so we don't corrupt the associations
    connect( ui->iPodAssociations, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), SLOT( onItemActivated() ) );
    connect( ui->iPodAssociations, SIGNAL( itemChanged(QTreeWidgetItem*,int)), SLOT( onSettingsChanged() ) );
}

void
IpodSettingsWidget::saveSettings()
{
    if ( hasUnsavedChanges() )
    {
        // save settings
        qDebug() << "Saving settings...";

        // remove all associations and add them again with the current settings
        QList<lastfm::User> roster = unicorn::Settings().userRoster();

        foreach( lastfm::User user, roster )
        {
            unicorn::UserSettings us( user.name() );
            us.remove( "associatedDevices" );
        }

        for ( int i = 0 ; i < ui->iPodAssociations->topLevelItemCount() ; ++i )
        {
            QTreeWidgetItem* item = ui->iPodAssociations->topLevelItem( i );
            QString deviceName = item->text( IpodColumnDeviceName );
            QString deviceId = item->data( IpodColumnDeviceName, Qt::UserRole ).toString();
            IpodDevice* ipod = new IpodDevice( deviceId, deviceName );

            ipod->associateDevice( static_cast<QComboBox*>( ui->iPodAssociations->itemWidget( item, IpodColumnUser ) )->currentText() );
            ipod->setScrobble( item->checkState( IpodColumnScrobble ) == Qt::Checked );
            ipod->setAlwaysAsk( item->checkState( IpodColumnAlwaysAsk ) == Qt::Checked );
        }

        onSettingsSaved();
    }
}

void
IpodSettingsWidget::populateIpodAssociations()
{
    QList<lastfm::User> roster = unicorn::Settings().userRoster();

    foreach( lastfm::User user, roster )
    {
        unicorn::UserSettings us( user.name() );
        int count = us.beginReadArray( "associatedDevices" );

        for ( int i = 0 ; i < count ; i++ )
        {
            us.setArrayIndex( i );
            QTreeWidgetItem* item = new QTreeWidgetItem( ui->iPodAssociations );
            QComboBox* comboBox = new QComboBox( ui->iPodAssociations );

            for ( int i = 0 ; i < roster.count() ; ++i )
            {
                comboBox->addItem( roster.at( i ).name() );

                if ( roster.at( i ).name() == user.name() )
                    comboBox->setCurrentIndex( i );
            }

            ui->iPodAssociations->setItemWidget( item, IpodColumnUser, comboBox );

            item->setText( IpodColumnDeviceName, us.value( "deviceName" ).toString() );
            item->setData( IpodColumnDeviceName, Qt::UserRole, us.value( "deviceId" ).toString() );

            item->setCheckState( IpodColumnScrobble, us.value( "scrobble" ).toBool() ? Qt::Checked : Qt::Unchecked );
            item->setCheckState( IpodColumnAlwaysAsk, us.value( "alwaysAsk" ).toBool() ? Qt::Checked : Qt::Unchecked );

        }

        us.endArray();
    }

    ui->clearAssociations->setEnabled( ui->iPodAssociations->topLevelItemCount() > 0 );
    ui->removeAssociation->setEnabled( false );
}

void
IpodSettingsWidget::clearIpodAssociations()
{
    QList<lastfm::User> roster = unicorn::Settings().userRoster();
    foreach( lastfm::User user, roster )
    {
        unicorn::UserSettings us( user.name() );
        us.remove( "associatedDevices" );
    }

#ifdef Q_WS_X11
    IpodDeviceLinux::deleteDevicesHistory();
#endif

    ui->iPodAssociations->clear();
    ui->clearAssociations->setEnabled( false );
    ui->removeAssociation->setEnabled( false );
}

void
IpodSettingsWidget::onItemActivated()
{
    ui->removeAssociation->setEnabled( true );
}

void
IpodSettingsWidget::removeIpodAssociation()
{
    QTreeWidgetItem* association = ui->iPodAssociations->currentItem();
    ui->iPodAssociations->takeTopLevelItem( ui->iPodAssociations->indexOfTopLevelItem( association ) );
    ui->removeAssociation->setEnabled( false );

    // This will delete all the users associations and only restore the remaining ones
    onSettingsChanged();
}
