/* imagewidget.h	-- Bernd Wachter <bwachter@lart.info>
 *
 * $Header$
 * $Log$
 */

#ifndef _IMAGEWIDGET_H
#define _IMAGEWIDGET_H

#include <QtGui>

class ImageWidget: public QWidget {
  Q_OBJECT

  public:
  ImageWidget();
  QString currentFilename();

  private:
  bool scaledImage;
  bool fitToWindow;
  double scaleFactor;
  QLabel* imageContainer;
  QImage displayedImage;
  QImage originalImage;
  QPixmap displayedPixmap;
  QScrollArea* imageArea;
  QScrollArea* infoArea;
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

  private slots:
  void paintToPrinter(QPrinter *printer);

  signals:

  protected:
  void enterEvent(QEvent *e);
};


#endif