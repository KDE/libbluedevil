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

#ifndef BLUEDEVIL_H
#define BLUEDEVIL_H

#include <bluedevil_export.h>
#include <bluedeviladapter.h>

#include <QtCore/QObject>
#include <QtDBus/QDBusObjectPath>

class BLUEDEVIL_EXPORT BlueDevil
    : public QObject
{
    Q_OBJECT

public:
    virtual ~BlueDevil();

    static BlueDevil *self();

    BlueDevilAdapter defaultAdapter() const;
    QList<BlueDevilAdapter> listAdapters() const;

Q_SIGNALS:
    void adapterAdded(const BlueDevilAdapter &adapter);
    void adapterRemoved(const BlueDevilAdapter &adapter);
    void defaultAdapterChanged(const BlueDevilAdapter &adapter);
    void propertyChanged(const QString &property, const QDBusVariant &newValue);

private:
    BlueDevil(QObject *parent = 0);

    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_adapterAdded(QDBusObjectPath))
    Q_PRIVATE_SLOT(d, void _k_adapterRemoved(QDBusObjectPath))
    Q_PRIVATE_SLOT(d, void _k_defaultAdapterChanged(QDBusObjectPath))
    Q_PRIVATE_SLOT(d, void _k_propertyChanged(QString,QDBusVariant))
};

#endif // BLUEDEVIL_H
