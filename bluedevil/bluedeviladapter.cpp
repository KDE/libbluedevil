/*****************************************************************************
 * This file is part of the BlueDevil project                                *
 *                                                                           *
 * Copyright (C) 2010 Rafael Fernández López <ereslibre@kde.org>             *
 * Copyright (C) 2010 UFO Coders <info@ufocoders.com>                        *
 *                                                                           *
 * This library is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU Library General Public               *
 * License as published by the Free Software Foundation; either              *
 * version 2 of the License, or (at your option) any later version.          *
 *                                                                           *
 * This library is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public License *
 * along with this library; see the file COPYING.LIB.  If not, write to      *
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 * Boston, MA 02110-1301, USA.                                               *
 *****************************************************************************/

#include "bluedeviladapter.h"
#include "bluedevildevice.h"

#include "bluedevil/bluezadapter1.h"
#include "bluedevil/dbusproperties.h"

namespace BlueDevil {

/**
 * @internal
 */
class Adapter::Private
{
public:
    Private(Adapter *q);
    ~Private();

    void startDiscovery();

    void _k_deviceRemoved(const QString &objectPath);
    void _k_propertyChanged(const QString &property, const QVariantMap &changed_properties, const QStringList &invalidated_properties);
    void _k_devicePropertyChanged(const QString &property, const QVariant &value);

    org::bluez::Adapter1               *m_bluezAdapterInterface;
    org::freedesktop::DBus::Properties *m_dbusPropertiesInterface;

    QMap<QString, Device*>    m_devicesMap;
    QMap<QString, Device*>    m_devicesMapUBIKey;
    QMap<QString, Device*>    m_unpairedDevices;

    bool           m_stableDiscovering;

