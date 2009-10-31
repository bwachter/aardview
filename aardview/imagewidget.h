/* imagewidget.h	-- Bernd Wachter <bwachter@lart.info>
 *
 * $Header$
 * $Log$
 */

#ifndef _IMAGEWIDGET_H
#define _IMAGEWIDGET_H

#include <QtGui>
#include "ui_imagewidget.h"

class ImageWidget: public QWidget {
  Q_OBJECT

  public:
  ImageWidget();
  QString currentFilename();

  private:
  Ui::ImageWidget ui;
  QLabel *imageContainer;
  QLabel *infoContainer;
  bool scaledImage;
  bool fitToWindow;
  double scaleFactor;
  QImage displayedImage;
  QImage originalImage;
  QPixmap displayedPixmap;
  QSettings settings;
  QString imageFileName;
  Qt::TransformationMode transformation;
  void displayImage();
  void scale(double factor);
#ifndef QT_NO_PRINTER
  QPrinter printer;
#endif

  public slots:
  void load(QString pathname);
  void toggleFtw();
  void normalSize();
  void open();
  void print();
  void printPreview();
  void reconfigure();
  void rotate();
  void zoomIn();
  void zoomOut();
  void updateInformation();

  private slots:
  void paintToPrinter(QPrinter *printer);

  signals:

  protected:
  void enterEvent(QEvent *e);
};


#endif
