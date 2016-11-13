/**
 * @file tnviewmodel.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#include "tnviewmodel.h"

TnViewModel::TnViewModel(QString directoryName, QObject *parent):
  QAbstractListModel(parent) {
  directory = QDir(directoryName);
  directoryItems = directory.entryList();
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
  endResetModel();
}

void TnViewModel::setFilter(QDir::Filters filters){
  beginResetModel();
  directory.setFilter(filters);
  endResetModel();
}
