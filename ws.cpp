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

#include "ws.h"

#include "NetworkAccessManager.h"

#include <qjson/parser.h>

#include <QByteArray>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QCoreApplication>
#include <QVariant>

static QNetworkAccessManager* nam = 0;

void sign( QMap<QString, QString> &params )
{
    params["user_credentials"] = bitspace::ws::ApiToken;

}

QString url(  const QString &method = QString() )
{
    if( method.isEmpty() )
        return BITSPACE_WS_HOST;
    QString host(BITSPACE_WS_HOST);
    return host + "/" + method;
}

QNetworkAccessManager*
bitspace::nam()
{
//    if (!::nam) ::nam = new NetworkAccessManager( qApp );
    if (!::nam) ::nam = new QNetworkAccessManager( qApp );
    return ::nam;
}

QByteArray authorizationHeader()
{
    qDebug() << "using basic http authentication";
    // HTTP Basic authentication header value: base64(username:password)
    QString concatenated = bitspace::ws::Username + ":" + bitspace::ws::Password;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    return headerData.toLocal8Bit();
}

void
bitspace::setNetworkAccessManager( QNetworkAccessManager* nam )
{
    delete ::nam;
    ::nam = nam;
    nam->setParent( qApp ); // ensure it isn't deleted out from under us
}

QNetworkReply*
bitspace::ws::get( QMap<QString, QString> params )
{
    QUrl url = ::url();
    QMapIterator<QString, QString> i( params );
    while (i.hasNext()) {
        i.next();
        QByteArray const key = QUrl::toPercentEncoding( i.key() );
        QByteArray const value = QUrl::toPercentEncoding( i.value() );
        url.addEncodedQueryItem( key, value );
    }

    qDebug() << url;

    return nam()->get( QNetworkRequest(url) );
}

QNetworkReply*
bitspace::ws::get( const QString &method )
{
    QUrl url = ::url( method );
    QNetworkRequest request;
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("User-Agent", "curl/7.20.0 (x86_64-unknown-linux-gnu) libcurl/7.20.0 OpenSSL/0.9.8l zlib/1.2.3.7");
    if( bitspace::ws::ApiToken.isEmpty() )
        request.setRawHeader("Authorization", authorizationHeader() );
    else
    {
        url.addEncodedQueryItem( "user_credentials", QUrl::toPercentEncoding(bitspace::ws::ApiToken) );
    }
    request.setUrl( url );
    qDebug() << url;
    return nam()->get( request );
}

QNetworkReply*
bitspace::ws::post( QMap<QString, QString> params, const QString &method )
{
    QUrl url = ::url( method );
    QByteArray query;
    QMapIterator<QString, QString> i( params );
    while (i.hasNext()) {
        i.next();
        query += QUrl::toPercentEncoding( i.key() )
               + '='
               + QUrl::toPercentEncoding( i.value() )
               + '&';
    }
    QNetworkRequest request;
    if( bitspace::ws::ApiToken.isEmpty() )
        request.setRawHeader("Authorization", authorizationHeader() );
    else
    {
        url.addEncodedQueryItem( "user_credentials", QUrl::toPercentEncoding(bitspace::ws::ApiToken) );
    }
    request.setUrl( url );
    qDebug() << url;
    return nam()->post( request, query );
}

QVariantMap bitspace::ws::parse( QNetworkReply* reply )
{
    //TODO error handling
    QByteArray data = reply->readAll();
    qDebug() << "data:" << data;
    reply->deleteLater();
    if( !data.size() )
    {
        // TODO bad response
    }

    QString json( data );
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(data, &ok).toMap();
    if( ok )
        return result;
    else {
        // TODO error
    }
    return QVariantMap();
}

namespace bitspace
{
    namespace ws
    {
        QString ApiToken;
        QString Username;
        QString Password;
    }
}

