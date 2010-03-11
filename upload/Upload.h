#ifndef UPLOAD_H
#define UPLOAD_H

#include <QObject>
#include <QNetworkReply>
#include <QVariant>
#include <QPointer>
namespace bitspace
{
class Upload : public QObject
{
Q_OBJECT
public:
    explicit Upload( QNetworkReply* reply, const QVariantMap &session, QObject *parent = 0);
    ~Upload();

signals:
    void uploadFinished();
    void uploadError( const QString & );
    void uploadProgress( qint64, qint64 );

public slots:
    void slotAbort();
private slots:
    void slotUploadFinished();
    void slotNotifyFinished();
    void slotError( QNetworkReply::NetworkError error );


private:
    QVariantMap m_session;
    QString m_filename;
    QPointer<QNetworkReply> m_uploadReply;
    QPointer<QNetworkReply> m_notifyReply;

};
}

#endif // UPLOAD_H
