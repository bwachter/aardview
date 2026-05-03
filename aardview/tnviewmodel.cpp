/**
 * @file tnviewmodel.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#include "localthumbnailprovider.h"
#include "thumbnailprovider.h"
#include "tnviewmodel.h"

const QSize TnViewModel::thumbnailSize(128, 128);

TnViewModel::TnViewModel(QString directoryName, QObject *parent):
  QAbstractListModel(parent) {
  m_provider = new LocalThumbnailProvider(this);
  connect(m_provider, &ThumbnailProvider::thumbnailReady,
          this, &TnViewModel::onThumbnailReady);
  directory = QDir(directoryName);
  directoryItems = directory.entryList();
  rebuildIndex();
}

void TnViewModel::rebuildIndex(){
  m_pathToRow.clear();
  for (int i = 0; i < directoryItems.size(); ++i)
    m_pathToRow[directory.filePath(directoryItems.at(i))] = i;
}

int TnViewModel::rowCount(const QModelIndex &parent) const {
  (void) parent;
  return directoryItems.count();
}

QVariant TnViewModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (index.row() >= directoryItems.size())
    return QVariant();

  if (role == Qt::DisplayRole)
    return directoryItems.at(index.row());
  else if (role == Qt::DecorationRole){
    QString path = directory.filePath(directoryItems.at(index.row()));
    if (m_thumbnails.contains(path))
      return QIcon(m_thumbnails[path]);
    m_provider->requestThumbnail(path, thumbnailSize);
    return QIcon(":/images/aardview-icon.png");
  } else
    return QVariant();
}

QVariant TnViewModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal)
    return QString("Column %1").arg(section);
  else
    return QString("Row %1").arg(section);
}

QString TnViewModel::filePath(const QModelIndex & index) const{
  if (!index.isValid() || index.row() >= directoryItems.size())
    return QString();
  return directory.filePath(directoryItems.at(index.row()));
}

bool TnViewModel::isDir(const QModelIndex & index) const {
  if (!index.isValid() || index.row() >= directoryItems.size())
    return false;
  QFileInfo info=QFileInfo(directoryItems.at(index.row()));
  return info.isDir();
}

void TnViewModel::setDirectory(QString directoryName){
  beginResetModel();
  directory.setPath(directoryName);
  directoryItems = directory.entryList();
  rebuildIndex();
  endResetModel();
}

void TnViewModel::setFilter(QDir::Filters filters){
  beginResetModel();
  directory.setFilter(filters);
  directoryItems = directory.entryList();
  rebuildIndex();
  endResetModel();
}

void TnViewModel::onThumbnailReady(const QString &path, const QPixmap &thumbnail){
  auto it = m_pathToRow.constFind(path);
  if (it == m_pathToRow.constEnd()) return;
  m_thumbnails[path] = thumbnail;
  QModelIndex idx = index(it.value(), 0);
  emit dataChanged(idx, idx, {Qt::DecorationRole});
}
