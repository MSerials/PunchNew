#pragma once
#include "Time.h"
#include "../cv_module/excv.h"
#include <mutex>
#include <iostream>
#include <string>
#include <vector>
#include<io.h>
#include <regex>


namespace MSerials {
	static void getFiles(string path, vector<string>& files, std::string _type_ = "(^[/a-zA-Z0-9-_:\\\\]*.bmp)",bool dir_it = false) {
		intptr_t   hFile = 0;
		//本身含有正则信息
		std::regex reg;
		try {
			reg = std::regex(_type_.c_str());
		}
		catch (std::regex_error e)
		{
			printf("regex error syntax");
			return;
		}
		//文件信息  
	
		struct _finddata_t fileinfo;
		string p;
		if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
		{
			do
			{
				//如果是目录,迭代之  
				//如果不是,加入列表  
				if (dir_it && (fileinfo.attrib &  _A_SUBDIR))
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
						getFiles(p.assign(path).append("\\").append(fileinfo.name), files, _type_);
				}
				else
				{
					if (std::regex_match(fileinfo.name, reg))
						files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}

	static std::vector<std::string> get_files_name(std::string path = "", std::string _type_ = "(^[/a-zA-Z0-9-_:\\\\]*.(bmp|png|jpg))" ,bool itertor_dir = false) {
		using namespace std;
		std::vector<std::string> files;
		getFiles(path, files, _type_);
		return files;
	}
};

class Tools
{
private:
	Tools() {};
public:
	static Tools* GetIns() {
		static Tools *m_tool = nullptr; if (nullptr == m_tool) { static std::mutex mtx; std::lock_guard<std::mutex> lck(mtx); m_tool = new Tools(); }
		return m_tool;
	}
	//功能，返回年月日小时分钟秒钟和毫秒
	char *Get_Time_zzz()
	{
		return _Timer::GetIns()->Get_Time_zzz();
	}
	

	static std::vector<std::string> split(std::string strtem, char a)
	{
		using namespace std;
		vector<string> strvec;

		string::size_type pos1, pos2;
		pos2 = strtem.find(a);
		pos1 = 0;
		while (string::npos != pos2)
		{
			strvec.push_back(strtem.substr(pos1, pos2 - pos1));

			pos1 = pos2 + 1;
			pos2 = strtem.find(a, pos1);
		}
		strvec.push_back(strtem.substr(pos1));
		return strvec;
	}

	//并不采用单例模式
	static excv Excv;
};