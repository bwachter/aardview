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
  ImageWidget(QWidget* parent = 0);

  private:
  QLabel* imageContainer;
  QImage displayedImage;
  QImage originalImage;
  public slots:
  void load(QString pathname);

  signals:

};


#endif
