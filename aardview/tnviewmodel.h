/**
 * @file tnviewmodel.h
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#ifndef _TNVIEWMODEL_H
#define _TNVIEWMODEL_H

#include <QHash>
#include <QtGui>

class ThumbnailProvider;

class TnViewModel: public QAbstractListModel {
    Q_OBJECT

  public:
    TnViewModel(QString directoryName="", QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    void setDirectory(QString directoryName);
    void setFilter(QDir::Filters filters);
    QString filePath(const QModelIndex &index) const;
    bool isDir(const QModelIndex &index) const;

  private slots:
    void onThumbnailReady(const QString &path, const QPixmap &thumbnail);

  private:
    static const QSize thumbnailSize;

    QStringList directoryItems;
    QDir directory;
    QHash<QString, int> m_pathToRow;
    QHash<QString, QPixmap> m_thumbnails;
    ThumbnailProvider *m_provider;

    void rebuildIndex();
};

#endif
