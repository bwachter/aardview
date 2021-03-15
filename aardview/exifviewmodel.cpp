/**
 * @file exifviewmodel.cpp
 * @copyright 2021 GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2021
 */

#include <QIcon>
#include "exifviewmodel.h"

ExifViewModel::ExifViewModel(QObject *parent):
  QAbstractTableModel(parent) {
}

ExifViewModel::ExifViewModel(QMap<QString, QString> map, QObject *parent):
  QAbstractTableModel(parent) {
  _data=map;
}

int ExifViewModel::columnCount(const QModelIndex &parent) const {
  (void) parent;
  return 2;
}

int ExifViewModel::rowCount(const QModelIndex &parent) const {
  (void) parent;
  return _data.count();
}

QVariant ExifViewModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (index.row() >= _data.count())
    return QVariant();

  if (role == Qt::DisplayRole||
      role == Qt::ToolTipRole){
    if (index.column() == 0)
      return _data.keys().at(index.row());
    if (index.column() == 1)
      return _data.values().at(index.row());
  }

  return QVariant();
}

QVariant ExifViewModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal){
    if (section==0)
      return "Key";
    else if (section==1)
      return "Value";
    else
      return "Uh?!";
  } else
    return QString("%1").arg(section+1);
}

void ExifViewModel::setMap(const QMap<QString, QString>& map){
  beginResetModel();
  _data=map;
  endResetModel();
}

/*
void ExifViewModel::setFilter(QDir::Filters filters){
  beginResetModel();
//  directory.setFilter(filters);
  endResetModel();
}
*/
