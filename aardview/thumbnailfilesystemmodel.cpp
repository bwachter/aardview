/**
 * @file thumbnailfilesystemmodel.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2026
 */

#include "localthumbnailprovider.h"
#include "thumbnailfilesystemmodel.h"
#include "thumbnailprovider.h"

const QSize ThumbnailFileSystemModel::thumbnailSize(128, 128);

ThumbnailFileSystemModel::ThumbnailFileSystemModel(QObject *parent)
  : QFileSystemModel(parent)
  , m_provider(new LocalThumbnailProvider(this)) {
  connect(m_provider, &ThumbnailProvider::thumbnailReady,
          this, &ThumbnailFileSystemModel::onThumbnailReady);
}

QVariant ThumbnailFileSystemModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::DecorationRole && index.isValid() && !isDir(index)){
    QString path = filePath(index);
    if (m_thumbnails.contains(path))
      return QIcon(m_thumbnails.value(path));
    m_provider->requestThumbnail(path, thumbnailSize);
    return QFileSystemModel::data(index, role);
  }
  return QFileSystemModel::data(index, role);
}

void ThumbnailFileSystemModel::onThumbnailReady(const QString &path, const QPixmap &thumbnail){
  if (thumbnail.isNull()) return;
  m_thumbnails[path] = thumbnail;
  QModelIndex idx = index(path);
  if (idx.isValid())
    emit dataChanged(idx, idx, {Qt::DecorationRole});
}
