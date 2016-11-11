#include "settingsdialog.h"

SettingsDialog *SettingsDialog::settingsDialog = 0;

SettingsDialog *SettingsDialog::instance(){
  if (!settingsDialog){
    settingsDialog = new SettingsDialog();
  }

  return settingsDialog;
}

SettingsDialog::SettingsDialog(): QDialog() {
  setupUi(this);

  settings.beginGroup("main");
  mainInitialized->setChecked(!settings.value("initialized").toBool());
  mainFocusFollowsMouse->setChecked(settings.value("focusFollowsMouse").toBool());
  mainShowStatusbar->setChecked(settings.value("showStatusbar").toBool());
  mainExternalEditor->setText(settings.value("externalEditor").toString());
  mainInitialX->setValue(settings.value("initialX").toInt());
  mainInitialY->setValue(settings.value("initialY").toInt());
  mainSaveSizeOnExit->setChecked(settings.value("saveSizeOnExit").toBool());
  settings.endGroup();

  settings.beginGroup("dirview");
  dirShowOnlyDirs->setChecked(settings.value("showOnlyDirs").toBool());
  dirShowSizeCol->setChecked(settings.value("showSizeCol").toBool());
  dirShowTypeCol->setChecked(settings.value("showTypeCol").toBool());
  dirShowLastModifiedCol->setChecked(settings.value("showLastModifiedCol").toBool());
  settings.endGroup();

  settings.beginGroup("tnview");
  tnFilterFiles->setChecked(settings.value("filterFiles").toBool());
  tnFileMask->setText(settings.value("fileMask").toString());
  tnShowOnlyFiles->setChecked(settings.value("showOnlyFiles").toBool());
  tnCaseInsensitiveMatching->setChecked(settings.value("caseInsensitiveMatching").toBool());
  settings.endGroup();

  settings.beginGroup("viewer");
  viewerHideInfoArea->setChecked(settings.value("hideInfoArea").toBool());
  viewerResetFtwOnChange->setChecked(settings.value("resetFtwOnChange").toBool());
  viewerFitToWindow->setChecked(settings.value("fitToWindow").toBool());
  viewerShrinkOnly->setChecked(settings.value("shrinkOnly").toBool());
  viewerSmoothTransformation->setChecked(settings.value("smoothTransformation").toBool());
  viewerLoadAction->setCurrentIndex(settings.value("loadAction").toInt());
  settings.endGroup();
}

void SettingsDialog::accept(){
  qDebug() << "Saved settings";

  settings.beginGroup("main");
  settings.setValue("initialized", !mainInitialized->isChecked());
  settings.setValue("focusFollowsMouse", mainFocusFollowsMouse->isChecked());
  settings.setValue("showStatusbar", mainShowStatusbar->isChecked());
  settings.setValue("externalEditor", mainExternalEditor->text());
  settings.setValue("initialX", mainInitialX->value());
  settings.setValue("initialY", mainInitialY->value());
  settings.setValue("saveSizeOnExit", mainSaveSizeOnExit->isChecked());
  settings.endGroup();

  settings.beginGroup("dirview");
  settings.setValue("showOnlyDirs", dirShowOnlyDirs->isChecked());
  settings.setValue("showSizeCol", dirShowSizeCol->isChecked());
  settings.setValue("showTypeCol", dirShowTypeCol->isChecked());
  settings.setValue("showLastModifiedCol", dirShowLastModifiedCol->isChecked());
  settings.endGroup();

  settings.beginGroup("tnview");
  settings.setValue("filterFiles", tnFilterFiles->isChecked());
  settings.setValue("fileMask", tnFileMask->text());
  settings.setValue("showOnlyFiles", tnShowOnlyFiles->isChecked());
  settings.setValue("caseInsensitiveMatching", tnCaseInsensitiveMatching->isChecked());
  settings.endGroup();

  settings.beginGroup("viewer");
  settings.setValue("hideInfoArea", viewerHideInfoArea->isChecked());
  settings.setValue("resetFtwOnChange", viewerResetFtwOnChange->isChecked());
  settings.setValue("fitToWindow", viewerFitToWindow->isChecked());
  settings.setValue("shrinkOnly", viewerShrinkOnly->isChecked());
  settings.setValue("smoothTransformation", viewerSmoothTransformation->isChecked());
  settings.setValue("loadAction", viewerLoadAction->currentIndex());
  settings.endGroup();

  this->hide();
  emit configurationChanged();
}

void SettingsDialog::defaults(){
  bool initialized=settings.value("main/initialized").toBool();

  if (!initialized){
    qDebug() << "Setting initial settings...";
    settings.beginGroup("main");
    settings.setValue("focusFollowsMouse", true);
    settings.setValue("initialized", true);
    settings.setValue("externalEditor", "/usr/bin/gimp");
    settings.setValue("showStatusbar", false);
    settings.setValue("initialX", 640);
    settings.setValue("initialY", 480);
    settings.endGroup();
    settings.beginGroup("dirview");
    settings.setValue("showOnlyDirs", true);
    settings.setValue("showSizeCol", false);
    settings.setValue("showTypeCol", false);
    settings.setValue("showLastModifiedCol", false);
    settings.endGroup();
    settings.beginGroup("tnview");
    settings.setValue("showOnlyFiles", true);
    settings.setValue("caseInsensitiveMatching", true);
    settings.setValue("filterFiles", true);
    settings.setValue("fileMask", ".*(bmp|gif|ico|jpg|jpeg|mng|png|pbm|pgm|ppm|svg|tif|tiff|xbm|xpm)$");
    settings.endGroup();
    settings.beginGroup("viewer");
    settings.setValue("hideInfoArea", true);
    settings.setValue("resetFtwOnChange", true);
    settings.setValue("fitToWindow", true);
    settings.setValue("shrinkOnly", true);
    settings.setValue("padding", 5);
    settings.setValue("loadAction", 0);
    settings.endGroup();
    emit configurationChanged();
    // do something on first start
  }
}
//

QVariant SettingsDialog::value(const QString &key, const QVariant &defaultValue) const {
  return settings.value(key, defaultValue);
}
