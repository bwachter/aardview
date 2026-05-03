/**
 * @file thumbnailprovider.h
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2026
 */

#ifndef _THUMBNAILPROVIDER_H
#define _THUMBNAILPROVIDER_H

#include <QObject>
#include <QPixmap>
#include <QSize>

class ThumbnailProvider: public QObject {
    Q_OBJECT

  public:
    explicit ThumbnailProvider(QObject *parent = nullptr): QObject(parent) {}
    virtual ~ThumbnailProvider() = default;

    virtual void requestThumbnail(const QString &path, const QSize &size) = 0;

  signals:
    void thumbnailReady(const QString &path, const QPixmap &thumbnail);
};

#endif
