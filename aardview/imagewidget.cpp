#include "imagewidget.h"

ImageWidget::ImageWidget(): QWidget(){
  ui.setupUi(this);

  infoContainer=new QLabel;
  imageContainer=new QLabel;

  scaleFactor=1.0;

  reconfigure();

  ui.imageArea->setWidget(imageContainer);
  // TODO maybe move the infocontainer to a dock, too?
  ui.infoArea->setWidget(infoContainer);

  updateInformation();
  load(":/images/aardview.png");
}

void ImageWidget::reconfigure(){
  qDebug() << "Checking configuration settings (viewer)";
  ui.infoArea->setVisible(!settings.value("viewer/hideInfoArea").toBool());
  if (settings.value("viewer/resetFtwOnChange").toBool())
    fitToWindow=settings.value("viewer/fitToWindow").toBool();
  if (settings.value("viewer/smoothTransformation").toBool())
    transformation=Qt::SmoothTransformation;
}

void ImageWidget::load(QString pathname){
  originalImage.load(pathname);
  if (originalImage.isNull()){
    qDebug() << "Unable to load image: " << pathname;
    // FIXME, set pixmap to a valid pixmap or one from resource, configurable
    displayedPixmap = 0;
  } else {
    imageFileName=pathname;
    displayedPixmap=QPixmap::fromImage(originalImage);
  }

  if (settings.value("viewer/resetFtwOnChange").toBool())
    fitToWindow=settings.value("viewer/fitToWindow").toBool();
  //if (settings.value("viewer/resetScalingOnChange").toBool())
  // FIXME, add scaling

  displayImage();
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
        displayedPixmap.scaled(viewSize - QSize::QSize(p, p),
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
  imageContainer->adjustSize();
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

void ImageWidget::updateInformation(){
  infoContainer->setText("<h3>Image information</h3>Sorry, not yet implemented");
}
  
void ImageWidget::zoomIn(){ scale(1.25); }

void ImageWidget::zoomOut(){ scale(0.8); }

void ImageWidget::enterEvent(QEvent *e){
  if (settings.value("main/focusFollowsMouse").toBool())
    ui.imageArea->setFocus();
  QWidget::enterEvent(e);
}

