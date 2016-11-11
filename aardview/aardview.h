/* aardview.h   -- Bernd Wachter <bwachter@lart.info>
 *
 * $Header$
 * $Log$
 */

#ifndef _AARDVIEW_H
#define _AARDVIEW_H

#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QDirModel>
#include <QGestureEvent>

#include <QtGui>

#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

#include "ui_aardview.h"
#include "imageloader.h"
#include "tnviewmodel.h"
#include "alistview.h"
#include "atreeview.h"
#include "adirmodel.h"

class AardView: public QMainWindow, private Ui::AardView{
    Q_OBJECT

  public:
    AardView(QUuid uid);
    ~AardView();

    /**
     * Return the currently selected directory
     */
    QString directory();

    /**
     * Return the currently selected filename
     * @return empty string if no file is selected, otherwise a filename
     *         with path
     */
    QString filename();
    QString title();

  private:
    bool menuBarVisible;
    QUuid m_uid;

    ADirModel *dirViewModel;
    TnViewModel *tnViewModel;
    QSortFilterProxyModel *tnViewModelProxy;
    QSortFilterProxyModel *dirViewModelProxy;
    ImageLoader *loader;
#ifndef QT_NO_PRINTER
    QPrinter printer;
#endif

    void loadPixmap(const QString &filename, const QSize viewSize=QSize());

  public slots:
    void reconfigure();
    void selectNext();
    void selectPrev();
    void handleArguments();

  private slots:
    void dirIndexChanged();
    void displayPixmap(const QPixmap &pixmap);
    void thumbIndexChanged();
    void open();
    void paintToPrinter(QPrinter *printer);
    void print();
    void printPreview();
    void openEditor();
    void toggleMenuBar();
    void handlePaste();

  protected:
    bool event(QEvent *event);
    bool gestureEvent(QGestureEvent *event);
    void closeEvent(QCloseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);

  signals:
    void requestPixmap(const QString &filename, const QSize viewSize);
    void showAbout();
    void showSettings();
    void requestClose(QUuid uid);
};

#endif
