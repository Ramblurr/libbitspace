/****************************************************************************************
 * Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>                             *
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

#include "Upload.h"

#include "../ws.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
namespace bitspace
{
    Upload::Upload( QObject* parent ) : QObject( parent )
    {

    }

    void Upload::startNewSession()
    {
        m_sessionReply =  ws::get("uploads/new"); // get new upload session
        connect( m_sessionReply, SIGNAL(finished()), SLOT(slotNewSession()) );
    }

    bool Upload::upload( const QString & file_path )
    {
        if( m_session.size() == 0 )
        {
            qWarning() << "No Upload Session";
            return false;
        }

        QFile file( file_path );
        if( !file.exists() )
        {
            qWarning() << "No such file: " << file_path;
            return false;
        }
        bool open = file.open( QIODevice::ReadOnly );
        if(!open)
        {
            qDebug() << "opening " << file_path << "failed";
            return false;
        }

        QByteArray boundary = "----------------------------8e61d618ca16";
        QByteArray nl = "\r\n";
        QByteArray sep = "--" + boundary + nl;

        QNetworkRequest header;
        header.setUrl( m_session["url"].toString() );
        header.setRawHeader("User-Agent", "curl/7.20.0 (x86_64-unknown-linux-gnu) libcurl/7.20.");
        header.setRawHeader("Accept", "*/*");
        header.setHeader(QNetworkRequest::ContentTypeHeader, QString("multipart/form-data; boundary=" + boundary) );

        QByteArray ba;
        ba.append(sep);
        QMapIterator<QString, QVariant> i( m_session["params"].toMap() );
        while ( i.hasNext() )
        {
            i.next();
            ba.append( QString("Content-Disposition: form-data; name=\"%1\"").arg( i.key() ).toUtf8() );
            ba.append(nl);
            ba.append(nl);
            ba.append( i.value().toString().toUtf8());
            ba.append(nl);
            ba.append(sep);
        }

        m_filename = QFileInfo(file_path).fileName();
        QString file_param = m_session["file_param"].toString();
            ba.append( QString("Content-Disposition: form-data; name=\"%1\"; filename=\"%2\"").arg( file_param, m_filename ).toUtf8() );
        ba.append(nl);
        ba.append("Content-Type: application/octet-stream");
        ba.append(nl);
        ba.append(nl);
        ba.append( file.readAll() );
        ba.append(nl);
        ba.append("--" + boundary + "--" + nl);
        header.setHeader(QNetworkRequest::ContentLengthHeader, ba.size() );
        qDebug() << "Queryomg:\n" << ba;
        foreach(QString key, header.rawHeaderList())
        {
            qDebug() << key << ": " << header.rawHeader(key.toUtf8());
        }

        m_uploadReply = bitspace::nam()->post( header, ba);
        connect(m_uploadReply, SIGNAL(uploadProgress(qint64,qint64)), this, SIGNAL(uploadProgress(qint64,qint64)));
        connect(m_uploadReply, SIGNAL(finished()), this, SLOT(slotUploadFinished()));
        connect(m_uploadReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotUploadFinished()));
        return true;
    }

    void Upload::slotNewSession()
    {
        qDebug() << "Upload::slotNewSession()";
        if( !m_sessionReply )
            return;
        m_session = ws::parse( m_sessionReply );
        qDebug() << "Got upload session";
        qDebug() << "upload url:" << m_session["url"].toString();
        qDebug() << "file_param:" << m_session["file_param"].toString();
        qDebug() << "params: {";
        QVariantMap params = m_session["params"].toMap();
        QMapIterator<QString, QVariant> i( params );
        while ( i.hasNext() )
        {
            i.next();
            qDebug() << "\t" << i.key() << ": " << i.value().toString();
        }
        qDebug() << "}";
    }

    void Upload::slotUploadFinished()
    {
        qDebug() << "Upload::slotUploadFinished()";
        QString reply = m_uploadReply->readAll();
        qDebug() << reply;
        QString key = m_session["params"].toMap()["key"].toString();
        key = key.replace("${filename}", m_filename);

        QMap<QString, QString> params;
        params["upload[key]"] = key;
        m_notifyReply =  ws::post(params, "uploads"); // get new upload session
        connect( m_notifyReply, SIGNAL(finished()), SLOT(slotNotifyFinished()) );
    }
    void Upload::slotNotifyFinished()
    {
        QString reply = m_notifyReply->readAll();
        qDebug() << reply;
        emit uploadFinished();
    }

    void Upload::slotError( QNetworkReply::NetworkError error )
    {
        qDebug() << "Error: " << error << m_uploadReply->errorString();
        emit uploadError( m_uploadReply->errorString() );
    }

    void Upload::slotAbort()
    {
        if( m_uploadReply && m_uploadReply->isRunning() )
            m_uploadReply->abort();
        else if( m_notifyReply && m_notifyReply->isRunning() )
            m_notifyReply->abort();
    }

}
