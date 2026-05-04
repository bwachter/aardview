/**
 * @file qmlimageprovider.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2026
 */

#include <QImageReader>
#include <QUrl>
#include <QDir>
#include <QFile>
#include "qmlimageprovider.h"

QmlImageProvider::QmlImageProvider():
  QQuickImageProvider(QQuickImageProvider::Pixmap),
  m_cache(50) {}

QPixmap QmlImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {
  QString path = QUrl::fromPercentEncoding(id.toUtf8());

  QPixmap *cached = m_cache.object(path);
  if (cached) {
    if (size) *size = cached->size();
    if (requestedSize.isValid())
      return cached->scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return *cached;
  }

  if (QDir(path).exists() || !QFile::exists(path)) {
    QPixmap defaultPixmap(":/images/aardview.png");
    if (size) *size = defaultPixmap.size();
    if (requestedSize.isValid())
      return defaultPixmap.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return defaultPixmap;
  }

  QImageReader reader(path);
  reader.setAutoTransform(true);
  QPixmap pixmap = QPixmap::fromImageReader(&reader);

  if (pixmap.isNull()) {
    QPixmap defaultPixmap(":/images/aardview.png");
    if (size) *size = defaultPixmap.size();
    if (requestedSize.isValid())
      return defaultPixmap.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return defaultPixmap;
  }

  m_cache.insert(path, new QPixmap(pixmap));

  if (size) *size = pixmap.size();
  if (requestedSize.isValid())
    return pixmap.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  return pixmap;
}
