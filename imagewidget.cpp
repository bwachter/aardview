#include "imagewidget.h"

ImageWidget::ImageWidget(QWidget* parent): QWidget(parent){
  QVBoxLayout *layout = new QVBoxLayout;
  imageContainer = new QLabel;

  layout->addWidget(imageContainer);
  setLayout(layout);
/*
  //load image from resource
  load("");
  imageContainer->setPixmap(QPixmap::fromImage(originalImage));
*/
}

void ImageWidget::load(QString pathname){
  //originalImage.load(pathname);
  originalImage.load(pathname);
  //loadImage(pathname, &image, this);
  imageContainer->setPixmap(QPixmap::fromImage(originalImage));
}
