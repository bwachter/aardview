/**
 * @file aardview.h
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#ifndef _AARDVIEW_H
#define _AARDVIEW_H

#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QGestureEvent>
#include <QFileSystemModel>

#include <QtGui>

#include "ui_aardview.h"
#include "imageloader.h"
#include "tnviewmodel.h"
#include "exifviewmodel.h"
#include "alistview.h"
#include "atreeview.h"

class AardView: public QMainWindow, private Ui::AardView{
    Q_OBJECT

  public:
    AardView(QUuid uid, QString initialPath);
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
    QString path();
    QString loadedPath();
    QString title();

  private:
    bool menuBarVisible;
    QUuid m_uid;
    // m_loadedPath stores the path to a loaded image
    // m_path stores the path to change to
    // if m_path is an image m_loadedPath is expected to update once the
    // image is loaded
    // if m_path is a directory m_path and m_loadedPath are different until
    // next time an image is loaded (setLoadedPath updates path as well)
    //
    // as images can be loaded from both thumbnail and directory view it's
    // easier to make sure those store the latest changes than trying to
    // query all possible sources every time this info is needed
    QString m_path, m_loadedPath;

    QFileSystemModel *dirViewModel;
    ExifViewModel *exifViewModel;
    TnViewModel *tnViewModel;
    QSortFilterProxyModel *tnViewModelProxy;
    ImageLoader *loader;

    void setPath(const QString &path);
    void setLoadedPath(const QString &path);

    void loadPixmap(const QString &filename, const QSize viewSize=QSize());

  public slots:
    // if filename is empty "first display" logic gets triggered
    void load(const QString &path);
    void reconfigure();
    void selectNext();
    void selectPrev();

  private slots:
    void dirIndexChanged();
    void displayPixmap(const QPixmap &pixmap);
    void thumbIndexChanged();
    void toggleMenuBar();
    void handlePaste();

    void forwardEdit(){ emit openEditor(loader->currentFilename()); };
    void forwardOpen(){ emit showOpen(m_uid); };
    void forwardPrint(){ emit showPrint(loader->currentPixmap()); };
    void forwardPrintPreview(){ emit showPrintPreview(loader->currentPixmap()); };
    void forwardQuit(){ emit requestDestroy(m_uid, true); };

  protected:
    bool event(QEvent *event);
    bool gestureEvent(QGestureEvent *event);
    void closeEvent(QCloseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);

  signals:
    void requestPixmap(const QString &filename, const QSize viewSize);
    void openEditor(const QString &filename);
    void showAbout();
    void showSettings();
    void showOpen();
    void showOpen(QUuid uid);
    void showPrint(const QPixmap &pixmap);
    void showPrintPreview(const QPixmap &pixmap);
    void requestClose(QUuid uid);
    void requestDestroy(QUuid uid, bool force);
};

#endif
