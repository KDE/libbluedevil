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

#include "bluedevil/bluezdevice1.h"
#include "bluedevil/dbusproperties.h"

#include <QtCore/QString>
#include <QtCore/QThreadPool>

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
    Private(BlueDevil::Device *q, const QString &path);
    ~Private();

    void _k_propertyChanged(const QString &interface_name, const QVariantMap &changed_values, const QStringList &invalidated_values);
    QStringList _k_stringListToUpper(const QStringList & list);

    org::bluez::Device1                *m_bluezDeviceInterface;
    org::freedesktop::DBus::Properties *m_dbuspropertiesInterface;
    Adapter                            *m_adapter;

    // Bluez cached properties
    bool        m_registrationOnBusRejected; // used for avoid trying to register this device more
                                             // than one time on the bus.

    Device *const m_q;
};

Device::Private::Private(Device *q, const QString &path)
    : m_bluezDeviceInterface(0)
    , m_dbuspropertiesInterface(0)
    , m_registrationOnBusRejected(false)
    , m_q(q)
{
  m_bluezDeviceInterface = new org::bluez::Device1("org.bluez",
                                                        path,
                                                        QDBusConnection::systemBus(),
                                                        m_q);

  m_dbuspropertiesInterface = new org::freedesktop::DBus::Properties("org.bluez",path,QDBusConnection::systemBus(),m_q);
}

Device::Private::~Private()
{
    delete m_bluezDeviceInterface;
    delete m_dbuspropertiesInterface;
}

QStringList Device::Private::_k_stringListToUpper(const QStringList& list)
{
    QStringList upperList(list);
    for(int i=0;i<upperList.size();i++) {
      upperList[i] = upperList.value(i).toUpper();
    }
    return upperList;
}

void Device::Private::_k_propertyChanged(const QString &interface_name, const QVariantMap &changed_values, const QStringList &invalidated_values)
{
  QVariantMap::const_iterator i;
  for(i = changed_values.constBegin(); i != changed_values.constEnd(); ++i) {
    QString property = i.key();
    QVariant value = i.value();
    if (property == "Paired") {
        emit m_q->pairedChanged(value.toBool());
    } else if (property == "Connected") {
        emit m_q->connectedChanged(value.toBool());
    } else if (property == "Trusted") {
        emit m_q->trustedChanged(value.toBool());
    } else if (property == "Blocked") {
        emit m_q->blockedChanged(value.toBool());
    } else if (property == "Alias") {
        emit m_q->aliasChanged(value.toString());
    } else if (property == "Name") {
        emit m_q->nameChanged(value.toString());
    } else if (property == "UUIDs") {
        emit m_q->UUIDsChanged(_k_stringListToUpper(value.toStringList()));
    }
    emit m_q->propertyChanged(property, value);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Device::Device(const QString &path, Adapter *adapter)
    : QObject(adapter)
    , d(new Private(this,path))
{
    d->m_adapter = adapter;
    qRegisterMetaType<BlueDevil::QUInt32StringMap>("BlueDevil::QUInt32StringMap");
    qDBusRegisterMetaType<BlueDevil::QUInt32StringMap>();

    connect(d->m_dbuspropertiesInterface,SIGNAL(PropertiesChanged(QString,QVariantMap,QStringList)),this,SLOT(_k_propertyChanged(QString,QVariantMap,QStringList)));

}

Device::~Device()
{
    delete d;
}

void Device::pair() const
{
    d->m_bluezDeviceInterface->Pair();
}

Adapter *Device::adapter() const
{
    return d->m_adapter;
}

QString Device::address() const
{
    return d->m_bluezDeviceInterface->address();
}

QString Device::name() const
{
    return d->m_bluezDeviceInterface->name();
}

QString Device::friendlyName() const
{
    QString alias = d->m_bluezDeviceInterface->alias();
    QString name = d->m_bluezDeviceInterface->name();
    if (alias.isEmpty() || alias == name) {
        return name;
    }
    return QString("%1 (%2)").arg(alias).arg(name);
}

QString Device::icon() const
{
    QString icon = d->m_bluezDeviceInterface->icon();
    if (icon.isEmpty()) {
        return "preferences-system-bluetooth";
    }
    return icon;
}

quint32 Device::deviceClass() const
{
    return d->m_bluezDeviceInterface->deviceClass();
}

bool Device::isPaired() const
{
    return d->m_bluezDeviceInterface->paired();
}

QString Device::alias() const
{
    return d->m_bluezDeviceInterface->alias();
}

bool Device::hasLegacyPairing() const
{
    return d->m_bluezDeviceInterface->legacyPairing();
}

QStringList Device::UUIDs()
{
    QStringList UUIDs = d->_k_stringListToUpper(d->m_bluezDeviceInterface->uUIDs());
    if (sender()) {
        emit UUIDsResult(this, UUIDs);
    }
    return UUIDs;
}

QString Device::UBI()
{
    const QString path = d->m_bluezDeviceInterface->path();
    if (sender()) {
        emit UBIResult(this, path);
    }
    return path;
}

bool Device::isConnected()
{
    bool connected = d->m_bluezDeviceInterface->connected();
    if (sender()) {
        emit isConnectedResult(this, connected);
    }
    return connected;
}

bool Device::isTrusted()
{
    bool trusted = d->m_bluezDeviceInterface->trusted();
    if (sender()) {
        emit isTrustedResult(this, trusted);
    }
    return trusted;
}

bool Device::isBlocked()
{
    bool blocked = d->m_bluezDeviceInterface->blocked();
    if (sender()) {
        emit isBlockedResult(this, blocked);
    }
    return blocked;
}

void Device::setTrusted(bool trusted)
{
    d->m_bluezDeviceInterface->setTrusted(trusted);
}

void Device::setBlocked(bool blocked)
{
    d->m_bluezDeviceInterface->setBlocked(blocked);
}

void Device::setAlias(const QString &alias)
{
    d->m_bluezDeviceInterface->setAlias(alias);
}

void Device::disconnect()
{
    d->m_bluezDeviceInterface->Disconnect();
}

void Device::connectDevice()
{
    d->m_bluezDeviceInterface->Connect();
}

}

#include "moc_bluedevildevice.cpp"