    Adapter *const m_q;
};

Adapter::Private::Private(Adapter *q)
    : m_stableDiscovering(false)
    , m_q(q)
{
}

Adapter::Private::~Private()
{
    delete m_bluezAdapterInterface;
    delete m_dbusPropertiesInterface;
}

void Adapter::Private::startDiscovery()
{
    m_bluezAdapterInterface->StartDiscovery();
}

void Adapter::Private::_k_deviceRemoved(const QString &objectPath)
{
    Device *const device = m_devicesMapUBIKey.take(objectPath);
    if (device) {
        m_devicesMap.remove(m_devicesMap.key(device));
        m_unpairedDevices.remove(objectPath);
        emit m_q->deviceRemoved(device);
        delete device;
    }
}

void Adapter::Private::_k_propertyChanged(const QString &interface_name, const QVariantMap &changed_properties, const QStringList &invalidated_properties)
{
    QVariantMap::const_iterator i;
    for(i = changed_properties.constBegin(); i != changed_properties.constEnd(); ++i) {
      QVariant value = i.value();
      QString property = i.key();
      if (property == "Alias") {
          emit m_q->nameChanged(value.toString());
      } else if (property == "Powered") {
          emit m_q->poweredChanged(value.toBool());
      } else if (property == "Discoverable") {
          emit m_q->discoverableChanged(value.toBool());
      } else if (property == "Pairable") {
          emit m_q->pairableChanged(value.toBool());
      } else if (property == "PairableTimeout") {
          emit m_q->pairableTimeoutChanged(value.toUInt());
      } else if (property == "DiscoverableTimeout") {
          emit m_q->discoverableTimeoutChanged(value.toUInt());
      } else if (property == "Discovering") {
          emit m_q->discoveringChanged(value.toBool());
      }
      emit m_q->propertyChanged(property, value);
    }
}

void Adapter::Private::_k_devicePropertyChanged(const QString& property, const QVariant& value)
{
    Device *device = qobject_cast<Device*>(m_q->sender());
    Q_ASSERT(device);

    emit m_q->deviceChanged(device);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Adapter::Adapter(const QString &adapterPath, QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    d->m_bluezAdapterInterface = new org::bluez::Adapter1("org.bluez", adapterPath, QDBusConnection::systemBus(), this);
    d->m_dbusPropertiesInterface = new org::freedesktop::DBus::Properties("org.bluez", adapterPath, QDBusConnection::systemBus(), this);

    connect(d->m_dbusPropertiesInterface, SIGNAL(PropertiesChanged(QString,QVariantMap,QStringList)),
            this, SLOT(_k_propertyChanged(QString,QVariantMap,QStringList)));
}

Adapter::~Adapter()
{
    delete d;
}

QString Adapter::address() const
{
    return d->m_bluezAdapterInterface->address();
}

QString Adapter::name() const
{
    return d->m_bluezAdapterInterface->alias();
}

QString Adapter::systemName() const
{
    return d->m_bluezAdapterInterface->name();
}

quint32 Adapter::adapterClass() const
{
    return d->m_bluezAdapterInterface->adapterClass();
}

bool Adapter::isPowered() const
{
    return d->m_bluezAdapterInterface->powered();
}

bool Adapter::isDiscoverable() const
{
    return d->m_bluezAdapterInterface->discoverable();
}

bool Adapter::isPairable() const
{
    return d->m_bluezAdapterInterface->pairable();
}

quint32 Adapter::paireableTimeout() const
{
    return d->m_bluezAdapterInterface->pairableTimeout();
}

quint32 Adapter::discoverableTimeout() const
{
    return d->m_bluezAdapterInterface->discoverableTimeout();
}

bool Adapter::isDiscovering() const
{
    return d->m_bluezAdapterInterface->discovering();
}

QList<Device*> Adapter::unpairedDevices() const
{
    return d->m_unpairedDevices.values();
}

Device *Adapter::deviceForAddress(const QString &address)
{
    if (d->m_devicesMap.contains(address)) {
        return d->m_devicesMap[address];
    }
    return 0;
}

Device *Adapter::deviceForUBI(const QString &UBI)
{
    if (d->m_devicesMapUBIKey.contains(UBI)) {
        return d->m_devicesMapUBIKey[UBI];
    }
    return 0;
}

QStringList Adapter::UUIDs()
{
    QStringList UUIDs = d->m_bluezAdapterInterface->uUIDs();
    for(int i=0;i<UUIDs.size();i++) {
      UUIDs[i] = UUIDs.value(i).toUpper();
    }
    return UUIDs;
}

void Adapter::setName(const QString& name)
{
    d->m_bluezAdapterInterface->setAlias(name);
}

void Adapter::setPowered(bool powered)
{
    d->m_bluezAdapterInterface->setPowered(powered);
}

void Adapter::setDiscoverable(bool discoverable)
{
    d->m_bluezAdapterInterface->setDiscoverable(discoverable);
}

void Adapter::setPairable(bool pairable)
{
    d->m_bluezAdapterInterface->setPairable(pairable);
}

void Adapter::setPaireableTimeout(quint32 paireableTimeout)
{
    d->m_bluezAdapterInterface->setPairableTimeout(paireableTimeout);
}

void Adapter::setDiscoverableTimeout(quint32 discoverableTimeout)
{
    d->m_bluezAdapterInterface->setDiscoverableTimeout(discoverableTimeout);
}

void Adapter::removeDevice(Device *device)
{
    d->m_bluezAdapterInterface->RemoveDevice(QDBusObjectPath(device->UBI()));
}

void Adapter::startDiscovery() const
{
    d->m_stableDiscovering = false;
    d->startDiscovery();
}

void Adapter::startStableDiscovery() const
{
    d->m_stableDiscovering = true;
    d->startDiscovery();
}

void Adapter::stopDiscovery() const
{
    d->m_stableDiscovering = false;
    d->m_bluezAdapterInterface->StopDiscovery();
}

QList< Device* > Adapter::devices()
{
    return d->m_devicesMap.values();
}

void Adapter::addDevice(const QString &objectPath)
{
    Device * device = new Device(objectPath,this);
    d->m_devicesMap.insert(device->address(),device);
    d->m_devicesMapUBIKey.insert(objectPath,device);
    emit deviceFound(device);
    if(!device->isPaired()) {
        d->m_unpairedDevices.insert(objectPath,device);
        emit unpairedDeviceFound(device);
    }

    connect(device, SIGNAL(propertyChanged(QString,QVariant)), SLOT(_k_devicePropertyChanged(QString,QVariant)));
}

void Adapter::removeDevice(const QString &objectPath)
{
    d->_k_deviceRemoved(objectPath);
}

}

#include "bluedeviladapter.moc"
