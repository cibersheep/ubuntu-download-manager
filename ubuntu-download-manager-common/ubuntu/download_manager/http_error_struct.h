/*
 * Copyright 2014 Canonical Ltd.
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

#ifndef DOWNLOADER_LIB_HTTP_ERROR_STRUCT_H
#define DOWNLOADER_LIB_HTTP_ERROR_STRUCT_H

#include <QString>
#include "common.h"

class QDBusArgument;

namespace Ubuntu {

namespace DownloadManager {

/*!
    \class HttpErrorStruct 
    \brief The HttpErrorStruct represents the dbus structure that is used
           to communicate http errors that happened in the download
           manager to the different clients.
    \since 0.3
*/
class DOWNLOAD_MANAGER_EXPORT HttpErrorStruct {
    Q_PROPERTY(int code READ getCode)
    Q_PROPERTY(QString phrase READ getPhrase)

 public:

    /*!
        Creates a new structure with the default values.
    */
    HttpErrorStruct();

    /*!
        Creates a new structure with the given error types and message.
    */
    HttpErrorStruct(int code, QString phrase);

    /*!
        Copy constructor.
    */
    HttpErrorStruct(const HttpErrorStruct& other);

    /*!
        Assign operator.
    */
    HttpErrorStruct& operator=(const HttpErrorStruct& other);


    /*!
        \internal
    */
    friend QDBusArgument &operator<<(QDBusArgument &argument,
        const HttpErrorStruct& error);

    /*!
        \internal
    */
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
        HttpErrorStruct& error);

    /*!
        \fn int getCode() const

        Returns the http error code.
    */
    int getCode() const;

    /*!
        \fn QString getPhrase() const

        Returns a human readable message about the http error.
    */
    QString getPhrase() const;

 private:

    /*!
        \internal
    */
    int _code;

    /*!
        \internal
    */
    QString _phrase;
};

}  // DownloadManager

}  // Ubuntu

#endif // HTTP_ERROR_STRUCT_H
