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
  QTreeView *dirView;
  QListView *tnView;
  QListView *tagView;
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
