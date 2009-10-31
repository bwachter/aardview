#include "settingsdialog.h"

SettingsDialog::SettingsDialog(): QDialog(){
  ui.setupUi(this);

  settings.beginGroup("main");
  ui.mainInitialized->setChecked(!settings.value("initialized").toBool());
  ui.mainFocusFollowsMouse->setChecked(settings.value("focusFollowsMouse").toBool());
  ui.mainShowStatusbar->setChecked(settings.value("showStatusbar").toBool());
  ui.mainExternalEditor->setText(settings.value("externalEditor").toString());
  settings.endGroup();

  settings.beginGroup("dirview");
  ui.dirShowOnlyDirs->setChecked(settings.value("showOnlyDirs").toBool());
  ui.dirShowSizeCol->setChecked(settings.value("showSizeCol").toBool());
  ui.dirShowTypeCol->setChecked(settings.value("showTypeCol").toBool());
  ui.dirShowLastModifiedCol->setChecked(settings.value("showLastModifiedCol").toBool());
  settings.endGroup();

  settings.beginGroup("tnview");
  ui.tnFilterFiles->setChecked(settings.value("filterFiles").toBool());
  ui.tnFileMask->setText(settings.value("fileMask").toString());
  ui.tnShowOnlyFiles->setChecked(settings.value("showOnlyFiles").toBool());
  ui.tnCaseInsensitiveMatching->setChecked(settings.value("caseInsensitiveMatching").toBool());
  settings.endGroup();

  settings.beginGroup("viewer");
  ui.viewerHideInfoArea->setChecked(settings.value("hideInfoArea").toBool());
  ui.viewerResetFtwOnChange->setChecked(settings.value("resetFtwOnChange").toBool());
  ui.viewerFitToWindow->setChecked(settings.value("fitToWindow").toBool());
  ui.viewerShrinkOnly->setChecked(settings.value("shrinkOnly").toBool());
  ui.viewerSmoothTransformation->setChecked(settings.value("smoothTransformation").toBool());
  ui.viewerLoadAction->setCurrentIndex(settings.value("loadAction").toInt());
  settings.endGroup();
}

void SettingsDialog::accept(){
  qDebug() << "Saved settings";

  settings.beginGroup("main");
  settings.setValue("initialized", !ui.mainInitialized->isChecked());
  settings.setValue("focusFollowsMouse", ui.mainFocusFollowsMouse->isChecked());
  settings.setValue("showStatusbar", ui.mainShowStatusbar->isChecked());
  settings.setValue("externalEditor", ui.mainExternalEditor->text());
  settings.endGroup();

  settings.beginGroup("dirview");
  settings.setValue("showOnlyDirs", ui.dirShowOnlyDirs->isChecked());
  settings.setValue("showSizeCol", ui.dirShowSizeCol->isChecked());
  settings.setValue("showTypeCol", ui.dirShowTypeCol->isChecked());
  settings.setValue("showLastModifiedCol", ui.dirShowLastModifiedCol->isChecked());
  settings.endGroup();

  settings.beginGroup("tnview");
  settings.setValue("filterFiles", ui.tnFilterFiles->isChecked());
  settings.setValue("fileMask", ui.tnFileMask->text());
  settings.setValue("showOnlyFiles", ui.tnShowOnlyFiles->isChecked());
  settings.setValue("caseInsensitiveMatching", ui.tnCaseInsensitiveMatching->isChecked());
  settings.endGroup();

  settings.beginGroup("viewer");
  settings.setValue("hideInfoArea", ui.viewerHideInfoArea->isChecked());
  settings.setValue("resetFtwOnChange", ui.viewerResetFtwOnChange->isChecked());
  settings.setValue("fitToWindow", ui.viewerFitToWindow->isChecked());
  settings.setValue("shrinkOnly", ui.viewerShrinkOnly->isChecked());
  settings.setValue("smoothTransformation", ui.viewerSmoothTransformation->isChecked());
  settings.setValue("loadAction", ui.viewerLoadAction->currentIndex());
  settings.endGroup();

  this->hide();
  emit configurationChanged();
}
