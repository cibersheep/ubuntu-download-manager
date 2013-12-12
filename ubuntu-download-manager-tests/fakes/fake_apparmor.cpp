/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <system/uuid_utils.h>
#include "fake_apparmor.h"

FakeAppArmor::FakeAppArmor(QSharedPointer<UuidFactory> uuidFactory,
                           QObject *parent)
    : AppArmor(parent),
      _isConfined(true),
      _uuidFactory(uuidFactory) {
}

void
FakeAppArmor::getDBusPath(QString& id, QString& dbusPath) {
    id = UuidUtils::getDBusString(_uuidFactory->createUuid());
    dbusPath = id;
    if (_recording) {
        QList<QObject*> inParams;

        QList<QObject*> outParams;
        outParams.append(new StringWrapper(id));
        outParams.append(new StringWrapper(dbusPath));
        MethodParams params(inParams, outParams);
        MethodData methodData("getDBusPath", params);
        _called.append(methodData);
    }
}

QString
FakeAppArmor::getSecurePath(const QString& connName,
                        QString& dbusPath,
                        QString& localPath,
                        bool& isConfined) {
    QString id = UuidUtils::getDBusString(_uuidFactory->createUuid());
    getSecurePath(connName, id, dbusPath, localPath, isConfined);
    return id;
}

void
FakeAppArmor::getSecurePath(const QString& connName,
                       const QString& id,
                       QString& dbusPath,
                       QString& localPath,
                       bool& isConfined) {
    dbusPath = "dbus/" + id;
    localPath = "local/"+ id;
    isConfined = _isConfined;
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new StringWrapper(id));
        inParams.append(new StringWrapper(connName));

        QList<QObject*> outParams;
        outParams.append(new StringWrapper(id));
        outParams.append(new StringWrapper(dbusPath));
        outParams.append(new StringWrapper(localPath));
        MethodParams params(inParams, outParams);
        MethodData methodData("getSecurePath", params);
        _called.append(methodData);
    }
}

bool
FakeAppArmor::isConfined() {
    return _isConfined;
}

void
FakeAppArmor::setIsConfined() {
    _isConfined = true;
}