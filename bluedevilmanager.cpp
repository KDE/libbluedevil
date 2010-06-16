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

#include "bluedevilmanager.h"
#include "bluedeviladapter.h"

#include <bluezmanager.h>

#include <QtCore/QHash>

namespace BlueDevil {

static Manager *instance = 0;

class Manager::Private
{
public:
    Private(Manager *q);

    void _k_adapterAdded(const QDBusObjectPath &objectPath);
    void _k_adapterRemoved(const QDBusObjectPath &objectPath);
    void _k_defaultAdapterChanged(const QDBusObjectPath &objectPath);
    void _k_propertyChanged(const QString &property, const QDBusVariant &value);

    OrgBluezManagerInterface *m_bluezManagerInterface;

    Manager *const m_q;
};

Manager::Private::Private(Manager *q)
    : m_q(q)
{
}

void Manager::Private::_k_adapterAdded(const QDBusObjectPath &objectPath)
{
    emit m_q->adapterAdded(Adapter(objectPath.path()));
}

void Manager::Private::_k_adapterRemoved(const QDBusObjectPath &objectPath)
{
    emit m_q->adapterRemoved(Adapter(objectPath.path()));
}

void Manager::Private::_k_defaultAdapterChanged(const QDBusObjectPath &objectPath)
{
    emit m_q->defaultAdapterChanged(Adapter(objectPath.path()));
}

void Manager::Private::_k_propertyChanged(const QString &property, const QDBusVariant &newValue)
{
    emit m_q->propertyChanged(property, newValue);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Manager::Manager(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    d->m_bluezManagerInterface = new OrgBluezManagerInterface("org.bluez", "/", QDBusConnection::systemBus(), this);

    connect(d->m_bluezManagerInterface, SIGNAL(AdapterAdded(QDBusObjectPath)),
            this, SLOT(_k_adapterAdded(QDBusObjectPath)));
    connect(d->m_bluezManagerInterface, SIGNAL(AdapterRemoved(QDBusObjectPath)),
            this, SLOT(_k_adapterRemoved(QDBusObjectPath)));
    connect(d->m_bluezManagerInterface, SIGNAL(DefaultAdapterChanged(QDBusObjectPath)),
            this, SLOT(_k_defaultAdapterChanged(QDBusObjectPath)));
    connect(d->m_bluezManagerInterface, SIGNAL(PropertyChanged(QString,QDBusVariant)),
            this, SLOT(_k_propertyChanged(QString,QDBusVariant)));
}

Manager::~Manager()
{
    delete d->m_bluezManagerInterface;
    delete d;
}

Manager* Manager::self()
{
    if (!instance) {
        instance = new Manager;
    }
    return instance;
}

Adapter *Manager::defaultAdapter() const
{
    return new Adapter(d->m_bluezManagerInterface->DefaultAdapter().value().path());
}

QList<Adapter*> Manager::listAdapters() const
{
    QList<Adapter*> res;

    Q_FOREACH (const QDBusObjectPath &objectPath, d->m_bluezManagerInterface->ListAdapters().value()) {
        res << new Adapter(objectPath.path());
    }

    return res;
}

}

#include "bluedevilmanager.moc"
