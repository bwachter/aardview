/**
 * @file imageloader.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#include <QTime>

#include "imageloader.h"
#include "settingsdialog.h"

ImageLoader::ImageLoader(): QObject(){
  m_scaleFactor=1.0;

  reconfigure();
}

void ImageLoader::reconfigure(){
  SettingsDialog *settings = SettingsDialog::instance();

#ifdef DEBUG_SETTINGS
  qDebug() << "Checking configuration settings (viewer)";
#endif

  if (settings->value("viewer/resetFtwOnChange").toBool())
    m_fitToWindow=settings->value("viewer/fitToWindow").toBool();
  if (settings->value("viewer/smoothTransformation").toBool())
    m_transformation=Qt::SmoothTransformation;
}

void ImageLoader::load(const QString &pathname, const QSize &widgetViewSize){
  SettingsDialog *settings = SettingsDialog::instance();
  QTime timer;

  m_viewSize=widgetViewSize;

  timer.start();
  m_pixmap.load(pathname);

  qDebug() << "loaded " << pathname << "for" << widgetViewSize
           << "in" << timer.elapsed() << "ms";

  if (m_pixmap.isNull()){
    qDebug() << "Unable to load image: " << pathname;
    // FIXME, set pixmap to a valid pixmap or one from resource, configurable
    m_pixmap = QPixmap();
  } else {
    /// @todo last image should be tracked per instance; move to generic session saving
    //settings->setValue("viewer/lastImage", pathname);
    m_imageFileName=pathname;
  }

  if (settings->value("viewer/resetFtwOnChange").toBool())
    m_fitToWindow=settings->value("viewer/fitToWindow").toBool();
  //if (settings->value("viewer/resetScalingOnChange").toBool())
  // FIXME, add scaling

  displayImage();
}

void ImageLoader::displayImage(){
  if (m_pixmap.isNull()){
    qDebug() << "Skipping display stuff due to the lack of a pixmap";
    return;
  }
  SettingsDialog *settings = SettingsDialog::instance();

  bool keepAspectRatio=true;
  int p = settings->value("viewer/padding").toInt();
  QSize pixmapSize=m_pixmap.size();
  QSize paddingSize(p, p);

  // only scale if shrinkOnly is set to false _or_
  // shrinkonly is true and at least one dimension of the
  // image is larger than the view area
  if (m_fitToWindow &&
      (!settings->value("viewer/shrinkOnly").toBool() ||
      (settings->value("viewer/shrinkOnly").toBool() &&
       (pixmapSize.height() >= m_viewSize.height() ||
        pixmapSize.width() >= m_viewSize.width())))) {
    // fit the image to the window, keeping the aspect ratio
    if (keepAspectRatio){
      emit pixmapReady(
        m_pixmap.scaled(m_viewSize - paddingSize,
                        Qt::KeepAspectRatio,
                        m_transformation));
    } else {
      // fit the image to the window, ignoring the aspect ratio
      emit pixmapReady(
        m_pixmap.scaled(m_viewSize - paddingSize,
                        Qt::IgnoreAspectRatio,
                        m_transformation));
    }
  } else if (m_scaleFactor != 1.0) {
    emit pixmapReady(
      m_pixmap.scaled(m_scaleFactor * m_pixmap.size(),
      Qt::KeepAspectRatio,
      m_transformation));
  } else {
    // display the picture in its original size
    emit pixmapReady(m_pixmap);
  }
}

QString ImageLoader::currentFilename() const{ return m_imageFileName; }

QPixmap ImageLoader::currentPixmap() const{ return m_pixmap; }

void ImageLoader::normalSize(){ scale(0); }

void ImageLoader::rotate(){
}

void ImageLoader::toggleFtw(){
  if (m_fitToWindow) m_fitToWindow=false;
  else m_fitToWindow=true;
  displayImage();
}

void ImageLoader::scale(double factor){
  /// @todo: scale based on current size (fitted to window) instead of image size
  m_fitToWindow=false;
  m_scaleFactor*=factor;
  if (m_scaleFactor==0) m_scaleFactor++;
  displayImage();
}

void ImageLoader::repaint(const QSize &widgetViewSize){
  m_viewSize=widgetViewSize;
  displayImage();
}

void ImageLoader::zoomIn(){ scale(1.25); }

void ImageLoader::zoomOut(){ scale(0.8); }
