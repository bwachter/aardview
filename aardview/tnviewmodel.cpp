/**
 * @file tnviewmodel.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#include "localthumbnailprovider.h"
#include "settingsdialog.h"
#include "thumbnailprovider.h"
#include "tnviewmodel.h"

TnViewModel::TnViewModel(QString directoryName, QObject *parent):
  QAbstractListModel(parent) {
  m_provider = new LocalThumbnailProvider(this);
  connect(m_provider, &ThumbnailProvider::thumbnailReady,
          this, &TnViewModel::onThumbnailReady);
  SettingsDialog *settings = SettingsDialog::instance();
  int sz = settings->value("tnview/thumbnailSize", 128).toInt();
  m_thumbnailSize = QSize(sz, sz);
  directory = QDir(directoryName);
  directoryItems = directory.entryList();
  applyFileMask();
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

QHash<int, QByteArray> TnViewModel::roleNames() const {
  QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
  roles[FilePathRole] = "filePath";
  roles[Qt::DisplayRole] = "fileName";
  roles[Qt::DecorationRole] = "decoration";
  return roles;
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
      return QIcon(m_thumbnails.value(path));
    m_provider->requestThumbnail(path, m_thumbnailSize);
    return QIcon(":/images/aardview-icon.png");
  } else if (role == FilePathRole)
    return directory.filePath(directoryItems.at(index.row()));
  else
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

QString TnViewModel::filePath(int row) const{
  if (row < 0 || row >= directoryItems.size())
    return QString();
  return directory.filePath(directoryItems.at(row));
}

bool TnViewModel::isDir(const QModelIndex & index) const {
  if (!index.isValid() || index.row() >= directoryItems.size())
    return false;
  QFileInfo info=QFileInfo(directoryItems.at(index.row()));
  return info.isDir();
}

bool TnViewModel::isDir(int row) const {
  if (row < 0 || row >= directoryItems.size())
    return false;
  QFileInfo info=QFileInfo(directoryItems.at(row));
  return info.isDir();
}

void TnViewModel::setDirectory(QString directoryName){
  beginResetModel();
  directory.setPath(directoryName);
  directoryItems = directory.entryList();
  applyFileMask();
  rebuildIndex();
  endResetModel();
}

void TnViewModel::setFilter(QDir::Filters filters){
  beginResetModel();
  directory.setFilter(filters);
  directoryItems = directory.entryList();
  applyFileMask();
  rebuildIndex();
  endResetModel();
}

void TnViewModel::reconfigure(){
  SettingsDialog *settings = SettingsDialog::instance();
  int sz = settings->value("tnview/thumbnailSize", 128).toInt();
  m_thumbnailSize = QSize(sz, sz);
  m_thumbnails.clear();

  beginResetModel();
  directoryItems = directory.entryList();
  applyFileMask();
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

void TnViewModel::applyFileMask(){
  SettingsDialog *settings = SettingsDialog::instance();
  m_filterFiles = settings->value("tnview/filterFiles").toBool();
  m_caseInsensitive = settings->value("tnview/caseInsensitiveMatching").toBool();
  m_fileMask = settings->value("tnview/fileMask").toString();

  if (!m_filterFiles || m_fileMask.isEmpty())
    return;

  QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
  if (m_caseInsensitive)
    options |= QRegularExpression::CaseInsensitiveOption;

  m_fileRegex = QRegularExpression(m_fileMask, options);
  if (!m_fileRegex.isValid()){
    qWarning() << "Invalid file mask regex:" << m_fileMask;
    m_filterFiles = false;
    return;
  }

  QStringList filtered;
  for (const QString &item : directoryItems){
    if (m_fileRegex.match(item).hasMatch())
      filtered.append(item);
  }
  directoryItems = filtered;
}
