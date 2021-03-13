/**
 * @file aardviewlog.h
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2018
 */

#ifndef _AARDVIEWLOG_H
#define _AARDVIEWLOG_H

#include <QObject>

#ifdef HAS_SYSLOG
#include <syslog.h>
#endif

#ifdef HAS_SYSTEMD
#include <systemd/sd-journal.h>
#endif

class AardviewLog: public QObject {
    Q_OBJECT

  public:
    enum LogFilter {
      FileFilter,
      FunctionFilter
    };

    static AardviewLog *instance();
    static void setPriority(const int priority);
    static void setConsoleLogging(const int value);
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    // filter handlers
    static void installFilter(int filter);
    static void clearFilter(int filter);
    static void addToFilter(int filter);
    static void removeFromFilter(int filter);
    static void setFilterWhitelist(int filter);
    static void setFilterBlacklist(int filter);

  private:
    AardviewLog() {};
    AardviewLog(const AardviewLog&);

    static AardviewLog *avLog;
    int _logLevel;
    /*
     * -1: no console logging at all
     *  0: only critical/fatal to stderr
     *  1: info and warnings to stdout
     *  2: debug messages to stdout as well
     */
    int _consoleLogging;
    QStringList _functionList;
    QStringList _fileList;
    bool _functionListIsWhitelist, _fileListIsWhitelist;
};

#endif
