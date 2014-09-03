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

#include <QtGui>

#include "ui_aardview.h"
#include "imagewidget.h"
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

  public slots:
    void reconfigure();
    void selectNext();
    void selectPrev();

  private slots:
    void about();
    void dirIndexChanged();
    void thumbIndexChanged();
    void showSettings();
    void openEditor();
    void toggleMenuBar();
    void handlePaste();
    void handleArguments();

  protected:
    bool event(QEvent *event);
#ifdef QT46
    bool gestureEvent(QGestureEvent *event);
#endif
    void contextMenuEvent(QContextMenuEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);
};

#endif
