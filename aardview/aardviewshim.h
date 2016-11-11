/**
 * @file aardviewshim.h
 * @copyright 2016
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2016
 */

#ifndef _AARDVIEWSHIM_H
#define _AARDVIEWSHIM_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QWidgetAction>

#include "aardview.h"
#include "windowmodel.h"

class AardviewShim: public QObject {
    Q_OBJECT

  public:
    AardviewShim();

  private:
    WindowModel *m_windowModel;
    QListView *m_windowListWidget;
    QWidgetAction *trayMenuWidget;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *actionExit, *actionNewWindow;
    AardView *mw;
    bool useTray=false;

    void createTrayIcon();

  private slots:
    void about();
    void addWindow();
    void deleteWindow(QUuid uid);
    void toggleWindow(const QModelIndex &index);
};

#endif
