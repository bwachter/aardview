/**
 * @file thumbnailfilesystemmodel.h
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2026
 */

#ifndef _THUMBNAILFILESYSTEMMODEL_H
#define _THUMBNAILFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include <QHash>
#include <QPixmap>
#include <QSize>

class ThumbnailProvider;

class ThumbnailFileSystemModel: public QFileSystemModel {
    Q_OBJECT

  public:
    enum CustomRoles {
      IsDirRole = Qt::UserRole + 1
    };
    Q_ENUM(CustomRoles)

    explicit ThumbnailFileSystemModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

  public slots:
    void reconfigure();

  private slots:
    void onThumbnailReady(const QString &path, const QPixmap &thumbnail);

  private:
    QSize m_thumbnailSize;
    ThumbnailProvider *m_provider;
    QHash<QString, QPixmap> m_thumbnails;
};

#endif
