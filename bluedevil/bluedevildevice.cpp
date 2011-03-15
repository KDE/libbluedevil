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

#include "bluedevildevice.h"
#include "bluedeviladapter.h"

#include "bluedevil/bluezdevice.h"

#include <QtCore/QString>
#include <QtCore/QThreadPool>

#define ENSURE_PROPERTIES_FETCHED if (!d->m_propertiesFetched) { \
                                      d->fetchProperties();      \
                                  }

namespace BlueDevil {

class Task
    : public QRunnable
{
public:
    Task(Device *device, const char *slot)
        : m_device(device)
        , m_slot(slot)
    {
    }

    void run() {
        // We happen to be cool, so asyncCall is called: asyncCall(device, SLOT(method())). This
        // makes slot to be "1method()", and invokeMethod does not like this, so we have to transform
        // it to "method".
        QMetaObject::invokeMethod(m_device, m_slot.mid(1, m_slot.count() - 3).toLatin1().data(), Qt::DirectConnection);
    }

private:
    Device  *m_device;
    QString  m_slot;
};

void asyncCall(Device *device, const char *slot)
{
    QThreadPool::globalInstance()->start(new Task(device, slot));
}

/**
 * @internal
 */
class Device::Private
{
public:
    Private(const QString &address, const QString &alias, quint32 deviceClass, const QString &icon,
            bool legacyPairing, const QString &name, bool paired, Device *q);
    Private(Device *q);
    ~Private();

    void fetchProperties();

    bool _k_ensureDeviceCreated(const QString &busDevicePath = QString());
    void _k_propertyChanged(const QString &property, const QDBusVariant &value);

    OrgBluezDeviceInterface *m_bluezDeviceInterface;
    Adapter                 *m_adapter;

    // Bluez cached properties
    QString     m_address;
    QString     m_name;
    QString     m_icon;
    quint32     m_deviceClass;
    QStringList m_UUIDs;
    bool        m_paired;
    bool        m_connected;
    bool        m_trusted;
    bool        m_blocked;
    QString     m_alias;
    bool        m_legacyPairing;
    bool        m_propertiesFetched;
    bool        m_registrationOnBusRejected; // used for avoid trying to register this device more
                                             // than one time on the bus.

