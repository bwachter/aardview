/* imagewidget.h        -- Bernd Wachter <bwachter@lart.info>
 *
 * $Header$
 * $Log$
 */

#ifndef _IMAGEWIDGET_H
#define _IMAGEWIDGET_H

#include <QtGui>

#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

#include "ui_imagewidget.h"

class ImageWidget: public QWidget, private Ui::ImageWidget {
  Q_OBJECT

  public:
  ImageWidget(QWidget *parent=0);
  QString currentFilename();

  private:
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
  void repaint();

  public slots:
  void loadInitialFile();
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

  private slots:
  void paintToPrinter(QPrinter *printer);

  signals:

  protected:
  void enterEvent(QEvent *e);
  void paintEvent(QPaintEvent *e);
};


#endif
