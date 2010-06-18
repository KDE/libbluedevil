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

#include "bluedeviladapter.h"
#include "bluedevildevice.h"

#include <bluezadapter.h>

#define ENSURE_PROPERTIES_FETCHED if (!d->m_propertiesFetched) { \
                                      d->fetchProperties();      \
                                  }

namespace BlueDevil {

class Adapter::Private
{
public:
    Private(Adapter *q);
    ~Private();

    void fetchProperties();

    void _k_deviceCreated(const QDBusObjectPath &objectPath);
    void _k_deviceFound(const QString &address, const QVariantMap &map);
    void _k_deviceDisappeared(const QString &address);
    void _k_deviceRemoved(const QDBusObjectPath &objectPath);
    void _k_propertyChanged(const QString &property, const QDBusVariant &value);

    OrgBluezAdapterInterface *m_bluezAdapterInterface;
    QHash<QString, Device*>   m_devicesHash;

    // Bluez cached properties
    QString m_address;
    QString m_name;
    quint32 m_class;
    bool    m_powered;
    bool    m_discoverable;
    bool    m_pairable;
    quint32 m_pairableTimeout;
    quint32 m_discoverableTimeout;
    bool    m_discovering;
    bool    m_propertiesFetched;

    Adapter *const m_q;
};

Adapter::Private::Private(Adapter *q)
    : m_propertiesFetched(false)
    , m_q(q)
{
}

Adapter::Private::~Private()
{
    delete m_bluezAdapterInterface;
}

void Adapter::Private::fetchProperties()
{
    QVariantMap properties = m_bluezAdapterInterface->GetProperties().value();
    m_address = properties["Address"].toString();
    m_name = properties["Name"].toString();
    m_class = properties["Class"].toUInt();
    m_powered = properties["Powered"].toBool();
    m_discoverable = properties["Discoverable"].toBool();
    m_pairable = properties["Pairable"].toBool();
    m_pairableTimeout = properties["PairableTimeout"].toUInt();
    m_discoverableTimeout = properties["DiscoverableTimeout"].toUInt();
    m_discovering = properties["Discovering"].toBool();
    m_propertiesFetched = true;
}

void Adapter::Private::_k_deviceCreated(const QDBusObjectPath &objectPath)
{
    Q_UNUSED(objectPath)
}

void Adapter::Private::_k_deviceFound(const QString &address, const QVariantMap &map)
{
    Device *const device = new Device(address, map["Alias"].toString(), map["Class"].toUInt(),
                                      map["Icon"].toString(), map["LegacyPairing"].toBool(),
                                      map["Name"].toString(), map["Paired"].toBool(), m_q);
    m_devicesHash.insert(address, device);
    emit m_q->deviceFound(device);
}

void Adapter::Private::_k_deviceDisappeared(const QString &address)
{
    Device *const device = m_devicesHash.take(address);
    if (device) {
        emit m_q->deviceDisappeared(device);
        delete device;
    }
}

void Adapter::Private::_k_deviceRemoved(const QDBusObjectPath &objectPath)
{
    Q_UNUSED(objectPath)
}

void Adapter::Private::_k_propertyChanged(const QString &property, const QDBusVariant &value)
{
    if (property == "Name") {
        m_name = value.variant().toString();
    } else if (property == "Powered") {
        m_powered = value.variant().toBool();
    } else if (property == "Discoverable") {
        m_discoverable = value.variant().toBool();
    } else if (property == "Pairable") {
        m_pairable = value.variant().toBool();
    } else if (property == "PairableTimeout") {
        m_pairableTimeout = value.variant().toUInt();
    } else if (property == "DiscoverableTimeout") {
        m_discoverableTimeout = value.variant().toUInt();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Adapter::Adapter(const QString &adapterPath, QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    d->m_bluezAdapterInterface = new OrgBluezAdapterInterface("org.bluez", adapterPath, QDBusConnection::systemBus(), this);

    connect(d->m_bluezAdapterInterface, SIGNAL(DeviceCreated(QDBusObjectPath)),
            this, SLOT(_k_deviceCreated(QDBusObjectPath)));
    connect(d->m_bluezAdapterInterface, SIGNAL(DeviceFound(QString,QVariantMap)),
            this, SLOT(_k_deviceFound(QString,QVariantMap)));
    connect(d->m_bluezAdapterInterface, SIGNAL(DeviceDisappeared(QString)),
            this, SLOT(_k_deviceDisappeared(QString)));
    connect(d->m_bluezAdapterInterface, SIGNAL(DeviceRemoved(QDBusObjectPath)),
            this, SLOT(_k_deviceRemoved(QDBusObjectPath)));
    connect(d->m_bluezAdapterInterface, SIGNAL(PropertyChanged(QString,QDBusVariant)),
            this, SLOT(_k_propertyChanged(QString,QDBusVariant)));
}

Adapter::~Adapter()
{
    delete d;
}

QString Adapter::address() const
{
    ENSURE_PROPERTIES_FETCHED
    return d->m_address;
}

QString Adapter::name() const
{
    ENSURE_PROPERTIES_FETCHED
    return d->m_name;
}

void Adapter::setName(const QString &name)
{
    d->m_bluezAdapterInterface->SetProperty("Name", QDBusVariant(name)).waitForFinished();
}

quint32 Adapter::adapterClass() const
{
    ENSURE_PROPERTIES_FETCHED
    return d->m_class;
}

bool Adapter::isPowered() const
{
    ENSURE_PROPERTIES_FETCHED
    return d->m_powered;
}

void Adapter::setPowered(bool powered)
{
    d->m_bluezAdapterInterface->SetProperty("Powered", QDBusVariant(powered)).waitForFinished();
}

bool Adapter::isDiscoverable() const
{
    ENSURE_PROPERTIES_FETCHED
    return d->m_discoverable;
}

void Adapter::setDiscoverable(bool discoverable)
{
    d->m_bluezAdapterInterface->SetProperty("Discoverable", QDBusVariant(discoverable)).waitForFinished();
}

bool Adapter::isPairable() const
{
    ENSURE_PROPERTIES_FETCHED
    return d->m_pairable;
}

void Adapter::setPairable(bool pairable)
{
    d->m_bluezAdapterInterface->SetProperty("Pairable", QDBusVariant(pairable)).waitForFinished();
}

quint32 Adapter::paireableTimeout() const
{
    ENSURE_PROPERTIES_FETCHED
    return d->m_pairableTimeout;
}

void Adapter::setPaireableTimeout(quint32 paireableTimeout)
{
    d->m_bluezAdapterInterface->SetProperty("PaireableTimeout", QDBusVariant(paireableTimeout)).waitForFinished();
}

quint32 Adapter::discoverableTimeout() const
{
    ENSURE_PROPERTIES_FETCHED
    return d->m_discoverableTimeout;
}

void Adapter::setDiscoverableTimeout(quint32 discoverableTimeout)
{
    d->m_bluezAdapterInterface->SetProperty("DiscoverableTimeout", QDBusVariant(discoverableTimeout)).waitForFinished();
}

bool Adapter::isDiscovering() const
{
    ENSURE_PROPERTIES_FETCHED
    return d->m_discovering;
}

void Adapter::startDiscovery() const
{
    d->m_bluezAdapterInterface->StartDiscovery().waitForFinished();
}

void Adapter::stopDiscovery() const
{
    d->m_bluezAdapterInterface->StopDiscovery().waitForFinished();
}

QList<Device*> Adapter::foundDevices() const
{
    return d->m_devicesHash.values();
}

QDBusObjectPath Adapter::findDevice(const QString &address) const
{
    return d->m_bluezAdapterInterface->FindDevice(address).value();
}

QDBusObjectPath Adapter::createDevice(const QString &address) const
{
    return d->m_bluezAdapterInterface->CreateDevice(address).value();
}

}

#include "bluedeviladapter.moc"
