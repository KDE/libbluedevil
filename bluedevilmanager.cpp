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
    ~Private();

    void _k_adapterAdded(const QDBusObjectPath &objectPath);
    void _k_adapterRemoved(const QDBusObjectPath &objectPath);
    void _k_defaultAdapterChanged(const QDBusObjectPath &objectPath);

    OrgBluezManagerInterface *m_bluezManagerInterface;
    Adapter                  *m_defaultAdapter;
    QList<Adapter*>           m_adapterList;

    Manager *const m_q;
};

Manager::Private::Private(Manager *q)
    : m_defaultAdapter(0)
    , m_q(q)
{
}

Manager::Private::~Private()
{
    delete m_bluezManagerInterface;
}

void Manager::Private::_k_adapterAdded(const QDBusObjectPath &objectPath)
{
    Adapter adapter(objectPath.path());
    emit m_q->adapterAdded(&adapter);
}

void Manager::Private::_k_adapterRemoved(const QDBusObjectPath &objectPath)
{
    Adapter adapter(objectPath.path());
    emit m_q->adapterRemoved(&adapter);
}

void Manager::Private::_k_defaultAdapterChanged(const QDBusObjectPath &objectPath)
{
    Adapter adapter(objectPath.path());
    emit m_q->defaultAdapterChanged(&adapter);
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
}

Manager::~Manager()
{
    delete d;
}

Manager* Manager::self()
{
    if (!instance) {
        instance = new Manager;
    }
    return instance;
}

void Manager::release()
{
    delete instance;
    instance = 0;
}

Adapter *Manager::defaultAdapter() const
{
    delete d->m_defaultAdapter;
    d->m_defaultAdapter = 0;

    if (!QDBusConnection::systemBus().isConnected()) {
        return d->m_defaultAdapter;
    }

    const QString adapterPath = d->m_bluezManagerInterface->DefaultAdapter().value().path();
    if (!adapterPath.isEmpty()) {
        d->m_defaultAdapter = new Adapter(adapterPath, const_cast<Manager*>(this));
    }

    return d->m_defaultAdapter;
}

QList<Adapter*> Manager::listAdapters() const
{
    qDeleteAll(d->m_adapterList);
    d->m_adapterList.clear();

    if (!QDBusConnection::systemBus().isConnected()) {
        return d->m_adapterList;
    }

    Q_FOREACH (const QDBusObjectPath &objectPath, d->m_bluezManagerInterface->ListAdapters().value()) {
        d->m_adapterList << new Adapter(objectPath.path(), const_cast<Manager*>(this));
    }

    return d->m_adapterList;
}

}

#include "bluedevilmanager.moc"
