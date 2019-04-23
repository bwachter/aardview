/**
 * @file aardviewlog.h
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2018
 */

#ifndef _AARDVIEWLOG_H
#define _AARDVIEWLOG_H

#include <QObject>

#ifdef HAS_SYSTEMD
#include <systemd/sd-journal.h>
#endif

class AardviewLog: public QObject {
    Q_OBJECT

  public:
    static AardviewLog *instance();
    static void setPriority(const int priority);
    static void setConsoleLogging(const int value);
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

  private:
    AardviewLog() {};
    AardviewLog(const AardviewLog&);

    static AardviewLog *avLog;
    int avLogLevel;
    /*
     * -1: no console logging at all
     *  0: only critical/fatal to stderr
     *  1: info and warnings to stdout
     *  2: debug messages to stdout as well
     */
    int avConsoleLogging;
};

#endif
