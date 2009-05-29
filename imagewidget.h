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
  QLabel* imageContainer;
  QImage displayedImage;
  QImage originalImage;
  QScrollArea* imageArea;
  QScrollArea* infoArea;
  QSettings settings;

  public slots:
  void load(QString pathname);
  void toggleScaled();
  void toggleFullscreen();
  void adjustSize();

  signals:

  protected:
  bool eventFilter(QObject *obj, QEvent *ev);
};


#endif
