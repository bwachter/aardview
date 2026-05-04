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
#include <QRegularExpression>

class ThumbnailProvider;

class TnViewModel: public QAbstractListModel {
    Q_OBJECT

  public:
    enum TnRoles {
      FilePathRole = Qt::UserRole + 1
    };
    Q_ENUM(TnRoles)

    TnViewModel(QString directoryName="", QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    void setDirectory(QString directoryName);
    void setFilter(QDir::Filters filters);
    Q_INVOKABLE QString filePath(const QModelIndex &index) const;
    Q_INVOKABLE QString filePath(int row) const;
    Q_INVOKABLE bool isDir(const QModelIndex &index) const;
    Q_INVOKABLE bool isDir(int row) const;
    QHash<int, QByteArray> roleNames() const override;

  public slots:
    void reconfigure();

  private slots:
    void onThumbnailReady(const QString &path, const QPixmap &thumbnail);

  private:
    QSize m_thumbnailSize;

    QStringList directoryItems;
    QDir directory;
    QHash<QString, int> m_pathToRow;
    QHash<QString, QPixmap> m_thumbnails;
    ThumbnailProvider *m_provider;

    bool m_filterFiles;
    bool m_caseInsensitive;
    QString m_fileMask;
    QRegularExpression m_fileRegex;

    void rebuildIndex();
    void applyFileMask();
};

#endif
