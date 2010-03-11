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

#ifndef BITSPACE_UPLOAD_H
#define BITSPACE_UPLOAD_H

#include "bitspace_global.h"
#include "Upload.h"

#include <QNetworkReply>
#include <QObject>
#include <QVariant>

namespace bitspace
{

    class BITSPACESHARED_EXPORT UploadManager : public QObject
    {
        Q_OBJECT
    public:
           UploadManager( QObject* parent = 0 );
           void startNewSession();

           /**
             * Creates an upload job for the file
             * and returns an Upload object that
             * can be used to get progress updates or abort the job.
             */
           Upload* upload( const QString & file_path  );

           /**
             * returns true if the UploadManager has a valid session
             * and is ready to take uploads.
             */
           bool isValid() const;

    private slots:
           void slotNewSession();

    private:
           QNetworkReply* m_sessionReply;

           QVariantMap m_session;
           QString m_filename;

    };

}
#endif // BITSPACE_UPLOAD_H
