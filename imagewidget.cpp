#include "imagewidget.h"

ImageWidget::ImageWidget(): QWidget(){
  QVBoxLayout *layout = new QVBoxLayout;
  imageContainer = new QLabel;
  imageContainer->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

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

  if (settings.value("main/hideInfoArea").toBool())
    infoArea->hide();

  imageArea->setWidget(imageContainer);
/*
  //load image from resource
  load("");
  imageContainer->setPixmap(QPixmap::fromImage(originalImage));
*/
}

void ImageWidget::load(QString pathname){
  originalImage.load(pathname);
  imageContainer->setPixmap(QPixmap::fromImage(originalImage));
  adjustSize();
}

void ImageWidget::adjustSize(){
  imageContainer->setScaledContents(scaledImage);
  imageContainer->adjustSize();
}

void ImageWidget::toggleFullscreen(){
  if (scaledImage){
    // enable image scaling
    scaledImage=false;
    imageContainer->setScaledContents(true);
  } else {
    scaledImage=true;
    imageContainer->setScaledContents(false);
  }
}

void ImageWidget::toggleScaled(){
  if (scaledImage) scaledImage=false;
  else scaledImage=true;
  adjustSize();
}

bool ImageWidget::eventFilter(QObject *obj, QEvent *event){
  if (obj == imageArea){
    if (event->type() == QEvent::KeyPress){
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
      switch(keyEvent->key()){
        case Qt::Key_Z:
          toggleScaled();
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

