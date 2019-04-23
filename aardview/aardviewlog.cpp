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
    avLog->avLogLevel = LOG_INFO;
    avLog->avConsoleLogging = 0;
  }

  return avLog;
}

void AardviewLog::setConsoleLogging(const int value){
  AardviewLog *_instance = AardviewLog::instance();

  _instance->avConsoleLogging = value;
}

void AardviewLog::setPriority(const int priority){
  AardviewLog *_instance = AardviewLog::instance();

  _instance->avLogLevel = priority;
}

void AardviewLog::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg){
  AardviewLog *_instance = AardviewLog::instance();
  QByteArray localMsg = msg.toLocal8Bit();

  const char *file = context.file ? context.file : "";
  const char *function = context.function ? context.function : "";

  /* TODO:
   * this logging mechanism should support two kinds of filters:
   * - by priority
   * - by function or file location
   *
   * The latter is available from context.file and context.function.
   * context.line for the exact line number is also available, but
   * probably not very interesting.
   */

  // not all of those message types may have the best possible
  // syslog type. Unused: ALERT ERR
  switch(type){
    case QtDebugMsg:
      if (_instance->avLogLevel >= LOG_DEBUG){
#ifdef HAS_SYSTEMD
        sd_journal_print(LOG_DEBUG, "%s (%s:%u)",
                         localMsg.constData(),
                         file,
                         context.line
          );
        if (_instance->avConsoleLogging >= 2)
#endif
          fprintf(stdout, "[DEBUG] %s (%s:%u)\n",
                  localMsg.constData(),
                  file,
                  context.line);
      }
      break;
    case QtWarningMsg:
      if (_instance->avLogLevel >= LOG_WARNING){
#ifdef HAS_SYSTEMD
        sd_journal_print(LOG_WARNING, "%s",                         localMsg.constData());
        if (_instance->avConsoleLogging >= 1)
#endif
          fprintf(stdout, "[WARN] %s\n", localMsg.constData());
      }
      break;
    case QtCriticalMsg:
      // QtSystemMsg is defined as QtCritical
      if (_instance->avLogLevel >= LOG_CRIT){
#ifdef HAS_SYSTEMD
        sd_journal_print(LOG_CRIT, "%s", localMsg.constData());
        if (_instance->avConsoleLogging >= 0)
#endif
          fprintf(stderr, "[CRIT] %s\n", localMsg.constData());
      }
      break;
    case QtFatalMsg:
      if (_instance->avLogLevel >= LOG_EMERG){
#ifdef HAS_SYSTEMD
        sd_journal_print(LOG_EMERG, "%s", localMsg.constData());
        if (_instance->avConsoleLogging >= 0)
#endif
          fprintf(stderr, "[FATAL] %s\n", localMsg.constData());
      }
      break;
    case QtInfoMsg:
      if (_instance->avLogLevel >= LOG_INFO){
#ifdef HAS_SYSTEMD
        sd_journal_print(LOG_INFO, "%s", localMsg.constData());
        if (_instance->avConsoleLogging >= 1)
#endif
          fprintf(stdout, "[INFO] %s\n", localMsg.constData());
      }
      break;
  }
}
