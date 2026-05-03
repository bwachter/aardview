/**
 * @file localthumbnailprovider.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2026
 */

#include <QFutureWatcher>
#include <QImageReader>
#include <QMediaPlayer>
#include <QMimeDatabase>
#include <QQueue>
#include <QVideoFrame>
#include <QVideoSink>
#include <QtConcurrent>

#include "localthumbnailprovider.h"

static bool isVideoPath(const QString &path){
  if (path.startsWith(":/")) return false;
  return QMimeDatabase().mimeTypeForFile(path).name().startsWith("video/");
}

// Processes video thumbnail requests one at a time on the main thread.
// Plays each file just long enough to receive the first valid decoded frame,
// then stops. Errors skip to the next queued request.
class VideoThumbnailExtractor: public QObject {
    Q_OBJECT

  public:
    explicit VideoThumbnailExtractor(QObject *parent = nullptr)
      : QObject(parent)
      , m_player(new QMediaPlayer(this))
      , m_sink(new QVideoSink(this)) {
      m_player->setVideoSink(m_sink);
      connect(m_sink, &QVideoSink::videoFrameChanged,
              this, &VideoThumbnailExtractor::onVideoFrameChanged);
      connect(m_player, &QMediaPlayer::errorOccurred,
              this, &VideoThumbnailExtractor::onError);
    }

    void requestThumbnail(const QString &path, const QSize &size){
      m_queue.enqueue({path, size});
      if (!m_capturing)
        processNext();
    }

  signals:
    void thumbnailReady(const QString &path, const QPixmap &thumbnail);

  private slots:
    void onVideoFrameChanged(const QVideoFrame &frame){
      if (!m_capturing || !frame.isValid()) return;
      m_capturing = false;
      m_player->stop();
      m_player->setSource(QUrl());

      QImage image = frame.toImage();
      QPixmap pixmap;
      if (!image.isNull()){
        image = image.scaled(m_currentSize, Qt::KeepAspectRatio,
                             Qt::SmoothTransformation);
        pixmap = QPixmap::fromImage(std::move(image));
      }
      emit thumbnailReady(m_currentPath, pixmap);
      processNext();
    }

    void onError(QMediaPlayer::Error, const QString &){
      if (!m_capturing) return;
      m_capturing = false;
      m_player->setSource(QUrl());
      emit thumbnailReady(m_currentPath, QPixmap());
      processNext();
    }

  private:
    void processNext(){
      if (m_queue.isEmpty()){
        m_capturing = false;
        return;
      }
      auto req = m_queue.dequeue();
      m_currentPath = req.path;
      m_currentSize = req.size;
      m_capturing = true;
      m_player->setSource(QUrl::fromLocalFile(m_currentPath));
      m_player->play();
    }

    struct Request { QString path; QSize size; };
    QMediaPlayer *m_player;
    QVideoSink *m_sink;
    QString m_currentPath;
    QSize m_currentSize;
    QQueue<Request> m_queue;
    bool m_capturing = false;
};

// ── LocalThumbnailProvider ────────────────────────────────────────────────────

LocalThumbnailProvider::LocalThumbnailProvider(QObject *parent)
  : ThumbnailProvider(parent)
  , m_videoExtractor(new VideoThumbnailExtractor(this)) {
  connect(m_videoExtractor, &VideoThumbnailExtractor::thumbnailReady,
          this, [this](const QString &path, const QPixmap &pixmap){
            m_pending.remove(path);
            if (pixmap.isNull()) return;
            m_cache[path] = pixmap;
            emit thumbnailReady(path, pixmap);
          });
}

void LocalThumbnailProvider::requestThumbnail(const QString &path, const QSize &size){
  if (m_cache.contains(path)){
    emit thumbnailReady(path, m_cache[path]);
    return;
  }
  if (m_pending.contains(path))
    return;
  m_pending.insert(path);

  if (isVideoPath(path)){
    m_videoExtractor->requestThumbnail(path, size);
    return;
  }

  QFuture<QImage> future = QtConcurrent::run([path, size]() -> QImage {
    QImageReader reader(path);
    reader.setAutoTransform(true);
    QSize imageSize = reader.size();
    if (imageSize.isValid()){
      imageSize.scale(size, Qt::KeepAspectRatio);
      reader.setScaledSize(imageSize);
    }
    return reader.read();
  });

  auto *watcher = new QFutureWatcher<QImage>(this);
  connect(watcher, &QFutureWatcher<QImage>::finished, this,
          [this, path, watcher](){
            m_pending.remove(path);
            QImage image = watcher->result();
            watcher->deleteLater();
            if (image.isNull()) return;
            QPixmap pixmap = QPixmap::fromImage(std::move(image));
            m_cache[path] = pixmap;
            emit thumbnailReady(path, pixmap);
          });
  watcher->setFuture(future);
}

#include "localthumbnailprovider.moc"
