/****************************
Copyright: Zebao Zhou
File name: fcb_core.cpp
Author: Zebao Zhou

Version: 1.0
Date: 2019-12-20
Description: initial methods for properties and functions

Version: 1.1
Date: 2019-12-24
Description: add dispose() to release the memory that FCB owns including fcb and data

Version: 1.2
Date: 2019-12-27
Description: add tm2string, string2tm, pages2string and string2pages in order to easily store and load touch_time and pages in file

Version: 1.3
Date: 2019-12-28
Description: delete dispose() because bit table cannot be accessable in FCB
****************************/

#include"fcb_core.h"

/*
Summary:constructed function
*/
FCB::FCB()
{
	path = "/";
	mode = 0;
	type = 0;
	name = "";
	time_t now = time(0);
	tm *gmtm = gmtime(&now);
	touch_time = *gmtm;
	vector<int> _pages;
	pages = _pages;
	lastpage_length = 0;
	vector<FCB*> subs;
	subdir = subs;
}

FCB::FCB(string _ppath, string _name, int _type)
{
	path = _ppath;
	mode = 0;
	type = _type;
	set_name(_name);
	time_t now = time(0);
	tm *gmtm = gmtime(&now);
	touch_time = *gmtm;
	vector<int> _pages;
	pages = _pages;
	lastpage_length = 0;
	vector<FCB*> subs;
	subdir = subs;
}

/*
Summary: get the path of the file
Return: path
*/
string FCB::get_path()
{
	return path;
}

/*
Summary: get the file type
Return: the file type, 0 for file and 1 for directory
*/
int FCB::get_type()
{
	return type;
}

/*
Summary: get the authority mode of file
Return: the mode of file
*/
int FCB::get_mode()
{
	return mode;
}

/*
Summary: get the pages that file occupied
Return: the pages file occupied
*/
vector<int> FCB::get_pages()
{
	/*if (type == 1)
	{
		throw "directory has no memory allocate!";
	}*/
	return pages;
}

/*
Summary: get the used-length of the last page that file uses
Return: the used-length of the last page 
*/
long FCB::get_lplength()
{
	return lastpage_length;
}

/*
Summary: set FCB's path
Parameters:
	_path: the setting path of file
*/
void FCB::set_path(string _path)
{
	path = _path;
}

/*
Summary: set FCB's name and path properties at the meantime
Parameters:
	_name: the setting name of file
 */
void FCB::set_name(string _name)
{
	name = _name;
	if (path.length() == 0)
	{
		path = "/";
		return;
	}
	if (path[path.length() - 1] != '/')
		path = path + '/' + _name;
	else path = path + _name;
}

/*
Summary: update FCB's name and path properties at the meantime
Parameters:
	_name: the new name of file
*/
void FCB::update_name(string _name)
{
	name = _name;
	int parent_index = path.find_last_of('/');
	if (path.find_last_of('/') != -1) {
		path = path.substr(0, parent_index) + '/' + _name;
	}
}

/*
Summary: set or update the mode 
Parameters:
	_mode: the new mode
*/
void FCB::set_mode(int _mode)
{
	mode = _mode;
}

/*
Summary: set or update the pages file occupied
Parameters:
	_pages: the pages
*/
void FCB::set_pages(vector<int> _pages)
{
	pages = _pages;
}

/*
Summary: set or uplate the used-length of the last page
Parameter:
	length: the used-length
*/
void FCB::set_lplength(long length)
{
	lastpage_length = length;
}

/*
Summary: display the properties of the FCB
*/
void FCB::display()
{
	cout << "path: " << path << endl;
	cout << "mode: " << mode << endl;
	cout << "type: " << type << endl;
	cout << "touch_time: " << touch_time.tm_year + 1900 << "-" << touch_time.tm_mon + 1 << "-" << touch_time.tm_mday << endl;
	vector<FCB*> subs = subdir;
	vector<FCB*>::iterator it;
	for (it = subs.begin(); it < subs.end(); it++)
	{
		FCB f = **it;
		f.display();
	}
}

/*
Summary: change tm to string in order to easily store in file
Return: the result string of tm
*/
string FCB::tm2string()
{
	string res; 
	res = to_string(touch_time.tm_year) + "@" + to_string(touch_time.tm_mon) + "@" + to_string(touch_time.tm_mday) + "@" + to_string(touch_time.tm_hour) + "@" +
		to_string(touch_time.tm_min) + "@" + to_string(touch_time.tm_sec) + "@" + to_string(touch_time.tm_wday) + "@" + to_string(touch_time.tm_yday) + "@" + to_string(touch_time.tm_isdst);
	return res;
}

/*
Summary: split the string, used in string2pages and string2tm
*/
vector<string> FCB::split(const string& source, const string& flag) {
	vector<string> res;
	if ("" == source) return res;
	//先将要切割的字符串从string类型转换为char*类型  
	char * strs = new char[source.length() + 1]; //不要忘了  
	strcpy(strs, source.c_str());

	char * d = new char[flag.length() + 1];
	strcpy(d, flag.c_str());

	char *p = strtok(strs, d);
	while (p) {
		string s = p; //分割得到的字符串转换为string类型  
		res.push_back(s); //存入结果数组  
		p = strtok(NULL, d);
	}
	return res;
}

/*
Summary: change string to tm in order to load tm from file
Parameter:
	time: the string of time
*/
void FCB::string2tm(string& time)
{
	vector<string> ints = split(time, "@");
	tm temp;
	temp.tm_year = atoi(ints.at(0).c_str());
	temp.tm_mon = atoi(ints.at(1).c_str());
	temp.tm_mday = atoi(ints.at(2).c_str());
	temp.tm_hour = atoi(ints.at(3).c_str());
	temp.tm_min = atoi(ints.at(4).c_str());
	temp.tm_sec = atoi(ints.at(5).c_str());
	temp.tm_wday = atoi(ints.at(6).c_str());
	temp.tm_yday = atoi(ints.at(7).c_str());
	temp.tm_isdst = atoi(ints.at(8).c_str());
	touch_time = temp;
}

/*
Summary: change vector of pages to string in order to easily store in file
Return: the string result of the pages
*/
string FCB::pages2string()
{
	string res = "@";
	vector<int>::iterator it;
	for (it = pages.begin(); it < pages.end(); it++)
	{
		res = res + to_string((*it)) + "@";
	}
	return res;
}

/*
Summary: change string to vector of pages in order to load pages from file
Parameter:
	pagestr: the string of pages
*/
void FCB::string2pages(string& pagestr)
{
	vector<string> res = split(pagestr, "@");
	vector<string>::iterator it;
	for (it = res.begin(); it < res.end(); it++)
	{
		int page = atoi((*it).c_str());
		pages.push_back(page);
	}
}
