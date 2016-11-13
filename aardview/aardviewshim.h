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

#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

#include "aardview.h"
#include "settingsdialog.h"
#include "windowmodel.h"

class AardviewShim: public QObject {
    Q_OBJECT

  public:
    AardviewShim(const QStringList &arguments);

  public slots:
    void receivedMessage(int instanceId, QByteArray message);

  private:
    WindowModel *m_windowModel;
    QListView *m_windowListWidget;
    QWidgetAction *trayMenuWidget;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    bool useTray=true;
    SettingsDialog *m_settingsDialog;
#ifndef QT_NO_PRINTER
    QPrinter printer;
#endif

    void createTrayIcon();

  private slots:
    void about();

    void addWindow(const QString &argument);
    void addWindow(const QStringList &argumentList=QStringList());
    /**
     * Request removal of window with given uid
     *
     * If force is false the window may just be hidden, depending on variables
     * like systray availability.
     *
     * If force is true the window will always be destroyed. In some cases
     * closing a window may trigger an application exit.
     */
    void deleteWindow(QUuid uid, bool force=false);
    void toggleWindow(const QModelIndex &index);

    void edit(const QString &filename);
    void open(QUuid uid=QUuid());
    void paintToPrinter(QPrinter *printer);
    void print();
    void printPreview();
};

#endif
