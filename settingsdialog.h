/* settingsdialog.h	-- Bernd Wachter <bwachter@lart.info>
 *
 * $Header$
 * $Log$
 */

#ifndef _SETTINGSDIALOG_H
#define _SETTINGSDIALOG_H

#include <QtGui>
#include "ui_settingsdialog.h"

class SettingsDialog: public QDialog {
  Q_OBJECT

  public:
  SettingsDialog();

  private:
  Ui::SettingsDialog ui;
  
  public slots:
  
  signals:

};


#endif
