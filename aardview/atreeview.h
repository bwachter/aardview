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

class ATreeView: public QTreeView {
  Q_OBJECT

    public:
  ATreeView(QWidget* parent=0): QTreeView(parent) {}

  private:
  QSettings settings;

  protected:
  void enterEvent(QEvent *e){
    if (settings.value("main/focusFollowsMouse").toBool())
      this->setFocus();
    QTreeView::enterEvent(e);
  }
};


#endif
