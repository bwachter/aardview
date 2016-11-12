/**
 * @file windowmodel.h
 * @copyright 2016
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2016
 */

#ifndef _WINDOWMODEL_H
#define _WINDOWMODEL_H

#include <QAbstractListModel>
#include "aardview.h"

class WindowModel: public QAbstractListModel {
    Q_OBJECT

  public:
    WindowModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    void addWindow(const QUuid &uid, AardView *win);
    void deleteWindow(const QUuid &uid);
    bool contains(const QUuid &uid);
    AardView* getWindow(const QUuid &uid);

  private:
    QHash <QUuid, AardView*> m_windowList;
};

#endif
