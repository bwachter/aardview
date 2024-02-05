/**
 * @file alistview.h
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#ifndef _ALISTVIEW_H
#define _ALISTVIEW_H

#include <QtGui>
#include <QWidget>
#include <QListView>
#include "settingsdialog.h"

class AListView: public QListView {
    Q_OBJECT

  public:
    AListView(QWidget* parent=0): QListView(parent) {}

  private:
    SettingsDialog *settings = SettingsDialog::instance();

  protected:
    void enterEvent(QEnterEvent *e){
      if (settings->value("main/focusFollowsMouse").toBool())
        this->setFocus();
      QListView::enterEvent(e);
    }
};

#endif
