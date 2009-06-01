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
  void displayImage();
  void scale(double factor);

  public slots:
  void load(QString pathname);
  void toggleFtw();
  void normalSize();
  void rotate();
  void zoomIn();
  void zoomOut();

  signals:

  protected:
  bool eventFilter(QObject *obj, QEvent *ev);
  void enterEvent(QEvent *e);
};


#endif