    Device *const m_q;
};

Device::Private::Private(const QString &address, const QString &alias, quint32 deviceClass,
                         const QString &icon, bool legacyPairing, const QString &name, bool paired,
                         Device *q)
    : m_bluezDeviceInterface(0)
    , m_address(address)
    , m_name(name)
    , m_icon(icon)
    , m_deviceClass(deviceClass)
    , m_alias(alias)
    , m_paired(paired)
    , m_connected(false)
    , m_trusted(false)
    , m_blocked(false)
    , m_legacyPairing(legacyPairing)
    , m_propertiesFetched(false)
    , m_registrationOnBusRejected(false)
    , m_q(q)
{
}

Device::Private::Private(Device *q)
    : m_bluezDeviceInterface(0)
    , m_propertiesFetched(false)
    , m_registrationOnBusRejected(false)
    , m_q(q)
{
}

Device::Private::~Private()
{
    delete m_bluezDeviceInterface;
}

bool Device::Private::_k_ensureDeviceCreated(const QString &busDevicePath)
{
    if (m_q->sender()) {
        static_cast<QObject*>(m_q)->disconnect(m_adapter, SIGNAL(pairedDeviceCreated(QString)), m_q, SLOT(_k_ensureDeviceCreated(QString)));
    }

    if (m_registrationOnBusRejected) {
        return false;
    }

    if (!m_bluezDeviceInterface) {
        QString devicePath;

        if (busDevicePath.isEmpty()) {
            devicePath = m_adapter->findDevice(m_address);
            if (devicePath.isEmpty()) {
                devicePath = m_adapter->createDevice(m_address);
                if (devicePath.isEmpty()) {
                    m_registrationOnBusRejected = true;
                    return false;
                }
            }
        } else {
            devicePath = busDevicePath;
        }

        m_bluezDeviceInterface = new OrgBluezDeviceInterface("org.bluez",
                                                             devicePath,
                                                             QDBusConnection::systemBus(),
                                                             m_q);

        connect(m_bluezDeviceInterface, SIGNAL(DisconnectRequested()), m_q, SIGNAL(disconnectRequested()));
        connect(m_bluezDeviceInterface, SIGNAL(PropertyChanged(QString,QDBusVariant)),
                m_q, SLOT(_k_propertyChanged(QString,QDBusVariant)));

        const QVariantMap data = m_bluezDeviceInterface->GetProperties().value();
        m_paired = data["Paired"].toBool();
        m_connected = data["Connected"].toBool();
        m_trusted = data["Trusted"].toBool();
        m_blocked = data["Blocked"].toBool();
        m_address = data["Address"].toString();
        m_alias = data["Alias"].toString();
        m_deviceClass = data["Class"].toUInt();
        m_icon = data["Icon"].toString();
        m_legacyPairing = data["LegacyPairing"].toBool();
        m_name = data["Name"].toString();
        m_UUIDs.clear();
        Q_FOREACH (const QString &UUID, data["UUIDs"].toStringList()) {
            m_UUIDs << UUID.toUpper();
        }
        m_adapter->addDeviceWithUBI(devicePath, m_q);
    }

    return true;
}

void Device::Private::fetchProperties()
{
    if (!_k_ensureDeviceCreated()) {
        return;
    }
    m_propertiesFetched = true;
}

void Device::Private::_k_propertyChanged(const QString &property, const QDBusVariant &value)
{
    if (property == "Paired") {
        m_paired = value.variant().toBool();
        emit m_q->pairedChanged(m_paired);
    } else if (property == "Connected") {
        m_connected = value.variant().toBool();
        emit m_q->connectedChanged(m_connected);
    } else if (property == "Trusted") {
        m_trusted = value.variant().toBool();
        emit m_q->trustedChanged(m_trusted);
    } else if (property == "Blocked") {
        m_blocked = value.variant().toBool();
        emit m_q->blockedChanged(m_blocked);
    } else if (property == "Alias") {
        m_alias = value.variant().toString();
        emit m_q->aliasChanged(m_alias);
    } else if (property == "Name") {
        m_name = value.variant().toString();
        emit m_q->nameChanged(m_name);
    } else if (property == "UUIDs") {
        m_UUIDs.clear();
        Q_FOREACH (const QString &UUID, value.variant().toStringList()) {
            m_UUIDs << UUID.toUpper();
        }
        emit m_q->UUIDsChanged(m_UUIDs);
    }
    emit m_q->propertyChanged(property, value.variant());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Device::Device(const QString &address, const QString &alias, quint32 deviceClass,
               const QString &icon, bool legacyPairing, const QString &name, bool paired,
               Adapter *adapter)
    : QObject(adapter)
    , d(new Private(address, alias, deviceClass, icon, legacyPairing, name, paired, this))
{
    d->m_adapter = adapter;
    qRegisterMetaType<BlueDevil::QUInt32StringMap>("BlueDevil::QUInt32StringMap");
    qDBusRegisterMetaType<BlueDevil::QUInt32StringMap>();
}

Device::Device(const QString &pathOrAddress, Type type, Adapter *adapter)
    : QObject(adapter)
    , d(new Private(this))
{
    d->m_adapter = adapter;
    qRegisterMetaType<BlueDevil::QUInt32StringMap>("BlueDevil::QUInt32StringMap");
    qDBusRegisterMetaType<BlueDevil::QUInt32StringMap>();
    if (type == DevicePath) {
        if (!d->_k_ensureDeviceCreated(pathOrAddress)) {
            return;
        }
    } else {
        d->m_address = pathOrAddress;
        if (!d->_k_ensureDeviceCreated()) {
            return;
        }
    }
}

Device::~Device()
{
    delete d;
}

void Device::pair(const QString &agentPath, Adapter::RegisterCapability registerCapability) const
{
    QString capability;

    switch (registerCapability) {
        case Adapter::DisplayOnly:
            capability = "DisplayOnly";
            break;
        case Adapter::DisplayYesNo:
            capability = "DisplayYesNo";
            break;
        case Adapter::KeyboardOnly:
            capability = "KeyboardOnly";
            break;
        case Adapter::NoInputNoOutput:
            capability = "NoInputNoOutput";
            break;
        default:
            return;
    }

    connect(d->m_adapter, SIGNAL(pairedDeviceCreated(QString)), this, SLOT(_k_ensureDeviceCreated(QString)));
    d->m_adapter->createPairedDevice(d->m_address, agentPath, capability);
}

Adapter *Device::adapter() const
{
    return d->m_adapter;
}

QString Device::address() const
{
    return d->m_address;
}

QString Device::name() const
{
    if (d->m_name.isEmpty() && !d->_k_ensureDeviceCreated()) {
        return QString();
    }
    return d->m_name;
}

QString Device::friendlyName() const
{
    if (d->m_name.isEmpty() && !d->_k_ensureDeviceCreated()) {
        return QString();
    }
    if (d->m_alias.isEmpty() || d->m_alias == d->m_name) {
        return d->m_name;
    }
    return QString("%1 (%2)").arg(d->m_alias).arg(d->m_name);
}

QString Device::icon() const
{
    if (d->m_icon.isEmpty()) {
        return "preferences-system-bluetooth";
    }
    return d->m_icon;
}

quint32 Device::deviceClass() const
{
    return d->m_deviceClass;
}

bool Device::isPaired() const
{
    return d->m_paired;
}

bool Device::isRegistered() const
{
    return d->m_bluezDeviceInterface;
}

QString Device::alias() const
{
    return d->m_alias;
}

bool Device::hasLegacyPairing() const
{
    return d->m_legacyPairing;
}

QStringList Device::UUIDs()
{
    ENSURE_PROPERTIES_FETCHED
    if (sender()) {
        emit UUIDsResult(this, d->m_UUIDs);
    }
    return d->m_UUIDs;
}

QString Device::UBI()
{
    if (!d->_k_ensureDeviceCreated()) {
        return QString();
    }

    const QString path = d->m_bluezDeviceInterface->path();
    if (sender()) {
        emit UBIResult(this, path);
    }
    return path;
}

bool Device::isConnected()
{
    ENSURE_PROPERTIES_FETCHED
    if (sender()) {
        emit isConnectedResult(this, d->m_connected);
    }
    return d->m_connected;
}

bool Device::isTrusted()
{
    ENSURE_PROPERTIES_FETCHED
    if (sender()) {
        emit isTrustedResult(this, d->m_trusted);
    }
    return d->m_trusted;
}

bool Device::isBlocked()
{
    ENSURE_PROPERTIES_FETCHED
    if (sender()) {
        emit isBlockedResult(this, d->m_blocked);
    }
    return d->m_blocked;
}

bool Device::registerDevice()
{
    const bool res = d->_k_ensureDeviceCreated();
    emit registerDeviceResult(this, res);
    return res;
}

void Device::setTrusted(bool trusted)
{
    if (!d->_k_ensureDeviceCreated()) {
        return;
    }
    d->m_bluezDeviceInterface->SetProperty("Trusted", QDBusVariant(trusted));
}

void Device::setBlocked(bool blocked)
{
    if (!d->_k_ensureDeviceCreated()) {
        return;
    }
    d->m_bluezDeviceInterface->SetProperty("Blocked", QDBusVariant(blocked));
}

void Device::setAlias(const QString &alias)
{
    if (!d->_k_ensureDeviceCreated()) {
        return;
    }
    d->m_bluezDeviceInterface->SetProperty("Alias", QDBusVariant(alias));
}

QUInt32StringMap Device::discoverServices(const QString &pattern)
{
    if (!d->_k_ensureDeviceCreated()) {
        return QUInt32StringMap();
    }
    const QUInt32StringMap res = d->m_bluezDeviceInterface->DiscoverServices(pattern).value();
    if (sender()) {
        emit discoverServicesResult(this, res);
    }
    return res;
}

void Device::cancelDiscovery()
{
    if (d->m_bluezDeviceInterface) {
        d->m_bluezDeviceInterface->CancelDiscovery();
    }
}

void Device::disconnect()
{
    if (d->m_bluezDeviceInterface) {
        d->m_bluezDeviceInterface->Disconnect();
    }
}

}

#include "bluedevildevice.moc"

