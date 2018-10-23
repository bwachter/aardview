/**
 * @file settingsdialog.h
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#ifndef _SETTINGSDIALOG_H
#define _SETTINGSDIALOG_H

#include <QtGui>
#include "ui_settingsdialog.h"

class SettingsDialog: public QDialog, private Ui::SettingsDialog {
    Q_OBJECT

  public:
    static SettingsDialog *instance();
    static QStringList features();
    static QStringList readFormats();
    static QStringList writeFormats();

  private:
    SettingsDialog();
    SettingsDialog(const SettingsDialog &);
    QSettings settings;

    static SettingsDialog *settingsDialog;
    void defaults();

  public slots:
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

  private slots:
    void accept();

  signals:
    void configurationChanged();
};


#endif
