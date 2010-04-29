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

#include "bluedevil.h"
#include "bluedeviladapter.h"

#include <bluez.h>

#include <QtCore/QHash>

static BlueDevil *instance = 0;

class BlueDevil::Private
{
public:
    Private(BlueDevil *q);

    void _k_adapterAdded(const QDBusObjectPath &objectPath);
    void _k_adapterRemoved(const QDBusObjectPath &objectPath);
    void _k_defaultAdapterChanged(const QDBusObjectPath &objectPath);
    void _k_propertyChanged(const QString &property, const QDBusVariant &value);

    QHash<QString, BlueDevilAdapter*>  m_adapterMap;
    OrgBluezManagerInterface          *m_bluezManagerInterface;

    BlueDevil *m_q;
};

BlueDevil::Private::Private(BlueDevil *q)
    : m_q(q)
{
}

void BlueDevil::Private::_k_adapterAdded(const QDBusObjectPath &objectPath)
{
    emit m_q->adapterAdded(BlueDevilAdapter(objectPath.path()));
}

void BlueDevil::Private::_k_adapterRemoved(const QDBusObjectPath &objectPath)
{
    emit m_q->adapterRemoved(BlueDevilAdapter(objectPath.path()));
}

void BlueDevil::Private::_k_defaultAdapterChanged(const QDBusObjectPath &objectPath)
{
    emit m_q->defaultAdapterChanged(BlueDevilAdapter(objectPath.path()));
}

void BlueDevil::Private::_k_propertyChanged(const QString &property, const QDBusVariant &newValue)
{
    emit m_q->propertyChanged(property, newValue);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BlueDevil::BlueDevil(QObject *parent)
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

BlueDevil::~BlueDevil()
{
    delete d->m_bluezManagerInterface;
    delete d;
}

BlueDevil* BlueDevil::self()
{
    if (!instance) {
        instance = new BlueDevil;
    }
    return instance;
}

BlueDevilAdapter BlueDevil::defaultAdapter() const
{
    return BlueDevilAdapter(d->m_bluezManagerInterface->DefaultAdapter().value().path());
}

QList<BlueDevilAdapter> BlueDevil::listAdapters() const
{
    QList<BlueDevilAdapter> res;

    Q_FOREACH (const QDBusObjectPath &objectPath, d->m_bluezManagerInterface->ListAdapters().value()) {
        res << BlueDevilAdapter(objectPath.path());
    }

    return res;
}

#include "bluedevil.moc"
