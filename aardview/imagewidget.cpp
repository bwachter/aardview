#include "imagewidget.h"

/*
  The UI part of this class will end up in the mainwindow, while
  the image rescaling logic will end up in a caching multithreaded
  image loader. Though it would be possible to implement a simple,
  non-threaded image loader it's just not worth the trouble for v0.1
*/

ImageWidget::ImageWidget(QWidget *parent): QWidget(parent){
  setupUi(this);

  // FIXME, configure and add image name
  imageName->hide();
  scaleFactor=1.0;

  reconfigure();

  // FIXME, send signal when information update is required
  // FIXME, move this to an enum; 0=default,1=last,2=nothing

  // used to make sure the image get's loaded after UI has been
  // set up, giving us correct widget sizes
  QTimer::singleShot(0, this, SLOT(loadInitialFile()));
}

void ImageWidget::reconfigure(){
  qDebug() << "Checking configuration settings (viewer)";
  if (settings.value("viewer/resetFtwOnChange").toBool())
    fitToWindow=settings.value("viewer/fitToWindow").toBool();
  if (settings.value("viewer/smoothTransformation").toBool())
    transformation=Qt::SmoothTransformation;
}

void ImageWidget::loadInitialFile(){
  if (imageFileName == ""){
    if (settings.value("viewer/loadAction").toInt()==0)
      load(":/images/aardview.png");
    else if (settings.value("viewer/loadAction").toInt()==1)
      load(settings.value("viewer/lastImage").toString());
  }
}

void ImageWidget::load(QString pathname){
  originalImage.load(pathname);
  if (originalImage.isNull()){
    qDebug() << "Unable to load image: " << pathname;
    // FIXME, set pixmap to a valid pixmap or one from resource, configurable
    displayedPixmap = 0;
  } else {
    settings.setValue("viewer/lastImage", pathname);
    imageFileName=pathname;
    displayedPixmap=QPixmap::fromImage(originalImage);
  }

  if (settings.value("viewer/resetFtwOnChange").toBool())
    fitToWindow=settings.value("viewer/fitToWindow").toBool();
  //if (settings.value("viewer/resetScalingOnChange").toBool())
  // FIXME, add scaling

  displayImage();
  imageArea->verticalScrollBar()->setValue(0);
  imageArea->horizontalScrollBar()->setValue(0);
}

void ImageWidget::displayImage(){
  if (displayedPixmap.isNull()){
    qDebug() << "Skipping display stuff due to the lack of a pixmap";
    return;
  }
  bool keepAspectRatio=true;
  int p = settings.value("viewer/padding").toInt();
  QSize viewSize=this->size();
  QSize pixmapSize=displayedPixmap.size();
  QSize paddingSize(p, p);

  // only scale if shrinkOnly is set to false _or_
  // shrinkonly is true and at least one dimension of the
  // image is larger than the view area
  if (fitToWindow &&
      (!settings.value("viewer/shrinkOnly").toBool() ||
      (settings.value("viewer/shrinkOnly").toBool() &&
       (pixmapSize.height() >= viewSize.height() ||
        pixmapSize.width() >= viewSize.width())))) {
    // fit the image to the window, keeping the aspect ratio
    if (keepAspectRatio){
      imageContainer->setPixmap(
        displayedPixmap.scaled(viewSize - paddingSize,
                               Qt::KeepAspectRatio,
                               transformation));
    } else {
      // fit the image to the window, ignoring the aspect ratio
      imageContainer->setPixmap(QPixmap::fromImage(originalImage));
    }
  } else if (scaleFactor != 1.0) {
    imageContainer->setPixmap(
      displayedPixmap.scaled(scaleFactor * displayedPixmap.size(),
      Qt::KeepAspectRatio,
      transformation));
  } else {
    // display the picture in its original size
    imageContainer->setPixmap(displayedPixmap);
  }
  imageContainer->setScaledContents(false);
}

QString ImageWidget::currentFilename(){ return imageFileName; }

void ImageWidget::normalSize(){ scale(0); }

void ImageWidget::rotate(){
}

void ImageWidget::toggleFtw(){
  if (fitToWindow) fitToWindow=false;
  else fitToWindow=true;
  displayImage();
}

void ImageWidget::open(){
  QString fileName =
    QFileDialog::getOpenFileName(this,
                                 tr("Open File"), QDir::currentPath());
  if (!fileName.isEmpty()) {
    load(fileName);
  }
}

void ImageWidget::print(){
#ifndef QT_NO_PRINTER
  QPrintDialog dialog(&printer, this);
  if (dialog.exec()) {
    paintToPrinter(&printer);
  }
#endif
}

void ImageWidget::printPreview(){
#ifndef QT_NO_PRINTER
  QPrintPreviewDialog dialog(&printer, this);
  connect(&dialog, SIGNAL(paintRequested(QPrinter *)),
          this, SLOT(paintToPrinter(QPrinter *)));
  dialog.exec();
#endif
}

void ImageWidget::paintToPrinter(QPrinter *printer){
#ifndef QT_NO_PRINTER
  QPainter painter(printer);
  QRect rect = painter.viewport();
  QSize size = displayedPixmap.size();
  size.scale(rect.size(), Qt::KeepAspectRatio);
  painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
  painter.setWindow(displayedPixmap.rect());
  painter.drawPixmap(0, 0, displayedPixmap);
#endif
}

void ImageWidget::scale(double factor){
  fitToWindow=false;
  scaleFactor*=factor;
  if (scaleFactor==0) scaleFactor++;
  displayImage();
}

void ImageWidget::repaint(){
  displayImage();
}

void ImageWidget::zoomIn(){ scale(1.25); }

void ImageWidget::zoomOut(){ scale(0.8); }

void ImageWidget::enterEvent(QEvent *e){
  if (settings.value("main/focusFollowsMouse").toBool())
    imageArea->setFocus();
  QWidget::enterEvent(e);
}

void ImageWidget::paintEvent(QPaintEvent *e){
  repaint();
  QWidget::paintEvent(e);
}
