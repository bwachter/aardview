/**
 * @file exifviewmodel.h
 * @copyright 2021 GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2021
 */

#ifndef _EXIFVIEWMODEL_H
#define _EXIFVIEWMODEL_H

#include <QAbstractTableModel>
#include <QMap>

class ExifViewModel: public QAbstractTableModel {
    Q_OBJECT

  public:
    ExifViewModel(QMap<QString, QString>* mapPtr, QObject *parent = 0);
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    //void setFilter (QDir::Filters filters);

  public slots:
    void setMap(const QMap<QString, QString>& map);

  private:
    QMap<QString, QString> _data;
};

#endif
