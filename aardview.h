/* aardview.h	-- Bernd Wachter <bwachter@lart.info>
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

#include <QtGui>
#include "imagewidget.h"
#include "settingsdialog.h"
#include "tnviewmodel.h"

class AListView: public QListView {
  Q_OBJECT
    
    public:
  AListView(QWidget* parent=0): QListView(parent) {}

  private:
  QSettings settings;

  protected:
  void enterEvent(QEvent *e){
    if (settings.value("main/focusFollowsMouse").toBool())
      this->setFocus();
    QListView::enterEvent(e);
  }
};

class ATreeView: public QTreeView {
  Q_OBJECT
    
    public:
  ATreeView(QWidget* parent=0): QTreeView(parent) {}

  private:
  QSettings settings;

  protected:
  void enterEvent(QEvent *e){
    if (settings.value("main/focusFollowsMouse").toBool())
      this->setFocus();
    QTreeView::enterEvent(e);
  }
};

class AardView: public QMainWindow {
  Q_OBJECT

  public:
  AardView();
  QString getSelectedFilename();
  
  private:
  bool menuBarVisible;
  QSettings settings;
  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *helpMenu;
  QMenu *viewMenu;
  QAction *aboutAct;
  QAction *aboutQtAct;
  QAction *exitAct;
  QAction *editAct;
  QAction *menuAct;
  QAction *settingsAct;
  QAction *restoreAct;
  QAction *minimizeAct;
  QAction *maximizeAct;
  void createActions();
  void createMenus();
  void createDocks();

  QDirModel *dirViewModel;
  TnViewModel *tnViewModel;
  QSortFilterProxyModel *tnViewModelProxy;
  QSortFilterProxyModel *dirViewModelProxy;
  ATreeView *dirView;
  AListView *tnView;
  AListView *tagView;
  ImageWidget *widget;
  SettingsDialog *settingsDialog;

  private slots:
  void about();
  void dirIndexChanged();
  void thumbIndexChanged();
  void showSettings();
  void openEditor();
  void toggleMenuBar();
};

#endif
