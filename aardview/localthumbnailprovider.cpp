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
#include <QThread>
#include <QTimer>
#include <QVideoFrame>
#include <QVideoSink>
#include <QtConcurrent>

#include "localthumbnailprovider.h"

static bool isVideoPath(const QString &path){
  if (path.startsWith(":/")) return false;
  return QMimeDatabase().mimeTypeForFile(path).name().startsWith("video/");
}

// Cache and pending-set key: "path@width" — encodes both identity and requested
// size so that changing thumbnail size never serves a stale cached pixmap.
static QString cacheKey(const QString &path, const QSize &size){
  return path + QChar('@') + QString::number(size.width());
}

// Processes video thumbnail requests sequentially on the main thread.
//
// A 10-second timeout guards against files that never produce a decodable
// frame, ensuring the queue never stalls.
class VideoThumbnailExtractor: public QObject {
    Q_OBJECT

    enum State { Idle, Loading, Capturing };

  public:
    explicit VideoThumbnailExtractor(QObject *parent = nullptr)
      : QObject(parent)
      , m_player(new QMediaPlayer(this))
      , m_sink(new QVideoSink(this))
      , m_state(Idle) {
      m_player->setVideoSink(m_sink);
      connect(m_sink, &QVideoSink::videoFrameChanged,
              this, &VideoThumbnailExtractor::onVideoFrameChanged);
      connect(m_player, &QMediaPlayer::mediaStatusChanged,
              this, &VideoThumbnailExtractor::onMediaStatusChanged);
      connect(m_player, &QMediaPlayer::errorOccurred,
              this, &VideoThumbnailExtractor::onError);
      m_timeout.setSingleShot(true);
      connect(&m_timeout, &QTimer::timeout,
              this, &VideoThumbnailExtractor::onTimeout);
    }

  public slots:
    // key is the provider's cache key (path@width); path is the actual file.
    void requestThumbnail(const QString &key, const QString &path, const QSize &size){
      m_queue.enqueue({key, path, size});
      if (m_state == Idle)
        processNext();
    }

  signals:
    void thumbnailReady(const QString &key, const QPixmap &thumbnail);

  private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status){
      if (m_state != Loading) return;
      if (status == QMediaPlayer::LoadedMedia ||
          status == QMediaPlayer::BufferedMedia){
        // Seek to ~10 % of duration (capped at 5 s) for a non-black frame.
        qint64 seekMs = 0;
        if (m_player->duration() > 10000)
          seekMs = qMin(m_player->duration() / 10, qint64(5000));
        m_state = Capturing;   // set before setPosition so frames aren't missed
        if (seekMs > 0)
          m_player->setPosition(seekMs);
      } else if (status == QMediaPlayer::InvalidMedia){
        skipCurrent();
      }
    }

    void onVideoFrameChanged(const QVideoFrame &frame){
      if (m_state != Capturing || !frame.isValid()) return;
      m_state = Idle;
      m_timeout.stop();
      m_player->stop();
      m_player->setSource(QUrl());

      QImage image = frame.toImage();
      QPixmap pixmap;
      if (!image.isNull()){
        image = image.scaled(m_currentSize, Qt::KeepAspectRatio,
                             Qt::SmoothTransformation);
        pixmap = QPixmap::fromImage(std::move(image));
      }
      emit thumbnailReady(m_currentKey, pixmap);
      processNext();
    }

    void onError(QMediaPlayer::Error, const QString &){
      if (m_state == Idle) return;
      skipCurrent();
    }

    void onTimeout(){
      if (m_state == Idle) return;
      skipCurrent();
    }

  private:
    void processNext(){
      if (m_queue.isEmpty()){
        m_state = Idle;
        return;
      }
      auto req = m_queue.dequeue();
      m_currentKey  = req.key;
      m_currentSize = req.size;
      m_state = Loading;
      m_player->setSource(QUrl::fromLocalFile(req.path));
      m_player->play();
      m_timeout.start(10000);
    }

    void skipCurrent(){
      m_timeout.stop();
      m_state = Idle;
      m_player->stop();
      m_player->setSource(QUrl());
      emit thumbnailReady(m_currentKey, QPixmap());
      processNext();
    }

    struct Request { QString key; QString path; QSize size; };
    QMediaPlayer *m_player;
    QVideoSink   *m_sink;
    QTimer        m_timeout;
    QString       m_currentKey;
    QSize         m_currentSize;
    QQueue<Request> m_queue;
    State           m_state;
};

// ── LocalThumbnailProvider ────────────────────────────────────────────────────

LocalThumbnailProvider::LocalThumbnailProvider(QObject *parent)
  : ThumbnailProvider(parent)
  , m_videoExtractor(new VideoThumbnailExtractor())
  , m_videoThread(new QThread(this)) {
  m_videoExtractor->moveToThread(m_videoThread);
  m_videoThread->start();

  connect(m_videoExtractor, &VideoThumbnailExtractor::thumbnailReady,
          this, [this](const QString &key, const QPixmap &pixmap){
            m_pending.remove(key);
            if (pixmap.isNull()) return;
            m_cache[key] = pixmap;
            // strip "@width" suffix to recover the original file path
            emit thumbnailReady(key.left(key.lastIndexOf(QChar('@'))), pixmap);
          });

  connect(this, &LocalThumbnailProvider::videoThumbnailRequested,
          m_videoExtractor, &VideoThumbnailExtractor::requestThumbnail,
          Qt::QueuedConnection);
}

LocalThumbnailProvider::~LocalThumbnailProvider(){
  m_videoThread->quit();
  m_videoThread->wait();
  delete m_videoExtractor;
}

void LocalThumbnailProvider::requestThumbnail(const QString &path, const QSize &size){
  QString key = cacheKey(path, size);

  if (m_cache.contains(key)){
    emit thumbnailReady(path, m_cache[key]);
    return;
  }
  if (m_pending.contains(key))
    return;
  m_pending.insert(key);

  if (isVideoPath(path)){
    emit videoThumbnailRequested(key, path, size);
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
          [this, path, key, watcher](){
            m_pending.remove(key);
            QImage image = watcher->result();
            watcher->deleteLater();
            if (image.isNull()) return;
            QPixmap pixmap = QPixmap::fromImage(std::move(image));
            m_cache[key] = pixmap;
            emit thumbnailReady(path, pixmap);
          });
  watcher->setFuture(future);
}

#include "localthumbnailprovider.moc"
