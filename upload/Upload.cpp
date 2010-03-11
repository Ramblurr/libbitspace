#include "Upload.h"

#include "../ws.h"

#include <QDebug>

namespace bitspace
{
Upload::Upload( QNetworkReply* reply, const QVariantMap &session, QObject *parent) :
    QObject(parent),
    m_session( session ),
    m_uploadReply( reply )


{
    connect( m_uploadReply, SIGNAL(finished()), SLOT(slotUploadFinished()));
    connect( m_uploadReply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(slotError( QNetworkReply::NetworkError)));
    connect( m_uploadReply, SIGNAL(uploadProgress(qint64,qint64)), SIGNAL(uploadProgress( qint64, qint64 )));
}

Upload::~Upload()
{
    if( m_notifyReply )
        m_notifyReply->deleteLater();;
    if( m_uploadReply )
        m_uploadReply->deleteLater();;
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
