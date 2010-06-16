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

namespace BlueDevil {

class Adapter::Private
{
public:
    Private(Adapter *q);

    OrgBluezAdapterInterface *m_bluezAdapterInterface;
    QString                   m_adapterPath;

    Adapter *const m_q;
};

Adapter::Private::Private(Adapter *q)
    : m_q(q)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Adapter::Adapter(const QString &adapterPath)
    : QObject()
    , d(new Private(this))
{
    d->m_bluezAdapterInterface = new OrgBluezAdapterInterface("org.bluez", adapterPath, QDBusConnection::systemBus(), this);
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
    return d->m_bluezAdapterInterface->GetProperties().value()["Address"].toString();
}

void Adapter::setName(const QString &name)
{
    d->m_bluezAdapterInterface->SetProperty("Name", QDBusVariant(name));
}

QString Adapter::name() const
{
    return d->m_bluezAdapterInterface->GetProperties().value()["Name"].toString();
}

void Adapter::setPowered(bool powered)
{
    d->m_bluezAdapterInterface->SetProperty("Powered", QDBusVariant(powered));
}

bool Adapter::isPowered() const
{
    return d->m_bluezAdapterInterface->GetProperties().value()["Powered"].toBool();
}

void Adapter::setDiscoverable(bool discoverable)
{
    d->m_bluezAdapterInterface->SetProperty("Discoverable", QDBusVariant(discoverable));
}

bool Adapter::isDiscoverable() const
{
    return d->m_bluezAdapterInterface->GetProperties().value()["Discoverable"].toBool();
}

void Adapter::setPairable(bool pairable)
{
    d->m_bluezAdapterInterface->SetProperty("Pairable", QDBusVariant(pairable));
}

bool Adapter::isPairable() const
{
    return d->m_bluezAdapterInterface->GetProperties().value()["Pairable"].toBool();
}

void Adapter::setPaireableTimeout(quint32 paireableTimeout)
{
    d->m_bluezAdapterInterface->SetProperty("PaireableTimeout", QDBusVariant(paireableTimeout));
}

quint32 Adapter::paireableTimeout() const
{
    return d->m_bluezAdapterInterface->GetProperties().value()["PaireableTimeout"].toUInt();
}

void Adapter::setDiscoverableTimeout(quint32 discoverableTimeout)
{
    d->m_bluezAdapterInterface->SetProperty("DiscoverableTimeout", QDBusVariant(discoverableTimeout));
}

quint32 Adapter::discoverableTimeout() const
{
    return d->m_bluezAdapterInterface->GetProperties().value()["DiscoverableTimeout"].toUInt();
}

bool Adapter::isDiscovering() const
{
    return d->m_bluezAdapterInterface->GetProperties().value()["Discovering"].toBool();
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
