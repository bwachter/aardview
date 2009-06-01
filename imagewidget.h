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
  QLabel* imageContainer;
  QImage displayedImage;
  QImage originalImage;
  QPixmap displayedPixmap;
  QScrollArea* imageArea;
  QScrollArea* infoArea;
  QSettings settings;
  void displayImage();

  public slots:
  void load(QString pathname);
  void toggleFtw();

  signals:

  protected:
  bool eventFilter(QObject *obj, QEvent *ev);
};


#endif
