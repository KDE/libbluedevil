/*************************************************************************************
 *  Copyright (C) 2013 by Alejandro Fiestas Fiestas <afiestas@kde.org>               *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#ifndef VISHESH_PAYS_DINNER
#define VISHESH_PAYS_DINNER

#include "dbusobjectmanager.h"
#include "bluezagentmanager1.h"
#include "bluedevildbustypes.h"

#include <QObject>
#include <QDBusObjectPath>

namespace BlueDevil {
class Adapter;
class Manager;
class Device;

class ManagerPrivate : public QObject
{
    Q_OBJECT
public:
    ManagerPrivate(Manager *q);
    virtual ~ManagerPrivate();

    void initialize();
    void clean();
    Adapter *findUsableAdapter();
    Device  *deviceForUBI(const QString &UBI);


    org::freedesktop::DBus::ObjectManager *m_dbusObjectManager;
    org::bluez::AgentManager1             *m_bluezAgentManager;
    Adapter                               *m_usableAdapter;
    QMap<QString, Adapter*>                m_adapters;
    QHash<QString, Adapter*>               m_devAdapter;
    bool                                   m_bluezServiceRunning;

    Manager *const m_q;

public Q_SLOTS:
    void _k_bluezServiceRegistered();
    void _k_bluezServiceUnregistered();
    void _k_bluezAdapterPoweredChanged(bool powered);

    void _k_interfacesAdded(const QDBusObjectPath &objectPath, const QVariantMapMap &interfaces);
    void _k_interfacesRemoved(const QDBusObjectPath &objectPath, const QStringList &interfaces);
};

}

#endif //VISHESH_PAYS_DINNER