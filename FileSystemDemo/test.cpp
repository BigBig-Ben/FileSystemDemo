#include<iostream>
#include<iomanip>
#include"file_pipe.h"
using namespace std;

FCB *root = new FCB();
FCB *current;
short bit_table[1024] = { 0 };
//path split
vector<string> split(const string& str, const string& delim) {
	vector<string> res;
	if ("" == str) return res;
	//先将要切割的字符串从string类型转换为char*类型  
	char * strs = new char[str.length() + 1]; //不要忘了  
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p) {
		string s = p; //分割得到的字符串转换为string类型  
		res.push_back(s); //存入结果数组  
		p = strtok(NULL, d);
	}
	return res;
}

//界面初始化及载入FCBs
void init()
{
	cout << "***WELCOME TO USE FILE SYSTEM DEMO***" << endl;
	cout << endl;
}

//测试用假树
void test()
{
	//当前时间
	time_t now = time(0);
	tm *gmtm = gmtime(&now);
	
	vector<int> pages = vector<int>();
	pages.push_back(1);
	pages.push_back(2);
	pages.push_back(3);

	FCB *fcb1 = new FCB();
	fcb1->set_name("");
	fcb1->set_mode(7);
	fcb1->type = 1;

	FCB *fcb2 = new FCB(fcb1->get_path(), "usr", 1);
	fcb2->set_mode(5);

	FCB *fcb3 = new FCB(fcb1->get_path(), "sys", 1);
	fcb3->set_mode(4);

	FCB *fcb4 = new FCB(fcb2->get_path(), "bigben", 1);
	fcb4->set_mode(7);

	FCB *fcb5 = new FCB(fcb4->get_path(), "test.txt", 0);
	fcb5->set_mode(7);
	fcb5->set_pages(pages);
	fcb5->set_lplength(101);

	FCB *fcb6 = new FCB(fcb1->get_path(), "bin", 1);
	FCB *fcb7 = new FCB(fcb2->get_path(), "root", 1);

	fcb1->subdir.push_back(fcb3);
	fcb1->subdir.push_back(fcb2);
	fcb1->subdir.push_back(fcb6);
	fcb2->subdir.push_back(fcb4);
	fcb2->subdir.push_back(fcb7);
	fcb4->subdir.push_back(fcb5);

	root = fcb1;
	//root->display();
	//current = fcb3;	// /sys
	//current = fcb2;	// /usr
	//current = fcb1;	// /
	current = fcb4;		// /usr/bigben
}

//finished
void test_cd_absolute(string target)
{
	FCB* pre_path = current;			//record the working path in order to go back if cd fails
	vector<string> paths = split(target, "/");
	vector<string>::iterator it;
	current = root;
	for (it = paths.begin(); it < paths.end(); it++)
	{
		bool found = false;
		string target_sub = *it;
		vector<FCB*> subdir = current->subdir;
		vector<FCB*>::iterator fit;
		for (fit = subdir.begin(); fit < subdir.end(); fit++)
		{
			FCB* temp = *fit;
			if (temp->name == target_sub)
			{
				current = temp;
				found = true;
				break;
			}
		}
		if (!found)
		{
			cout << "Error: There is no " << target << endl;
			current = pre_path;		//go back to previous path
		}
	}
	//cout << current->get_path() << endl;
}

//finished
void test_cd_relative(string target)
{
	FCB* pre_path = current;			//record the working path in order to go back if cd fails
	vector<string> paths = split(target, "/");
	vector<string>::iterator it;
	for (it = paths.begin(); it < paths.end(); it++)
	{
		bool found = false;
		string target_sub = *it;
		vector<FCB*> subdir = current->subdir;
		vector<FCB*>::iterator fit;
		for (fit = subdir.begin(); fit < subdir.end(); fit++)
		{
			FCB* temp = *fit;
			if (temp->name == target_sub)
			{
				current = temp;
				found = true;
				break;
			}
		}
		if (!found)
		{
			cout << "Error: There is no " << target << endl;
			current = pre_path;		//go back to previous path
		}
	}
	//cout << current->get_path() << endl;
}

//finished
void test_cd_backward()
{
	string currentpath = current->get_path();
	int index = currentpath.find_last_of("/");
	string parent = currentpath.substr(0, index);
	test_cd_absolute(parent);
}

//finished
void test_ls_simple()
{
	vector<FCB*> subdir = current->subdir;
	vector<FCB*>::iterator it;
	for (it = subdir.begin(); it < subdir.end(); it++)
	{
		cout << (*it)->name << "      ";
	}
	cout << endl;
}

//finished
void test_ls_specific()
{
	vector<FCB*> subdir = current->subdir;
	vector<FCB*>::iterator it;
	cout << left << setw(15) << "name" << left << setw(20) << "modified time";
	cout << left << setw(12) << "type" << "size" << endl;
	for (it = subdir.begin(); it < subdir.end(); it++)
	{
		FCB fcb = **it;
		tm time = fcb.touch_time;
		string _touch_time = to_string(time.tm_year + 1900) + "/" + to_string(time.tm_mon) + "/" + to_string(time.tm_mday) + " " 
				+ to_string(time.tm_hour) + ":" + to_string(time.tm_min);
		cout << left << setw(15) << fcb.name << left << setw(20) << _touch_time;
		if (fcb.type == 1)
			cout << left << setw(12) << "folder";
		else
		{
			int index = fcb.name.find('.');
			string type_str = fcb.name.substr(index + 1) + " file";
			cout << left << setw(12) << type_str;
			//size
			cout << (fcb.get_pages().size() - 1) * 1024 + fcb.get_lplength();
		}
		cout << endl;
	}
	cout << endl;
}

