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

void WindowModel::deleteWindow(const QUuid &uid){
  AardView *win = m_windowList[uid];
  win->hide();
  m_windowList.remove(uid);
  win->deleteLater();
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

  // iter.key contains the uuid, which we probably don't need to expose
  AardView *win = iter.value();
  QFont font;

  switch(role){
    case Qt::DisplayRole:
      return win->title();
    case Qt::FontRole:
      if (win->isHidden())
        font.setItalic(true);
      return font;
    case Qt::UserRole:
      return QVariant::fromValue(iter.value());
    case Qt::CheckStateRole:
      if (win->isHidden())
        return Qt::Unchecked;
      else
        return Qt::Checked;
  }

  return QVariant();
}
