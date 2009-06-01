#include "imagewidget.h"

ImageWidget::ImageWidget(): QWidget(){
  QVBoxLayout *layout = new QVBoxLayout;
  imageContainer = new QLabel;

  imageArea = new QScrollArea;
  imageArea->installEventFilter(this);
  infoArea = new QScrollArea;
  infoArea->setFocusPolicy(Qt::NoFocus);

  imageArea->setWidgetResizable(true);

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(imageArea);
  layout->addWidget(infoArea);
  setLayout(layout);

  scaleFactor=1.0;
  if (settings.value("viewer/hideInfoArea").toBool())
    infoArea->hide();
  if (settings.value("viewer/resetFtwOnChange").toBool())
    fitToWindow=settings.value("viewer/fitToWindow").toBool();

  imageArea->setWidget(imageContainer);

  load(":/images/aardview.png");
}

void ImageWidget::load(QString pathname){
  originalImage.load(pathname);
  if (settings.value("viewer/resetFtwOnChange").toBool())
    fitToWindow=settings.value("viewer/fitToWindow").toBool();
  //if (settings.value("viewer/resetScalingOnChange").toBool())
  // FIXME, add scaling
  displayedPixmap=QPixmap::fromImage(originalImage);
  displayImage();
}

void ImageWidget::displayImage(){
  bool keepAspectRatio=true;
  if (fitToWindow){
    // fit the image to the window, keeping the aspect ratio
    if (keepAspectRatio){
      int p = settings.value("viewer/padding").toInt();
      imageContainer->setPixmap(
        displayedPixmap.scaled(this->size() - QSize::QSize(p, p),
                               Qt::KeepAspectRatio,
                               Qt::SmoothTransformation));
    } else {
      // fit the image to the window, ignoring the aspect ratio
      imageContainer->setPixmap(QPixmap::fromImage(originalImage));
    }
  } else if (scaleFactor != 1.0) {
    imageContainer->setPixmap(
      displayedPixmap.scaled(scaleFactor * displayedPixmap.size(),
      Qt::KeepAspectRatio,
      Qt::SmoothTransformation));
  } else {
    // display the picture in its original size
    imageContainer->setPixmap(displayedPixmap);
  }
  imageContainer->setScaledContents(false);
  imageContainer->adjustSize();
}

void ImageWidget::normalSize(){ scale(0); }

void ImageWidget::rotate(){
}

void ImageWidget::toggleFtw(){
  if (fitToWindow) fitToWindow=false;
  else fitToWindow=true;
  displayImage();
}

void ImageWidget::scale(double factor){
  fitToWindow=false;
  scaleFactor*=factor;
  if (scaleFactor==0) scaleFactor++;
  displayImage();
}
  
void ImageWidget::zoomIn(){ scale(1.25); }

void ImageWidget::zoomOut(){ scale(0.8); }

bool ImageWidget::eventFilter(QObject *obj, QEvent *event){
  if (obj == imageArea){
    if (event->type() == QEvent::KeyPress){
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
      switch(keyEvent->key()){
        case Qt::Key_N:
          normalSize();
          break;
        case Qt::Key_R:
          rotate();
          break;
        case Qt::Key_Z:
          toggleFtw();
          break;
        case Qt::Key_Minus:
          zoomOut();
          break;
        case Qt::Key_Plus:
          zoomIn();
          break;
        default:
          return QWidget::eventFilter(obj, event);
      };
      // if we got here we consumed the event
      return true;
    } 
  }
  // if we got here we don't care about the event
  return QWidget::eventFilter(obj, event);
}

