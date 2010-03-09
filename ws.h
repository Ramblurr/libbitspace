/****************************************************************************************
 * Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>                             *
 *  The ws architecture is based on liblastfm                                           *
 *  Copyright 2009 Last.fm Ltd.                                                         *
 *     - Primarily authored by Max Howell, Jono Cole and Doug Mansell                   *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 3 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/

#ifndef BITSPACE_WS_H
#define BITSPACE_WS_H

#include "bitspace_global.h"

#include <QMap>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#define BITSPACE_WS_HOST "http://bitspace.at"

namespace bitspace
{
    BITSPACESHARED_EXPORT void setNetworkAccessManager( QNetworkAccessManager* nam );
    QNetworkAccessManager* nam();

    namespace ws
    {
        BITSPACESHARED_EXPORT extern QString ApiToken;
        BITSPACESHARED_EXPORT extern QString Username;
        BITSPACESHARED_EXPORT extern QString Password;

        BITSPACESHARED_EXPORT QNetworkReply* get( QMap<QString, QString> );
        BITSPACESHARED_EXPORT QNetworkReply* get( const QString &method );
        BITSPACESHARED_EXPORT QNetworkReply* post( QMap<QString, QString>, const QString &method = QString() );

        QVariantMap parse( QNetworkReply* reply );

    }
}

#endif // BITSPACE_WS_H
