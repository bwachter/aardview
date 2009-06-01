#include "tnviewmodel.h"

TnViewModel::TnViewModel(QString directoryName, QObject *parent): 
  QAbstractListModel(parent) {
  directory = QDir(directoryName);
  directoryItems = directory.entryList();
}

int TnViewModel::rowCount(const QModelIndex &parent) const {
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

void TnViewModel::setDirectory(QString directoryName){
  directory.setPath(directoryName);
  directoryItems = directory.entryList();
  reset();
}
