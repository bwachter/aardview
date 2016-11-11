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
#include <QSettings>
#include <QAction>
#include <QDirModel>
#include <QGestureEvent>

#include <QtGui>

#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

#include "ui_aardview.h"
#include "imageloader.h"
#include "settingsdialog.h"
#include "tnviewmodel.h"
#include "alistview.h"
#include "atreeview.h"
#include "adirmodel.h"

class AardView: public QMainWindow, private Ui::AardView{
    Q_OBJECT

  public:
    AardView();
    ~AardView();
    QString getSelectedFilename();

  private:
    bool menuBarVisible;
    QSettings settings;

    ADirModel *dirViewModel;
    TnViewModel *tnViewModel;
    QSortFilterProxyModel *tnViewModelProxy;
    QSortFilterProxyModel *dirViewModelProxy;
    SettingsDialog *settingsDialog;
    ImageLoader *loader;
#ifndef QT_NO_PRINTER
    QPrinter printer;
#endif

    void loadPixmap(const QString &filename, const QSize viewSize=QSize());

  public slots:
    void reconfigure();
    void selectNext();
    void selectPrev();

  private slots:
    void about();
    void dirIndexChanged();
    void displayPixmap(const QPixmap &pixmap);
    void thumbIndexChanged();
    void showSettings();
    void open();
    void paintToPrinter(QPrinter *printer);
    void print();
    void printPreview();
    void openEditor();
    void toggleMenuBar();
    void handlePaste();
    void handleArguments();

  protected:
    bool event(QEvent *event);
    bool gestureEvent(QGestureEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);

  signals:
    void requestPixmap(const QString &filename, const QSize viewSize);
};

#endif
