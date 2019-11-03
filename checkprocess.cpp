#include "checkprocess.h"
#include <windows.h>
#include <Process.h>
#include "tlhelp32.h"

bool CheckProcess::checkProcessRunning(const QString &processName, QList<quint64> &listProcessId)
{
#ifdef Q_OS_WIN
    bool res = false;
    HANDLE    hToolHelp32Snapshot = nullptr;
    hToolHelp32Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32    pe = { sizeof(PROCESSENTRY32) };
    BOOL  isSuccess = Process32First(hToolHelp32Snapshot, &pe);
    while (isSuccess)
    {
        size_t len = WideCharToMultiByte(CP_ACP, 0, pe.szExeFile, static_cast<int>(wcslen(pe.szExeFile)), NULL, 0, NULL, NULL);
        char *des = (char *)malloc(sizeof(char) * (len + 1));
        WideCharToMultiByte(CP_ACP, 0, pe.szExeFile, static_cast<int>(wcslen(pe.szExeFile)), des,static_cast<int>(len), NULL, NULL);
        des[len] = '\0';
        if (!strcmp(des, processName.toStdString().c_str()))
        {
            listProcessId.append(pe.th32ProcessID);
            res = true;
            break;
        }
        free(des);
        isSuccess = Process32Next(hToolHelp32Snapshot, &pe);
    }
    CloseHandle(hToolHelp32Snapshot);
    return res;
#elif defined Q_OS_MAC
    bool res(false);
    QString strCommand = "ps -ef|grep " + processName + " |grep -v grep |awk '{print $2}'";

    const char* strFind_ComName = convertQString2char(strCommand);
    FILE * pPipe = popen(strFind_ComName, "r");
    if (pPipe)
    {
        std::string com;
        char name[512] = { 0 };
        while (fgets(name, sizeof(name), pPipe) != NULL)
        {
            int nLen = strlen(name);
            if (nLen > 0
                && name[nLen - 1] == '\n')
                //&& name[0] == '/')
            {
                name[nLen - 1] = '\0';
                listProcessId.append(atoi(name));
                res = true;
                break;
            }
        }
        pclose(pPipe);
    }
    return res;
#endif
}

CheckProcess::CheckProcess()
{

}
