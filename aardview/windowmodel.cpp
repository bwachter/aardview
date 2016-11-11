/**
 * @file windowmodel.cpp
 * @copyright 2016
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2016
 */

#include "windowmodel.h"

WindowModel::WindowModel(QObject *parent):
  QAbstractListModel(parent) {
}

void WindowModel::addWindow(const QUuid &uid, AardView *win){
  m_windowList[uid] = win;

  QModelIndex topLeft = createIndex(0, 0);
  QModelIndex bottomRight = createIndex(0, m_windowList.size());

  emit dataChanged(topLeft, bottomRight);
}

bool WindowModel::contains(const QUuid &uid){
  return m_windowList.contains(uid);
}

AardView* WindowModel::getWindow(const QUuid &uid){
  return m_windowList[uid];
}

int WindowModel::rowCount(const QModelIndex &parent) const {
  (void) parent;
  return m_windowList.size();
}

QVariant WindowModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (index.row() >= m_windowList.size())
    return QVariant();

  QHash<QUuid, AardView*>::const_iterator iter
    = m_windowList.constBegin() + index.row();


  if (role == Qt::DisplayRole)
    return iter.key();
  else if (role == Qt::UserRole)
    return QVariant::fromValue(iter.value());
  else
    return QVariant();
}
