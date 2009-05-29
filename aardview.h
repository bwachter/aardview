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

class AardView: public QMainWindow {
  Q_OBJECT

  public:
  AardView();
  
  private:
  QSettings settings;
  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *helpMenu;
  QMenu *viewMenu;
  QAction *aboutAct;
  QAction *aboutQtAct;
  QAction *exitAct;
  QAction *restoreAct;
  QAction *minimizeAct;
  QAction *maximizeAct;
  void createActions();
  void createMenus();
  void createDocks();

  QDirModel *model;
  QTreeView *tree;
  QListView *list;
  ImageWidget *widget;

  private slots:
  void dirIndexChanged();
};

#endif
