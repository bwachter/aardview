/**
 * @file atreeview.h
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#ifndef _ATREEVIEW_H
#define _ATREEVIEW_H

#include <QtGui>
#include <QWidget>
#include <QTreeView>
#include "settingsdialog.h"

class ATreeView: public QTreeView {
    Q_OBJECT

  public:
    ATreeView(QWidget* parent=0): QTreeView(parent) {}

  private:
    SettingsDialog *settings = SettingsDialog::instance();

  protected:
    void enterEvent(QEnterEvent *e){
      if (settings->value("main/focusFollowsMouse").toBool())
        this->setFocus();
      QTreeView::enterEvent(e);
    }
};


#endif
