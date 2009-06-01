#include "settingsdialog.h"

SettingsDialog::SettingsDialog(): QDialog(){
  ui.setupUi(this);

  settings.beginGroup("dirview");
  ui.dirShowOnlyDirs->setChecked(settings.value("showOnlyDirs").toBool());
  settings.endGroup();

  settings.beginGroup("tnview");
  ui.tnFileMask->setText(settings.value("fileMask").toString());
  ui.tnShowOnlyFiles->setChecked(settings.value("showOnlyFiles").toBool());
  ui.tnCaseInsensitiveMatching->setChecked(settings.value("caseInsensitiveMatching").toBool());
  settings.endGroup();

  settings.beginGroup("viewer");
  ui.viewerHideInfoArea->setChecked(settings.value("hideInfoArea").toBool());
  ui.viewerResetFtwOnChange->setChecked(settings.value("resetFtwOnChange").toBool());
  ui.viewerFitToWindow->setChecked(settings.value("fitToWindow").toBool());
  settings.endGroup();
}

void SettingsDialog::accept(){
  qDebug() << "Saved settings";

  settings.beginGroup("dirview");
  settings.setValue("showOnlyDirs", ui.dirShowOnlyDirs->isChecked());
  settings.endGroup();

  settings.beginGroup("tnview");
  settings.setValue("fileMask", ui.tnFileMask->text());
  settings.setValue("showOnlyFiles", ui.tnShowOnlyFiles->isChecked());
  settings.setValue("caseInsensitiveMatching", ui.tnCaseInsensitiveMatching->isChecked());
  settings.endGroup();

  settings.beginGroup("viewer");
  settings.setValue("hideInfoArea", ui.viewerHideInfoArea->isChecked());
  settings.setValue("resetFtwOnChange", ui.viewerResetFtwOnChange->isChecked());
  settings.setValue("fitToWindow", ui.viewerFitToWindow->isChecked());
  settings.endGroup();

  this->hide();
}
