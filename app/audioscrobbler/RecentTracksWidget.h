/*
   Copyright 2005-2009 Last.fm Ltd.
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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

#include <QWidget>
#include <QList>

#include "lib/unicorn/StylableWidget.h"

namespace lastfm { class Track; };
using lastfm::Track;

class RecentTracksWidget : public StylableWidget
{
    Q_OBJECT
public:
    RecentTracksWidget( QString username, QWidget* parent = 0 );

    void setUsername( QString username );
    void read();
    void write() const;

    void addCachedTrack( const Track& a_track );

private slots:
    void onTrackChanged();
    void onMoveFinished();
    void doWrite() const;

private:
    QString m_path;
    class QTimer* m_writeTimer;
};
