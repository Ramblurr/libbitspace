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
#include "NetworkAccessManager.h"

#include "bitspace_global.h"
//#include "InternetConnectionMonitor.h"
#include <lastfm/ws.h>
#include <lastfm/misc.h>
#include <QCoreApplication>
#include <QNetworkRequest>


static struct NetworkAccessManagerInit
{
    NetworkAccessManagerInit()
    {
    // TODO Win32 and Apple inits
    }
} init;


namespace bitspace
{
    BITSPACESHARED_EXPORT QByteArray UserAgent;
}


bitspace::NetworkAccessManager::NetworkAccessManager( QObject* parent )
               : QNetworkAccessManager( parent )
            #ifdef WIN32
               , m_pac( 0 )
               , m_monitor( 0 )
            #endif
{
    // can't be done in above init, as applicationName() won't be set
    if (bitspace::UserAgent.isEmpty())
    {
        QByteArray name = QCoreApplication::applicationName().toUtf8();
        QByteArray version = QCoreApplication::applicationVersion().toUtf8();
        if (version.size()) version.prepend( ' ' );
        bitspace::UserAgent = name + version /*+ " (" + bitspace::platform() + ")"*/;
    }
}


bitspace::NetworkAccessManager::~NetworkAccessManager()
{
#ifdef WIN32
    delete m_pac;
#endif
}


QNetworkProxy
bitspace::NetworkAccessManager::proxy( const QNetworkRequest& request )
{
    Q_UNUSED( request );

#ifdef WIN32
    IeSettings s;
    if (s.fAutoDetect)
    {
        if (!m_pac) {
            m_pac = new Pac;
            m_monitor = new InternetConnectionMonitor( this );
            connect( m_monitor, SIGNAL(connectivityChanged( bool )), SLOT(onConnectivityChanged( bool )) );
        }
        return m_pac->resolve( request, s.lpszAutoConfigUrl );
    }
#endif

    return QNetworkProxy::applicationProxy();
}


QNetworkReply*
bitspace::NetworkAccessManager::createRequest( Operation op, const QNetworkRequest& request_, QIODevice* outgoingData )
{
    QNetworkRequest request = request_;

    request.setRawHeader( "User-Agent", bitspace::UserAgent );

#ifdef WIN32
    // PAC proxies can vary by domain, so we have to check everytime :(
    QNetworkProxy proxy = this->proxy( request );
    if (proxy.type() != QNetworkProxy::NoProxy)
        QNetworkAccessManager::setProxy( proxy );
#endif

    return QNetworkAccessManager::createRequest( op, request, outgoingData );
}


void
bitspace::NetworkAccessManager::onConnectivityChanged( bool up )
{
    Q_UNUSED( up );

#ifdef WIN32
    if (up && m_pac) m_pac->resetFailedState();
#endif
}
