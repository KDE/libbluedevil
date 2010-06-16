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

#include <QtCore/QVariantList>

#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>

#define SERVICE "org.bluez"
#define ADAPTER_IFACE "org.bluez.Adapter"

namespace BlueDevil {

class Adapter::Private
{
public:
    QString m_adapterPath;
};

Adapter::Adapter(const QString &adapterPath)
    : d(new Private)
{
    d->m_adapterPath = adapterPath;
}

Adapter::~Adapter()
{
    delete d;
}

QString Adapter::adapterPath() const
{
    return d->m_adapterPath;
}

QString Adapter::address() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "GetProperties"));
    QVariantMap res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["Address"].toString();
}

void Adapter::setName(const QString &name)
{
    QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "SetProperty"));

    QVariantList arguments;
    arguments << QString("Name");
    arguments << name;
    msg.setArguments(arguments);

    QDBusConnection::systemBus().send(msg);
}

QString Adapter::name() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "GetProperties"));
    QVariantMap res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["Name"].toString();
}

void Adapter::setPowered(bool powered)
{
    QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "SetProperty"));

    QVariantList arguments;
    arguments << QString("Powered");
    arguments << powered;
    msg.setArguments(arguments);

    QDBusConnection::systemBus().send(msg);
}

bool Adapter::isPowered() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "GetProperties"));
    QVariantMap res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["Powered"].toBool();
}

void Adapter::setDiscoverable(bool discoverable)
{
    QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "SetProperty"));

    QVariantList arguments;
    arguments << QString("Discoverable");
    arguments << discoverable;
    msg.setArguments(arguments);

    QDBusConnection::systemBus().send(msg);
}

bool Adapter::isDiscoverable() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "GetProperties"));
    QVariantMap res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["Discoverable"].toBool();
}

void Adapter::setPairable(bool pairable)
{
    QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "SetProperty"));

    QVariantList arguments;
    arguments << QString("Pairable");
    arguments << pairable;
    msg.setArguments(arguments);

    QDBusConnection::systemBus().send(msg);
}

bool Adapter::isPairable() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "GetProperties"));
    QVariantMap res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["Pairable"].toBool();
}

void Adapter::setPaireableTimeout(quint32 paireableTimeout)
{
    QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "SetProperty"));

    QVariantList arguments;
    arguments << QString("PaireableTimeout");
    arguments << paireableTimeout;
    msg.setArguments(arguments);

    QDBusConnection::systemBus().send(msg);
}

quint32 Adapter::paireableTimeout() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "GetProperties"));
    QVariantMap res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["PaireableTimeout"].toBool();
}

void Adapter::setDiscoverableTimeout(quint32 discoverableTimeout)
{
    QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "SetProperty"));

    QVariantList arguments;
    arguments << QString("DiscoverableTimeout");
    arguments << discoverableTimeout;
    msg.setArguments(arguments);

    QDBusConnection::systemBus().send(msg);
}

quint32 Adapter::discoverableTimeout() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "GetProperties"));
    QVariantMap res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["DiscoverableTimeout"].toUInt();
}

bool Adapter::isDiscovering() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall(SERVICE, d->m_adapterPath, ADAPTER_IFACE, "GetProperties"));
    QVariantMap res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["Discovering"].toBool();
}

Adapter &Adapter::operator=(const Adapter& rhs)
{
    d->m_adapterPath = rhs.d->m_adapterPath;
    return *this;
}

bool Adapter::operator==(const Adapter &rhs) const
{
    return d->m_adapterPath == rhs.d->m_adapterPath;
}

bool Adapter::operator!=(const Adapter &rhs) const
{
    return d->m_adapterPath != rhs.d->m_adapterPath;
}

}
