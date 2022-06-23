/*
 * Str.h
 *
 *  Created on: 2018年12月7日
 *      Author: chengang
 */

#ifndef COMMON_STR_H_
#define COMMON_STR_H_
#include "common.h"
#include <string.h>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>



struct Str{
	typedef std::vector<std::string> StrVec;
	static std::list<std::string> split1(std::string &str, std::string pat) {
		std::list<std::string> bufStr;
		while (true) {
			int index = str.find(pat);
			std::string subStr = str.substr(0, index);
			if (!subStr.empty())
				bufStr.push_back(subStr);
			str.erase(0, index + pat.size());
			if (index == -1)
				break;
		}
		return bufStr;
	}
    static std::vector<std::string> split2(std::string str, std::string pattern)
	{
		std::string::size_type pos;
		std::vector<std::string> result;
		str+=pattern;//扩展字符串以方便操作
		int size = str.size();
		for(int i = 0; i < size; i++)
		{
			pos = str.find(pattern,i);
			if (pos < size) {
				std::string s = str.substr(i, pos - i);
				result.push_back(s);
				i=pos+pattern.size() - 1;
			}
		}
		return result;
	}

	static std::string splitBySize(std::string str, int size)
	{
		str.erase(0, size);
		return str;
	}

    template <class Type>
    static Type StringToNum(const string& str) {
        std::istringstream iss(str);
        Type num;
        iss >> num;
        return num;
    }

	/*删除末尾的数字 碰到字符退出*/
	static void DeleteDigit2(std::string &str) {
		int index = str.size()-1;
		while(index != -1) {
			if (str[index] >= '0' && str[index] <= '9') {
				str.erase(index, 1);
				index--;
			}  else {
				break;
			}
		}
	}

	static void Strim(string &str) {
		str.erase(str.find_last_not_of(' ') + 1, string::npos);    //去掉字符串末尾空格
		str.erase(0, str.find_first_not_of(' '));    //去掉字符串首空格
	}

	static void Strim(string &str, const char pat) {
		str.erase(str.find_last_not_of(pat) + 1, string::npos);
		str.erase(0, str.find_first_not_of(pat));
	}
	static std::string GetTimeStr(const std::string pat) {
		std::stringstream ss;
		timespec time;
		clock_gettime(CLOCK_REALTIME, &time);  //获取相对于1970年到现在的秒数
		tm nowTime;
		localtime_r(&time.tv_sec, &nowTime);
		ss << nowTime.tm_year + 1900 << pat << std::setw(2) << std::setfill('0') << nowTime.tm_mon + 1 << pat
		<< std::setw(2) << std::setfill('0') << nowTime.tm_mday << pat << std::setw(2) << std::setfill('0') << nowTime.tm_hour << pat
		<< std::setw(2) << std::setfill('0') << nowTime.tm_min << pat << std::setw(2) << std::setfill('0') << nowTime.tm_sec;
		return ss.str();
	}

	static bool CheckSuffix( const std::string& filename, const std::string& suffix )
	{
		std::size_t index = filename.find( suffix, filename.size( ) - suffix.size( ) );
		return ( index != std::string::npos );
	}

	static std::string CreateDir(std::string &rootDir, int num) {
	    std::vector<std::string>dirVec;
		std::string tmp = rootDir;
		tmp += "/";
		std::stringstream ss;
		timespec time;
		clock_gettime(CLOCK_REALTIME, &time);  //获取相对于1970年到现在的秒数
		tm nowTime;

		localtime_r(&time.tv_sec, &nowTime);
		ss << nowTime.tm_year + 1900 << "-" << nowTime.tm_mon + 1; //年月组成的目录
		tmp += ss.str();
        dirVec.push_back(tmp);
		tmp += "/";
		ss.str("");

		ss << nowTime.tm_mday; //天
		tmp += ss.str();
        dirVec.push_back(tmp);
        tmp += "/";
		ss.str("");

		ss << num;    //数字目录
		tmp += ss.str();
        dirVec.push_back(tmp);
		tmp += "/";
		ss.str("");
		for (int i = 0; i < dirVec.size(); ++i) {
			if (0 != access(dirVec[i].c_str(), 0)) {
				if (0 != mkdir(dirVec[i].c_str(), /*S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH*/0755)) {
				}
			}
		}
		return tmp;
	}
};

#endif /* COMMON_STR_H_ */
