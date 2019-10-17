#pragma once
#include <iostream>
//Need Rename MTime.h
#ifdef QT_VERSION
#include <QDateTime>
#endif
#ifndef __TIMER__
#define __TIMER__
//_Extend
class _Timer
{
private:
	_Timer() {}
public:
	static _Timer* GetIns() { static _Timer _T; return &_T; }


	//功能 阻塞n毫秒
	static bool _delay(int &val, DWORD n)
	{
            clock_t clk = clock();
            while (val) {
                if((clock()-clk)>n)
                    return false;
            }
            return true;
	}
	//accurcy to zzz
        const char * Get_Time_zzz()
	{
#ifdef QT_VERSION
                return QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz").toLocal8Bit().toStdString().c_str();
		std::cout << "qt" << std::endl;
#else
	#ifdef __AFX_H__ 
			SYSTEMTIME st;
			CString strTime;
			GetLocalTime(&st);
			strTime.Format(L"%4d-%d-%d-%d-%d-%d-%d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			return (char*)(LPCSTR)(CStringA)strTime;
	#else
		#error unknown library
	#endif
#endif
	}

};

#endif
