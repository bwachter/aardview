/* imageloader.h        -- Bernd Wachter <bwachter@lart.info>
 *
 * $Header$
 * $Log$
 */

#ifndef _IMAGELOADER_H
#define _IMAGELOADER_H

#include <QImage>
#include <QPixmap>
#include <QDebug>

class ImageLoader: public QObject {
    Q_OBJECT

  public:
    ImageLoader();
    QString currentFilename();

  private:
    bool scaledImage;
    bool fitToWindow;
    double scaleFactor;
    QImage displayedImage;
    QImage originalImage;
    QPixmap displayedPixmap;
    QString imageFileName;
    QSize viewSize;
    Qt::TransformationMode transformation;
    void displayImage();
    void scale(double factor);

  public slots:
    void load(const QString &pathname, const QSize &widgetViewSize);
    void toggleFtw();
    void normalSize();
    void reconfigure();
    void repaint(const QSize &widgetViewSize);
    void rotate();
    void zoomIn();
    void zoomOut();

  private slots:

  signals:
    void pixmapReady(const QPixmap &picture);
};


#endif
