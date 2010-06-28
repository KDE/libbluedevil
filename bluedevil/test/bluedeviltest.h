/*  This file is part of the KDE project
    Copyright (C) 2010 Rafael Fernández López <ereslibre@kde.org>
    Copyright (C) 2010 UFO Coders <info@ufocoders.com>

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

#ifndef BLUEDEVILTEST_H
#define BLUEDEVILTEST_H

#include <QtCore/QObject>

#define ASYNCHRONOUS_API

namespace BlueDevil {
    class Adapter;
    class Device;
}

using namespace BlueDevil;

class DeviceReceiver
    : public QObject
{
    Q_OBJECT

public:
    DeviceReceiver(QObject *parent = 0);
    virtual ~DeviceReceiver();

    void scanDevices();

public Q_SLOTS:
    void deviceFound(Device *device);
    void adapterAdded(Adapter *adapter);
#ifdef ASYNCHRONOUS_API
    void deviceRegistered(Device *device, bool registered);
#endif
};

#endif // BLUEDEVILTEST_H
