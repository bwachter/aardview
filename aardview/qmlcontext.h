#ifndef _QMLCONTEXT_H
#define _QMLCONTEXT_H

#include <QObject>
#include <QUrl>
#include <QUuid>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSortFilterProxyModel>
#include <QStringListModel>

#include "tnviewmodel.h"
#include "thumbnailfilesystemmodel.h"
#include "exifviewmodel.h"

class SettingsDialog;

class QmlContext : public QObject {
    Q_OBJECT
    Q_PROPERTY(TnViewModel* tnViewModel READ tnViewModel CONSTANT)
    Q_PROPERTY(ThumbnailFileSystemModel* dirViewModel READ dirViewModel CONSTANT)
    Q_PROPERTY(ExifViewModel* exifViewModel READ exifViewModel CONSTANT)
    Q_PROPERTY(QSortFilterProxyModel* tnViewModelProxy READ tnViewModelProxy CONSTANT)
    Q_PROPERTY(QStringListModel* dirContentsModel READ dirContentsModel CONSTANT)
    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
    Q_PROPERTY(QString currentTitle READ currentTitle NOTIFY currentPathChanged)
    Q_PROPERTY(QString displayPath READ displayPath NOTIFY displayPathChanged)
    Q_PROPERTY(bool videoMode READ videoMode NOTIFY videoModeChanged)
    Q_PROPERTY(int videoEndAction READ videoEndAction CONSTANT)

  public:
    explicit QmlContext(QObject *parent = nullptr);
    ~QmlContext();

    TnViewModel* tnViewModel() const;
    ThumbnailFileSystemModel* dirViewModel() const;
    ExifViewModel* exifViewModel() const;
    QSortFilterProxyModel* tnViewModelProxy() const;

    QString currentPath() const;
    QString currentTitle() const;
    QString displayPath() const;
    bool videoMode() const;
    int videoEndAction() const;

    QStringListModel* dirContentsModel() const;

    Q_INVOKABLE void setCurrentPath(const QString &path);
    Q_INVOKABLE QString fileName(const QString &path) const;
    Q_INVOKABLE bool isDir(const QString &path) const;
    Q_INVOKABLE void loadFile(const QString &path);
    Q_INVOKABLE void selectNext();
    Q_INVOKABLE void selectPrev();
    Q_INVOKABLE bool isVideoFile(const QString &path) const;
    Q_INVOKABLE QString tnFilePath(int proxyRow) const;
    Q_INVOKABLE bool tnIsDir(int proxyRow) const;
    Q_INVOKABLE void openSettings();
    Q_INVOKABLE void openAbout();
    Q_INVOKABLE void reconfigure();

  signals:
    void currentPathChanged();
    void displayPathChanged();
    void videoModeChanged();
    void requestLoadImage(const QString &path);
    void requestLoadVideo(const QString &path);

  private slots:
    void onVideoEnd();

  private:
    QUuid m_uid;
    QString m_path;
    QString m_loadedPath;
    bool m_videoMode;

    TnViewModel *m_tnViewModel;
    ThumbnailFileSystemModel *m_dirViewModel;
    ExifViewModel *m_exifViewModel;
    QSortFilterProxyModel *m_tnViewModelProxy;
    QStringListModel *m_dirContentsModel;
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
};

#endif
