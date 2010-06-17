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

#include "bluedevildevice.h"

#include <QtCore/QString>

namespace BlueDevil {

class Device::Private
{
public:
    Private(const QString &address, const QString &alias, quint32 deviceClass, const QString &icon,
            bool legacyPairing, const QString &name, bool paired, short RSSI);

    QString m_address;
    QString m_alias;
    quint32 m_deviceClass;
    QString m_icon;
    bool    m_legacyPairing;
    QString m_name;
    bool    m_paired;
    short   m_RSSI;
};

Device::Private::Private(const QString &address, const QString &alias, quint32 deviceClass,
                         const QString &icon, bool legacyPairing, const QString &name, bool paired,
                         short RSSI)
    : m_address(address)
    , m_alias(alias)
    , m_deviceClass(deviceClass)
    , m_icon(icon)
    , m_legacyPairing(legacyPairing)
    , m_name(name)
    , m_paired(paired)
    , m_RSSI(RSSI)
{
}                         

Device::Device(const QString &address, const QString &alias, quint32 deviceClass,
               const QString &icon, bool legacyPairing, const QString &name, bool paired,
               short RSSI, QObject *parent)
    : QObject(parent)
    , d(new Private(address, alias, deviceClass, icon, legacyPairing, name, paired, RSSI))
{
}

Device::~Device()
{
    delete d;
}

QString Device::getAddress() const
{
    return d->m_address;
}

QString Device::getAlias() const
{
    return d->m_alias;
}

quint32 Device::getDeviceClass() const
{
    return d->m_deviceClass;
}

QString Device::getIcon() const
{
    return d->m_icon;
}

bool Device::hasLegacyPairing() const
{
    return d->m_legacyPairing;
}

QString Device::getName() const
{
    return d->m_name;
}

bool Device::isPaired() const
{
    return d->m_paired;
}

short Device::getRSSI() const
{
    return d->m_RSSI;
}

}
