/*  This file is part of the KDE project
    Copyright (C) 2010 Rafael Fernández López <ereslibre@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef BLUEDEVILDEVICE_H
#define BLUEDEVILDEVICE_H

#include <bluedevil_export.h>

#include <QtCore/QObject>

namespace BlueDevil {

class Adapter;

class BLUEDEVIL_EXPORT Device
    : public QObject
{
    friend class Adapter;

public:
    virtual ~Device();

    QString getAddress() const;
    QString getAlias() const;
    quint32 getDeviceClass() const;
    QString getIcon() const;
    bool hasLegacyPairing() const;
    QString getName() const;
    bool isPaired() const;
    short getRSSI() const;

private:
    Device(const QString &address, const QString &alias, quint32 deviceClass, const QString &icon,
           bool legacyPairing, const QString &name, bool paired, short RSSI, QObject *parent = 0);

    class Private;
    Private *const d;
};

}

#endif // BLUEDEVILDEVICE_H
