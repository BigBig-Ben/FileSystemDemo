/****************************
Copyright: Zebao Zhou
File name: fcb_core.h
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
****************************/

#include<iostream>
#include<string>
#include<vector>
#include<ctime>

using namespace std;

class FCB
{
private:
	string path;				//file path
	int type;					//file type, 0 for file and 1 for directory
	int mode;					//authority mode  -RWX
	vector<int> pages;			//the pages that data occupied
	long lastpage_length;		//the used-length of the last page the file uses

public:
	string name;				//file name
	tm touch_time;				//the latest modefied time
	vector<FCB*> subdir;			//sub directory

	FCB();
	FCB(string _ppath, string _name, int _type = 0);
	~FCB() {};
	string get_path();
	int get_type();
	int get_mode();
	vector<int> get_pages();
	long get_lplength();

	void set_path(string _path);
	void set_name(string _name);
	void update_name(string _name);
	void set_mode(int new_mode);
	void set_pages(vector<int> _pages);
	void set_lplength(long length);
	void display();
	vector<string> split(const string& source, const string& flag);
	string tm2string();
	void string2tm(string& time);
	string pages2string();
	void string2pages(string& pagestr);
};