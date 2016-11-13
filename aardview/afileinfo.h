/**
 * @file afileinfo.h
 * @copyright 2016 GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2016
 */

#ifndef _AFILEINFO_H
#define _AFILEINFO_H

#include <QFileInfo>
#include <QDir>
#include <QDebug>

class AFileInfo: public QFileInfo {
  public:
    AFileInfo(): QFileInfo(){}
    AFileInfo(const QString &file): QFileInfo(file){
      // QFileInfo::absolutePath() treats pathnames as files when not ending
      // in /, even though QFileInfo is aware about a path being a directory
      if (isDir() && file.at(file.size()-1) != '/')
        QFileInfo::setFile(file + "/");
    }

    QString friendlyFilePath() const{
      QString path = absoluteFilePath();
      QDir dir;

      if (path.startsWith(dir.homePath())){
        path.replace(0, dir.homePath().size(), "~");
      }

      return path;
    }
};

#endif
