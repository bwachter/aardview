#ifndef _QMLIMAGEPROVIDER_H
#define _QMLIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QCache>
#include <QPixmap>

class QmlImageProvider : public QQuickImageProvider {
    Q_OBJECT
  public:
    explicit QmlImageProvider();
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;

  private:
    QCache<QString, QPixmap> m_cache;
};

#endif
