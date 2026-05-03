/**
 * @file filecollectionmodel.h
 * @copyright 2018 GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2018
 */

#ifndef _FILECOLLECTIONMODEL_H
#define _FILECOLLECTIONMODEL_H

#include <QAbstractListModel>
#include "aardview.h"

// define an item class with filename, thumbnail, and item flags
// adapt this class to be a list of that
// change thumbnailview to use that, fed through a directory listing
// also use this to collect files to be copied, deleted, ...
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
    AardView* getWindow(const QString &title);

  private:
    QHash <QUuid, AardView*> m_windowList;
};

#endif
