/**
 * @file aprintpreviewdialog.h
 * @copyright 2016 GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2016
 */

#ifndef _APRINTPREVIEWDIALOG_H
#define _APRINTPREVIEWDIALOG_H

#ifndef QT_NO_PRINTER
#include <QPrintPreviewDialog>
#include <QPixmap>

/**
 * Extend QPrintPreviewDialog to allow passing in a QPixmap to be printed
 * (and shown in the preview)
 *
 * This works for this kind of simple application where the pixmap is
 * everything supposed to be printed.
 */
class APrintPreviewDialog: public QPrintPreviewDialog {
    Q_OBJECT

  public:
    APrintPreviewDialog(): QPrintPreviewDialog(){}
    APrintPreviewDialog(QPrinter *printer,
                        QWidget *parent=0):
      QPrintPreviewDialog(printer, parent){
      init();
    }
    APrintPreviewDialog(QPrinter *printer,
                        const QPixmap &pixmap,
                        QWidget *parent=0):
      QPrintPreviewDialog(printer, parent){
      m_pixmap = pixmap;
      init();
    }

    /**
     * Set the pixmap for printing
     *
     * @param pixmap the pixmap
     */
    void setPixmap(const QPixmap &pixmap){
      m_pixmap = pixmap;
      init();
    }

  private:
    QPixmap m_pixmap;

    /**
     * Set up signal/slot connection required for passing the pixmap
     * to the painter
     */
    void init(){
      connect(this,
              SIGNAL(paintRequested(QPrinter *)),
              this,
              SLOT(forwardPaintRequested(QPrinter *)));
    }

  private slots:
    void forwardPaintRequested(QPrinter *printer){
      emit paintRequested(printer, m_pixmap);
    }

  signals:
    /*
     * A signal to hook up to the paint function when painting is required.
     *
     * @param printer the printer object
     * @param pixmap the pixmap to print
     */
    void paintRequested(QPrinter * printer, const QPixmap &pixmap);
};

#endif

#endif
