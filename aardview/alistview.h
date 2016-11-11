/* alistview.h  -- Bernd Wachter <bwachter@lart.info>
 *
 * $Header$
 * $Log$
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
    void enterEvent(QEvent *e){
      if (settings->value("main/focusFollowsMouse").toBool())
        this->setFocus();
      QListView::enterEvent(e);
    }
};

#endif
