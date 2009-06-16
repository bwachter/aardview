/* tnviewmodel.h	-- Bernd Wachter <bwachter@lart.info>
 *
 * $Header$
 * $Log$
 */

#ifndef _TNVIEWMODEL_H
#define _TNVIEWMODEL_H

#include <QtGui>

class TnViewModel: public QAbstractListModel {
  Q_OBJECT

  public:
  TnViewModel(QString directoryName, QObject *parent = 0);
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;
  void setDirectory(QString directoryName);
  void setFilter (QDir::Filters filters);
  QString filePath(const QModelIndex & index) const;
  bool isDir(const QModelIndex & index) const;

  private:
    QStringList directoryItems;
    QDir directory;
  
};


#endif
