/**
 * @file aardviewshim.h
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#ifndef _AARDVIEWSHIM_H
#define _AARDVIEWSHIM_H

#include <QObject>
#include <QSystemTrayIcon>

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
    QSystemTrayIcon *m_trayIcon;

    /// @todo hack, make it configurable
    bool useTray=true;
    SettingsDialog *m_settingsDialog;
#ifndef QT_NO_PRINTER
    QPrinter m_printer;
#endif

    /**
     * Initialize and create a system tray icon, if supported by the platform.
     */
    void createTrayIcon();

  private slots:
    /**
     * Display about box with build specific information
     */
    void about();
    /**
     * This method creates and shows a new viewer main window, opening
     * either a directory or a file, depending on options passed.
     *
     * If a window for this file or directory already exists it may be
     * reused, depending on viewer configuration.
     *
     * @param argumentList a list of directories or filenames to open
     */
    void addWindow(const QStringList &argumentList=QStringList());
    /**
     * @overload
     *
     * @param argument a single file or directory to open
     */
    void addWindow(const QString &argument);
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
    /**
     * Toggle visibility of a window.
     *
     * @param index the index of the window in a view
     */
    void toggleWindow(const QModelIndex &index);

    /**
     * Open an external editor for image editing.
     *
     * @param filename the filename to edit
     */
    void edit(const QString &filename);
    /**
     * Open a new picture. When called with a valid window uid the given
     * window is reused, otherwise the picture is loaded in a new window.
     *
     * @param uid the window uid to reuse, if any
     */
    void open(QUuid uid=QUuid());
    /**
     * Paint pixmap data to a specific printer
     * Note: printing functionality is currently disabled.
     */
    void paintToPrinter(QPrinter *printer);
    /**
     * Open a print dialogue
     * Note: printing functionality is currently disabled.
     */
    void print();
    /**
     * Open a print preview
     * Note: printing functionality is currently disabled.
     */
    void printPreview();
};

#endif
