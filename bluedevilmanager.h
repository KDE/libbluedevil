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

#ifndef BLUEDEVILMANAGER_H
#define BLUEDEVILMANAGER_H

#include <bluedevil_export.h>

#include <QtCore/QObject>
#include <QtDBus/QDBusObjectPath>

namespace BlueDevil {

class Adapter;

class BLUEDEVIL_EXPORT Manager
    : public QObject
{
    Q_OBJECT

public:
    virtual ~Manager();

    static Manager *self();
    static void release();

    /**
     * @return The default adapter. NULL if there is no default adapter.
     */
    Adapter *defaultAdapter() const;

    /**
     * @return A list with all the connected adapters.
     */
    QList<Adapter*> listAdapters() const;

Q_SIGNALS:
    void adapterAdded(Adapter *adapter);
    void adapterRemoved(Adapter *adapter);
    void defaultAdapterChanged(Adapter *adapter);

private:
    Manager(QObject *parent = 0);

    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_adapterAdded(QDBusObjectPath))
    Q_PRIVATE_SLOT(d, void _k_adapterRemoved(QDBusObjectPath))
    Q_PRIVATE_SLOT(d, void _k_defaultAdapterChanged(QDBusObjectPath))
    Q_PRIVATE_SLOT(d, void _k_propertyChanged(QString,QDBusVariant))
};

}

#endif // BLUEDEVILMANAGER_H
