/*
 * Copyright 2013-2015 Canonical Ltd.
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

#pragma once

#include <QMap>
#include <QList>
#include <ubuntu/transfers/errors/auth_error_struct.h>
#include <ubuntu/transfers/errors/hash_error_struct.h>
#include <ubuntu/transfers/errors/http_error_struct.h>
#include <ubuntu/transfers/errors/network_error_struct.h>
#include <ubuntu/transfers/errors/process_error_struct.h>
#include "download_state_struct.h"
#include "download_struct.h"
#include "group_download_struct.h"

using namespace Ubuntu::DownloadManager;
using namespace Ubuntu::Transfers::Errors;

typedef QMap<QString, QString> StringMap;
typedef QList<GroupDownloadStruct> StructList;

Q_DECLARE_METATYPE(AuthErrorStruct)
Q_DECLARE_METATYPE(HashErrorStruct)
Q_DECLARE_METATYPE(HttpErrorStruct)
Q_DECLARE_METATYPE(NetworkErrorStruct)
Q_DECLARE_METATYPE(ProcessErrorStruct)
Q_DECLARE_METATYPE(DownloadStruct)
Q_DECLARE_METATYPE(DownloadStateStruct)
Q_DECLARE_METATYPE(StringMap)
Q_DECLARE_METATYPE(StructList)

