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

class BlueDevilAdapter::Private
{
public:
    QString m_adapterPath;
};

BlueDevilAdapter::BlueDevilAdapter(const QString &adapterPath)
    : d(new Private)
{
    d->m_adapterPath = adapterPath;
}

BlueDevilAdapter::~BlueDevilAdapter()
{
    delete d;
}

QString BlueDevilAdapter::adapterPath() const
{
    return d->m_adapterPath;
}

QString BlueDevilAdapter::address() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "GetProperties"));
    QMap<QString, QVariant> res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["Address"].toString();
}

void BlueDevilAdapter::setName(const QString &name)
{
    QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "SetProperty"));

    QVariantList arguments;
    arguments << QString("Name");
    arguments << name;
    msg.setArguments(arguments);

    QDBusConnection::systemBus().send(msg);
}

QString BlueDevilAdapter::name() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "GetProperties"));
    QMap<QString, QVariant> res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["Name"].toString();
}

void BlueDevilAdapter::setPowered(bool powered)
{
    QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "SetProperty"));

    QVariantList arguments;
    arguments << QString("Powered");
    arguments << powered;
    msg.setArguments(arguments);

    QDBusConnection::systemBus().send(msg);
}

bool BlueDevilAdapter::isPowered() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "GetProperties"));
    QMap<QString, QVariant> res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["Powered"].toBool();
}

void BlueDevilAdapter::setDiscoverable(bool discoverable)
{
    QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "SetProperty"));

    QVariantList arguments;
    arguments << QString("Discoverable");
    arguments << discoverable;
    msg.setArguments(arguments);

    QDBusConnection::systemBus().send(msg);
}

bool BlueDevilAdapter::isDiscoverable() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "GetProperties"));
    QMap<QString, QVariant> res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["Discoverable"].toBool();
}

void BlueDevilAdapter::setPairable(bool pairable)
{
    QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "SetProperty"));

    QVariantList arguments;
    arguments << QString("Pairable");
    arguments << pairable;
    msg.setArguments(arguments);

    QDBusConnection::systemBus().send(msg);
}

bool BlueDevilAdapter::isPairable() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "GetProperties"));
    QMap<QString, QVariant> res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["Pairable"].toBool();
}

void BlueDevilAdapter::setPaireableTimeout(quint32 paireableTimeout)
{
    QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "SetProperty"));

    QVariantList arguments;
    arguments << QString("PaireableTimeout");
    arguments << paireableTimeout;
    msg.setArguments(arguments);

    QDBusConnection::systemBus().send(msg);
}

quint32 BlueDevilAdapter::paireableTimeout() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "GetProperties"));
    QMap<QString, QVariant> res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["PaireableTimeout"].toBool();
}

void BlueDevilAdapter::setDiscoverableTimeout(quint32 discoverableTimeout)
{
    QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "SetProperty"));

    QVariantList arguments;
    arguments << QString("DiscoverableTimeout");
    arguments << discoverableTimeout;
    msg.setArguments(arguments);

    QDBusConnection::systemBus().send(msg);
}

quint32 BlueDevilAdapter::discoverableTimeout() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "GetProperties"));
    QMap<QString, QVariant> res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["DiscoverableTimeout"].toUInt();
}

bool BlueDevilAdapter::isDiscovering() const
{
    const QDBusMessage msg(QDBusMessage::createMethodCall("org.bluez", d->m_adapterPath, QString(), "GetProperties"));
    QMap<QString, QVariant> res = QDBusConnection::systemBus().call(msg).arguments().first().toMap();

    return res["Discovering"].toBool();
}

BlueDevilAdapter &BlueDevilAdapter::operator=(const BlueDevilAdapter& rhs)
{
    d->m_adapterPath = rhs.d->m_adapterPath;
    return *this;
}

bool BlueDevilAdapter::operator==(const BlueDevilAdapter &rhs) const
{
    return d->m_adapterPath == rhs.d->m_adapterPath;
}

bool BlueDevilAdapter::operator!=(const BlueDevilAdapter &rhs) const
{
    return d->m_adapterPath != rhs.d->m_adapterPath;
}
