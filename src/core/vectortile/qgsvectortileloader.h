/***************************************************************************
  qgsvectortileloader.h
  --------------------------------------
  Date                 : March 2020
  Copyright            : (C) 2020 by Martin Dobias
  Email                : wonder dot sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSVECTORTILELOADER_H
#define QGSVECTORTILELOADER_H

#define SIP_NO_FILE

class QByteArray;

#include "qgsvectortilerenderer.h"

/**
 * \ingroup core
 * Keeps track of raw tile data that need to be decoded
 *
 * \since QGIS 3.14
 */
class QgsVectorTileRawData
{
  public:
    //! Constructs a raw tile object
    QgsVectorTileRawData( QgsTileXYZ tileID = QgsTileXYZ(), const QByteArray &raw = QByteArray() )
      : id( tileID ), data( raw ) {}

    //! Tile position in tile matrix set
    QgsTileXYZ id;
    //! Raw tile data
    QByteArray data;
};


class QNetworkReply;
class QEventLoop;

class QgsMBTilesReader;

/**
 * \ingroup core
 * The loader class takes care of loading raw vector tile data from a tile source.
 *
 * \since QGIS 3.14
 */
class QgsVectorTileLoader : public QObject
{
    Q_OBJECT
  public:

    //! Returns raw tile data for the specified range of tiles. Blocks the caller until all tiles are fetched.
    static QList<QgsVectorTileRawData> blockingFetchTileRawData( const QString &sourceType, const QString &sourcePath, int zoomLevel, const QPointF &viewCenter, const QgsTileRange &range );

    //! Returns raw tile data for a single tile, doing a HTTP request. Block the caller until tile data are downloaded.
    static QByteArray loadFromNetwork( const QgsTileXYZ &id, const QString &requestUrl );
    //! Returns raw tile data for a single tile loaded from MBTiles file
    static QByteArray loadFromMBTiles( const QgsTileXYZ &id, QgsMBTilesReader &mbTileReader );
    //! Decodes gzip byte stream, returns true on success
    static bool decodeGzip( const QByteArray &bytesIn, QByteArray &bytesOut );

    //
    // non-static stuff
    //

    //! Constructs tile loader for doing asynchronous requests and starts network requests
    QgsVectorTileLoader( const QString &uri, int zoomLevel, const QgsTileRange &range, const QPointF &viewCenter, QgsFeedback *feedback );
    ~QgsVectorTileLoader();

    //! Blocks the caller until all asynchronous requests are finished (with a success or a failure)
    void downloadBlocking();

  private:
    void loadFromNetworkAsync( const QgsTileXYZ &id, const QString &requestUrl );

  private slots:
    void tileReplyFinished();
    void canceled();

  signals:
    //! Emitted when a tile request has finished. If a tile request has failed, the returned raw tile byte array is empty.
    void tileRequestFinished( const QgsVectorTileRawData &rawTile );

  private:
    //! Event loop used for blocking download
    std::unique_ptr<QEventLoop> mEventLoop;
    //! Feedback object that allows cancellation of pending requests
    QgsFeedback *mFeedback;
    //! Running tile requests
    QList<QNetworkReply *> mReplies;

};

#endif // QGSVECTORTILELOADER_H
