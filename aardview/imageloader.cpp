#include "imageloader.h"
#include "settingsdialog.h"

/*
  The UI part of this class will end up in the mainwindow, while
  the image rescaling logic will end up in a caching multithreaded
  image loader. Though it would be possible to implement a simple,
  non-threaded image loader it's just not worth the trouble for v0.1
*/

ImageLoader::ImageLoader(): QObject(){
  scaleFactor=1.0;

  reconfigure();
}

void ImageLoader::reconfigure(){
  SettingsDialog *settings = SettingsDialog::instance();

  qDebug() << "Checking configuration settings (viewer)";
  if (settings->value("viewer/resetFtwOnChange").toBool())
    fitToWindow=settings->value("viewer/fitToWindow").toBool();
  if (settings->value("viewer/smoothTransformation").toBool())
    transformation=Qt::SmoothTransformation;
}

void ImageLoader::load(const QString &pathname, const QSize &widgetViewSize){
  SettingsDialog *settings = SettingsDialog::instance();

  qDebug() << "Requested pixmap for " << pathname
           << " with size " << widgetViewSize;
  viewSize=widgetViewSize;
  originalImage.load(pathname);
  if (originalImage.isNull()){
    qDebug() << "Unable to load image: " << pathname;
    // FIXME, set pixmap to a valid pixmap or one from resource, configurable
    displayedPixmap = QPixmap();
  } else {
    // FIXME, last image should be tracked per instance
    //settings->setValue("viewer/lastImage", pathname);
    imageFileName=pathname;
    displayedPixmap=QPixmap::fromImage(originalImage);
  }

  if (settings->value("viewer/resetFtwOnChange").toBool())
    fitToWindow=settings->value("viewer/fitToWindow").toBool();
  //if (settings->value("viewer/resetScalingOnChange").toBool())
  // FIXME, add scaling

  displayImage();
}

void ImageLoader::displayImage(){
  if (displayedPixmap.isNull()){
    qDebug() << "Skipping display stuff due to the lack of a pixmap";
    return;
  }
  SettingsDialog *settings = SettingsDialog::instance();

  bool keepAspectRatio=true;
  int p = settings->value("viewer/padding").toInt();
  QSize pixmapSize=displayedPixmap.size();
  QSize paddingSize(p, p);

  // only scale if shrinkOnly is set to false _or_
  // shrinkonly is true and at least one dimension of the
  // image is larger than the view area
  if (fitToWindow &&
      (!settings->value("viewer/shrinkOnly").toBool() ||
      (settings->value("viewer/shrinkOnly").toBool() &&
       (pixmapSize.height() >= viewSize.height() ||
        pixmapSize.width() >= viewSize.width())))) {
    // fit the image to the window, keeping the aspect ratio
    if (keepAspectRatio){
      emit pixmapReady(
        displayedPixmap.scaled(viewSize - paddingSize,
                               Qt::KeepAspectRatio,
                               transformation));
    } else {
      // fit the image to the window, ignoring the aspect ratio
      emit pixmapReady(QPixmap::fromImage(originalImage));
    }
  } else if (scaleFactor != 1.0) {
    emit pixmapReady(
      displayedPixmap.scaled(scaleFactor * displayedPixmap.size(),
      Qt::KeepAspectRatio,
      transformation));
  } else {
    // display the picture in its original size
    emit pixmapReady(displayedPixmap);
  }
//imageContainer->setScaledContents(false);
}

QString ImageLoader::currentFilename(){ return imageFileName; }

void ImageLoader::normalSize(){ scale(0); }

void ImageLoader::rotate(){
}

void ImageLoader::toggleFtw(){
  if (fitToWindow) fitToWindow=false;
  else fitToWindow=true;
  displayImage();
}

void ImageLoader::scale(double factor){
  fitToWindow=false;
  scaleFactor*=factor;
  if (scaleFactor==0) scaleFactor++;
  displayImage();
}

void ImageLoader::repaint(const QSize &widgetViewSize){
  viewSize=widgetViewSize;
  displayImage();
}

void ImageLoader::zoomIn(){ scale(1.25); }

void ImageLoader::zoomOut(){ scale(0.8); }
