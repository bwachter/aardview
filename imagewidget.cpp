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
    if (keepAspectRatio){
      int p = settings.value("viewer/padding").toInt();
      imageContainer->setPixmap(
        displayedPixmap.scaled(this->size() - QSize::QSize(p, p),
                               Qt::KeepAspectRatio,
                               Qt::SmoothTransformation));
    } else {
      imageContainer->setPixmap(QPixmap::fromImage(originalImage));
    }
  } else {
    imageContainer->setPixmap(displayedPixmap);
  }
  imageContainer->setScaledContents(false);
  imageContainer->adjustSize();
}

void ImageWidget::toggleFtw(){
  if (fitToWindow) fitToWindow=false;
  else fitToWindow=true;
  displayImage();
}

bool ImageWidget::eventFilter(QObject *obj, QEvent *event){
  if (obj == imageArea){
    if (event->type() == QEvent::KeyPress){
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
      switch(keyEvent->key()){
        case Qt::Key_Z:
          toggleFtw();
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

