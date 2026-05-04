/**
 * @file localthumbnailprovider.h
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2026
 */

#ifndef _LOCALTHUMBNAILPROVIDER_H
#define _LOCALTHUMBNAILPROVIDER_H

#include <QHash>
#include <QSet>
#include "thumbnailprovider.h"

class VideoThumbnailExtractor;
class QThread;

class LocalThumbnailProvider: public ThumbnailProvider {
    Q_OBJECT

  public:
    explicit LocalThumbnailProvider(QObject *parent = nullptr);
    ~LocalThumbnailProvider();
    void requestThumbnail(const QString &path, const QSize &size) override;

  signals:
    void videoThumbnailRequested(const QString &key, const QString &path, const QSize &size);

  private:
    QHash<QString, QPixmap> m_cache;
    QSet<QString> m_pending;
    VideoThumbnailExtractor *m_videoExtractor;
    QThread *m_videoThread;
};

#endif
