/**
 * @file aardviewlog.cpp
 * @copyright 2018 GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2018
 */

#include "aardviewlog.h"
#include <syslog.h>
#include <stdio.h>

AardviewLog *AardviewLog::avLog = 0;

AardviewLog *AardviewLog::instance(){
  if (!avLog){
    avLog = new AardviewLog();
    avLog->_logLevel = LOG_INFO;
    avLog->_consoleLogging = 0;
    avLog->_fileListIsWhitelist = false;
    avLog->_functionListIsWhitelist = false;
  }

  return avLog;
}

void AardviewLog::setConsoleLogging(const int value){
  AardviewLog *_instance = AardviewLog::instance();
  _instance->_consoleLogging = value;
}

void AardviewLog::setPriority(const int priority){
  AardviewLog *_instance = AardviewLog::instance();
  _instance->_logLevel = priority;
}

void AardviewLog::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg){
  AardviewLog *_instance = AardviewLog::instance();
  QByteArray localMsg = msg.toLocal8Bit();

  const char *file = context.file ? context.file : "";
  const char *function = context.function ? context.function : "";

  // filter based on function white/blacklisting
  if (_instance->_functionList.contains(file) &&
      _instance->_functionListIsWhitelist == false)
    return;
  if (!_instance->_functionList.contains(file) &&
      _instance->_functionListIsWhitelist == true)
    return;

  // filter based on file white/blacklisting
  if (_instance->_fileList.contains(file) &&
      _instance->_fileListIsWhitelist == false)
    return;
  if (!_instance->_fileList.contains(file) &&
      _instance->_fileListIsWhitelist == true)
    return;

  // TODO: add elaborate filter supporting matching on file _and_
  //       function _and_ range in file

  // not all of those message types may have the best possible
  // syslog type. Unused: ALERT ERR
  switch(type){
    case QtDebugMsg:
      if (_instance->_logLevel >= LOG_DEBUG){
#ifdef HAS_SYSTEMD
        sd_journal_print(LOG_DEBUG, "%s (%s:%u)",
                         localMsg.constData(),
                         file,
                         context.line
          );
        if (_instance->_consoleLogging >= 2)
#endif
          fprintf(stdout, "[DEBUG] %s (%s:%u)\n",
                  localMsg.constData(),
                  file,
                  context.line);
      }
      break;
    case QtWarningMsg:
      if (_instance->_logLevel >= LOG_WARNING){
#ifdef HAS_SYSTEMD
        sd_journal_print(LOG_WARNING, "%s",                         localMsg.constData());
        if (_instance->_consoleLogging >= 1)
#endif
          fprintf(stdout, "[WARN] %s\n", localMsg.constData());
      }
      break;
    case QtCriticalMsg:
      // QtSystemMsg is defined as QtCritical
      if (_instance->_logLevel >= LOG_CRIT){
#ifdef HAS_SYSTEMD
        sd_journal_print(LOG_CRIT, "%s", localMsg.constData());
        if (_instance->_consoleLogging >= 0)
#endif
          fprintf(stderr, "[CRIT] %s\n", localMsg.constData());
      }
      break;
    case QtFatalMsg:
      if (_instance->_logLevel >= LOG_EMERG){
#ifdef HAS_SYSTEMD
        sd_journal_print(LOG_EMERG, "%s", localMsg.constData());
        if (_instance->_consoleLogging >= 0)
#endif
          fprintf(stderr, "[FATAL] %s\n", localMsg.constData());
      }
      break;
    case QtInfoMsg:
      if (_instance->_logLevel >= LOG_INFO){
#ifdef HAS_SYSTEMD
        sd_journal_print(LOG_INFO, "%s", localMsg.constData());
        if (_instance->_consoleLogging >= 1)
#endif
          fprintf(stdout, "[INFO] %s\n", localMsg.constData());
      }
      break;
  }
}
