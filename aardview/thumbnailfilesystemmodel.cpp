/**
 * @file thumbnailfilesystemmodel.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2026
 */

#include "localthumbnailprovider.h"
#include "settingsdialog.h"
#include "thumbnailfilesystemmodel.h"
#include "thumbnailprovider.h"

ThumbnailFileSystemModel::ThumbnailFileSystemModel(QObject *parent)
  : QFileSystemModel(parent)
  , m_provider(new LocalThumbnailProvider(this)) {
  connect(m_provider, &ThumbnailProvider::thumbnailReady,
          this, &ThumbnailFileSystemModel::onThumbnailReady);
  SettingsDialog *settings = SettingsDialog::instance();
  int sz = settings->value("tnview/thumbnailSize", 128).toInt();
  m_thumbnailSize = QSize(sz, sz);
}

QHash<int, QByteArray> ThumbnailFileSystemModel::roleNames() const {
  QHash<int, QByteArray> roles = QFileSystemModel::roleNames();
  roles[IsDirRole] = "isDir";
  return roles;
}

QVariant ThumbnailFileSystemModel::data(const QModelIndex &index, int role) const {
  if (role == IsDirRole)
    return isDir(index);
  if (role == Qt::DecorationRole && index.isValid() && !isDir(index)){
    QString path = filePath(index);
    if (m_thumbnails.contains(path))
      return QIcon(m_thumbnails.value(path));
    m_provider->requestThumbnail(path, m_thumbnailSize);
    return QFileSystemModel::data(index, role);
  }
  return QFileSystemModel::data(index, role);
}

void ThumbnailFileSystemModel::reconfigure(){
  SettingsDialog *settings = SettingsDialog::instance();
  int sz = settings->value("tnview/thumbnailSize", 128).toInt();
  m_thumbnailSize = QSize(sz, sz);
  m_thumbnails.clear();
}

void ThumbnailFileSystemModel::onThumbnailReady(const QString &path, const QPixmap &thumbnail){
  if (thumbnail.isNull()) return;
  m_thumbnails[path] = thumbnail;
  QModelIndex idx = index(path);
  if (idx.isValid())
    emit dataChanged(idx, idx, {Qt::DecorationRole});
}
