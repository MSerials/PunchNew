#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace tl
{
	class exstring
	{		
	public:
		exstring() {};
		~exstring() {};
		static std::vector<std::string> split(std::string  str,std::string delim)
		{
			std::string::size_type pos;
			std::vector<std::string> result;
			str += delim;//扩展字符串以方便操作  
			int size = static_cast<int>(str.size());
			for (int i = 0; i<str.size(); i++)
			{
				pos = str.find(delim, i);
				if (pos<size)
				{
					std::string s = str.substr(i, pos - i);
					result.push_back(s);
					i = static_cast<int>(pos) + static_cast<int>(delim.size()) - 1;
				}
			}
			return result;
		}
	};
};