//finished
void test_printcurrentpath()
{
	cout << current->get_path() << endl;
}

//finished
void test_mkdir(string dir_name)
{
	FCB *fcb = new FCB(current->get_path(), dir_name, 1);
	fcb->set_mode(7);	//??
	current->subdir.push_back(fcb);
}

//finished
void test_rmdir(string dir_name)
{
	vector<FCB*> subdir = current->subdir;
	vector<FCB*>::iterator it;
	bool found = false;		//是否找到
	for (it = subdir.begin(); it < subdir.end(); it++)
	{
		if ((*it)->name == dir_name)
		{
			found = true;
			if ((*it)->subdir.size() != 0)
			{
				cout << "Warning: " << dir_name << " is not an empty directory. If you still want to remove it, try to do it with -f " << endl;
			}
			else
			{
				//(*it)->dispose();		//删除fcb和data
				//从subdir中删除dir_name
				it = subdir.erase(it);
				break;
			}
		}
	}
	if (!found)
	{
		cout << "Error: There is no " << dir_name << " directory" << endl;
		return;
	}
	current->subdir = subdir;
}

//finished
void test_rmdir_force(string dir_name)
{
	vector<FCB*> subdir = current->subdir;
	vector<FCB*>::iterator it;
	bool found = false;		//是否找到
	for (it = subdir.begin(); it < subdir.end(); it++)
	{
		if ((*it)->name == dir_name)
		{
			found = true;
			//(*it)->dispose();		//删除fcb和data
			//从subdir中删除dir_name
			it = subdir.erase(it);
			break;
		}
	}
	if (!found)
	{
		cout << "Error: There is no " << dir_name << " directory" << endl;
		return;
	}
	current->subdir = subdir;
}

//？？？？？？？
void test_rndir(string dir_name, string new_name)
{
	vector<FCB*> subdir = current->subdir;
	vector<FCB*>::iterator it;
	bool found = false;		//是否找到
	for (it = subdir.begin(); it < subdir.end(); it++)
	{
		if ((*it)->name == dir_name)
		{
			found = true;
			/*
			* 如何更改子目录和文件路径？
			*/

		}
	}
	if (!found)
	{
		cout << "Error: There is no " << dir_name << " directory" << endl;
		return;
	}
	current->subdir = subdir;
}

//finished
void data_save_test()
{
	DateBlock block[1024];
	for (int i = 0; i < 1024; i++)
	{
		block[i].block_id = i;
		strcpy(block[i].data, "");
	}
	strcpy(block[0].data, "hello world!");
	strcpy(block[1].data, "how are you?");
	strcpy(block[2].data, "I am fine, thank you!");
	bit_table[0] = 1;
	bit_table[1] = 1;
	bit_table[2] = 1;
	//write
	DataPipe *data_pipe = new DataPipe();
	data_pipe->open(true);
	data_pipe->write_bittable(bit_table);
	for (int i = 0; i < 1024; i++)
	{
		data_pipe->write(&block[i], i);
	}
	data_pipe->close();
	
	//read
	/*bit_table[2] = 1;
	DataPipe *data_pipe = new DataPipe();
	data_pipe->open(true);
	data_pipe->read_bittable(bit_table);
	for (int i = 0; i < 1024; i++)
		cout << bit_table[i] << " ";
	cout << endl;
	for (int i = 0; i < 3; i++)
	{
		data_pipe->read(&block[i],i);
		cout << block[i].block_id << " + " << block[i].data << endl;
	}
	data_pipe->close();*/
}

//finished
void fcb_load_test()
{
	FCBPipe *pipe = new FCBPipe();
	FCB* test = pipe->load();
	test->display();
}

//finished
void fcb_save_test()
{
	vector<int> pages = vector<int>();
	pages.push_back(0);
	pages.push_back(1);
	pages.push_back(2);

	FCB *fcb1 = new FCB();
	fcb1->set_name("");
	fcb1->set_mode(7);
	fcb1->type = 1;

	FCB *fcb2 = new FCB(fcb1->get_path(), "usr", 1);
	fcb2->set_mode(5);

	FCB *fcb3 = new FCB(fcb1->get_path(), "sys", 1);
	fcb3->set_mode(4);

	FCB *fcb4 = new FCB(fcb2->get_path(), "bigben", 1);
	fcb4->set_mode(7);

	FCB *fcb5 = new FCB(fcb4->get_path(), "test.txt", 0);
	fcb5->set_mode(7);
	fcb5->set_pages(pages);
	fcb5->set_lplength(101);

	FCB *fcb6 = new FCB(fcb1->get_path(), "bin", 1);
	FCB *fcb7 = new FCB(fcb2->get_path(), "root", 1);

	fcb1->subdir.push_back(fcb3);
	fcb1->subdir.push_back(fcb2);
	fcb1->subdir.push_back(fcb6);
	fcb2->subdir.push_back(fcb4);
	fcb2->subdir.push_back(fcb7);
	fcb4->subdir.push_back(fcb5);

	FCBPipe *pipe = new FCBPipe();
	pipe->store(fcb1);

	delete pipe;
	delete fcb1, fcb2, fcb3, fcb4, fcb5, fcb6, fcb7;
}

int main() 
{
	//init();
	//test();
	//root->display();
	//test_cd_absolute("/usr/bigben");
	//test_cd_relative("bigben");
	//test_ls_simple();
	//test_ls_specific();
	//test_printcurrentpath();
	/*test_mkdir("folder");
	test_cd_relative("folder");
	test_mkdir("hahh");
	test_cd_backward();
	test_rmdir("folder");
	root->display();
	test_rmdir_force("folder");*/
	//root->display();
	//test_ls_simple();
	data_save_test();
	fcb_save_test();
	//fcb_load_test();
	return 0;
}