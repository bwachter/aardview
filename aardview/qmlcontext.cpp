/**
 * @file qmlcontext.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2026
 */

#include <QMimeDatabase>
#include <QMimeType>
#include <QDir>
#include <QDebug>

#include "qmlcontext.h"
#include "tnviewmodel.h"
#include "thumbnailfilesystemmodel.h"
#include "exifviewmodel.h"
#include "settingsdialog.h"
#include "afileinfo.h"

QmlContext::QmlContext(QObject *parent):
  QObject(parent),
  m_uid(QUuid::createUuid()),
  m_videoMode(false) {

  m_tnViewModel = new TnViewModel();
  m_dirViewModel = new ThumbnailFileSystemModel();
  m_dirViewModel->setRootPath(QDir::rootPath());
  m_exifViewModel = new ExifViewModel();
  m_tnViewModelProxy = new QSortFilterProxyModel(this);
  m_tnViewModelProxy->setSourceModel(m_tnViewModel);
  m_dirContentsModel = new QStringListModel(this);

  m_player = new QMediaPlayer(this);
  m_audioOutput = new QAudioOutput(this);
  m_audioOutput->setVolume(1.0f);
  m_player->setAudioOutput(m_audioOutput);

  connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status){
    if (status == QMediaPlayer::EndOfMedia)
      onVideoEnd();
  });

  reconfigure();
}

QmlContext::~QmlContext(){
}

TnViewModel* QmlContext::tnViewModel() const { return m_tnViewModel; }
ThumbnailFileSystemModel* QmlContext::dirViewModel() const { return m_dirViewModel; }
ExifViewModel* QmlContext::exifViewModel() const { return m_exifViewModel; }
QSortFilterProxyModel* QmlContext::tnViewModelProxy() const { return m_tnViewModelProxy; }
QStringListModel* QmlContext::dirContentsModel() const { return m_dirContentsModel; }

QString QmlContext::currentPath() const { return m_path; }

QString QmlContext::displayPath() const {
  if (!m_loadedPath.isEmpty()) return m_loadedPath;
  return ":/images/aardview.png";
}

QString QmlContext::currentTitle() const {
  QString result = "<unknown>";
  if (!m_path.isEmpty()) result = m_path;
  else if (!m_loadedPath.isEmpty()) result = m_loadedPath;
  AFileInfo info(result);
  return info.friendlyFilePath();
}

bool QmlContext::videoMode() const { return m_videoMode; }

int QmlContext::videoEndAction() const {
  return SettingsDialog::instance()->value("viewer/videoEndAction", 0).toInt();
}

void QmlContext::setCurrentPath(const QString &path){
  m_path = path;
  emit currentPathChanged();
}

void QmlContext::loadFile(const QString &path){
  setCurrentPath(path);

  AFileInfo info(path);
  QString dirPath = info.isFile() ? info.absolutePath() : path;
  QDir dir(dirPath);
  SettingsDialog *settings = SettingsDialog::instance();
  QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  if (!settings->value("dirview/showOnlyDirs", true).toBool())
    entries += dir.entryList(QDir::Files);
  QStringList fullPaths;
  for (const QString &entry : entries)
    fullPaths.append(dir.absoluteFilePath(entry));
  m_dirContentsModel->setStringList(fullPaths);

  if (info.isFile()){
    if (isVideoFile(path)){
      m_videoMode = true;
      emit videoModeChanged();
      m_player->stop();
      emit requestLoadVideo(path);
    } else {
      m_videoMode = false;
      emit videoModeChanged();
      m_player->stop();
      m_loadedPath = path;
      emit displayPathChanged();
      emit requestLoadImage(path);
    }
  } else {
    m_videoMode = false;
    emit videoModeChanged();
    m_player->stop();
    m_tnViewModel->setDirectory(path);
    emit requestLoadImage(":/images/aardview.png");
  }
}

void QmlContext::selectNext(){
  int index;
  // Use the proxy model row count
  int rowCount = m_tnViewModelProxy->rowCount();
  if (rowCount == 0) return;

  // Find current index in proxy
  int currentProxyRow = 0; // default to first
  // QML doesn't give us a selection model, so we track via currentPath
  for (int i = 0; i < rowCount; ++i){
    QModelIndex idx = m_tnViewModelProxy->index(i, 0);
    QModelIndex src = m_tnViewModelProxy->mapToSource(idx);
    if (m_tnViewModel->filePath(src) == m_loadedPath){
      currentProxyRow = i;
      break;
    }
  }

  if (currentProxyRow + 1 >= rowCount)
    index = 0;
  else
    index = currentProxyRow + 1;

  QModelIndex idx = m_tnViewModelProxy->index(index, 0);
  QModelIndex src = m_tnViewModelProxy->mapToSource(idx);
  loadFile(m_tnViewModel->filePath(src));
}

void QmlContext::selectPrev(){
  int index;
  int rowCount = m_tnViewModelProxy->rowCount();
  if (rowCount == 0) return;

  int currentProxyRow = 0;
  for (int i = 0; i < rowCount; ++i){
    QModelIndex idx = m_tnViewModelProxy->index(i, 0);
    QModelIndex src = m_tnViewModelProxy->mapToSource(idx);
    if (m_tnViewModel->filePath(src) == m_loadedPath){
      currentProxyRow = i;
      break;
    }
  }

  if (currentProxyRow <= 0)
    index = rowCount - 1;
  else
    index = currentProxyRow - 1;

  QModelIndex idx = m_tnViewModelProxy->index(index, 0);
  QModelIndex src = m_tnViewModelProxy->mapToSource(idx);
  loadFile(m_tnViewModel->filePath(src));
}

bool QmlContext::isVideoFile(const QString &path) const {
  if (path.startsWith(":/")) return false;
  return QMimeDatabase().mimeTypeForFile(path).name().startsWith("video/");
}

QString QmlContext::tnFilePath(int proxyRow) const {
  QModelIndex proxyIdx = m_tnViewModelProxy->index(proxyRow, 0);
  QModelIndex srcIdx = m_tnViewModelProxy->mapToSource(proxyIdx);
  return m_tnViewModel->filePath(srcIdx);
}

bool QmlContext::tnIsDir(int proxyRow) const {
  QModelIndex proxyIdx = m_tnViewModelProxy->index(proxyRow, 0);
  QModelIndex srcIdx = m_tnViewModelProxy->mapToSource(proxyIdx);
  return m_tnViewModel->isDir(srcIdx);
}

QString QmlContext::fileName(const QString &path) const {
  if (path.isEmpty()) return "";
  return QFileInfo(path).fileName();
}

bool QmlContext::isDir(const QString &path) const {
  if (path.startsWith(":/")) return false;
  return QFileInfo(path).isDir();
}

void QmlContext::openSettings(){
  SettingsDialog::instance()->show();
}

void QmlContext::openAbout(){
  // TODO: implement about dialog for QML
}

void QmlContext::reconfigure(){
  SettingsDialog *settings = SettingsDialog::instance();
  m_tnViewModel->reconfigure();
  m_dirViewModel->reconfigure();

  if (settings->value("tnview/showOnlyFiles", true).toBool())
    m_tnViewModel->setFilter(QDir::Files);
}

void QmlContext::onVideoEnd(){
  int action = SettingsDialog::instance()->value("viewer/videoEndAction", 0).toInt();
  switch(action){
    case 0: // Stop
      m_player->stop();
      break;
    case 1: // Loop
      m_player->setPosition(0);
      m_player->play();
      break;
    case 2: // Jump next
      selectNext();
      break;
  }
}
