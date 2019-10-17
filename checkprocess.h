#ifndef CHECKPROCESS_H
#define CHECKPROCESS_H

#include <QProcess>

class CheckProcess
{
public:
    CheckProcess();
    bool checkProcessRunning(const QString &processName, QList<quint64> &listProcessId);
};

#endif // CHECKPROCESS_H
