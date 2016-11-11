/* atreeview.h  -- Bernd Wachter <bwachter@lart.info>
 *
 * $Header$
 * $Log$
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
    void enterEvent(QEvent *e){
      if (settings->value("main/focusFollowsMouse").toBool())
        this->setFocus();
      QTreeView::enterEvent(e);
    }
};


#endif
