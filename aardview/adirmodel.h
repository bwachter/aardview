/**
 * @file adirmodel.h
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2014
 */

#ifndef _ADIRMODEL_H
#define _ADIRMODEL_H

#include <QDirModel>
#include <QDir>
#include <QDebug>

class ADirModel: public QDirModel {
    Q_OBJECT

  public:
    ADirModel(QObject* parent=0): QDirModel(parent){}
    void setFilter(QDir::Filters filters){
      qDebug() << "Setting filter" << filters;
      beginResetModel();
      QDirModel::setFilter(filters);
      endResetModel();
    };

};

#endif
