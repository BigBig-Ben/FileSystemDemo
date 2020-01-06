/****************************
Copyright: Zebao Zhou
File name: main.cpp
Author: Zebao Zhou

Version: 1.0
Date: 2019-12-27
Description: the main file of file system Demo
			 achieve the graphic of the system and the I/O operations
			 realize the logical of the system

Version: 1.1
Date: 2019-12-29
Description: change dispose from inner FCB to maincpp and improve the authority mode with super
****************************/
#include<iostream>
#include<string>
#include<iomanip>
#include"file_pipe.h"
using namespace std;

FCB *root = new FCB();	//the root directory
FCB *current;			//the working directory
short bit_table[1024];	//bit_table to record pages usage
vector<string> history;	//the record of the right commands
string role;			//the operate role of system

/*
Summary: split path into strings to be used in cd
Parameters:
	str: the whole string
	delim: the separate flag
Return: the result of split str
*/
vector<string> split(const string& str, const string& delim) {
	vector<string> res;
	if ("" == str) return res;
	//convert str from string to char* first 
	char * strs = new char[str.length() + 1];  
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p) {
		//convert splited-result from char* to string, and then store them in vector
		string s = p;
		res.push_back(s);
		p = strtok(NULL, d);
	}
	return res;
}

/*
Summary: init the graphic and role, load the fcb-tree and bit table
*/
void init()
{
	cout << "*** WELCOME TO USE FILE SYSTEM DEMO ***" << endl;
	FCBPipe *fcbpipe = new FCBPipe();
	DataPipe *datapipe = new DataPipe();
	//load fcb-tree
	root = fcbpipe->load();
	current = root;
	role = "user";
	//load bit table
	datapipe->open();
	datapipe->read_bittable(bit_table);
	delete fcbpipe, datapipe;
}

/*
Summary: quit the system ,store the fcb-tree & bit table
*/
void exit()
{
	cout << "*** THANK YOU FOR USING FILE SYSTEM DEMO ***" << endl;
	FCBPipe *fcbpipe = new FCBPipe();
	DataPipe *datapipe = new DataPipe();
	//store the fcb-tree
	fcbpipe->store(root);
	datapipe->open(true);
	//store the bit table
	datapipe->write_bittable(bit_table);
	datapipe->close();
	delete fcbpipe, datapipe;
	cout << "*** DATA STORE COMPLETE ***" << endl;
}

/*
Summary: to get the index of the first unused page
Return:
*/
int find_first_emptypage()
{
	int i;
	for (i = 0; i < 1024; i++)
	{
		if (bit_table[i] == 0)
			break;
	}
	return i;
}

/*
Summary: to get the count of available pages
Return: the count of available pages
*/
int get_available_count()
{
	int cnt = 0;
	for (int i = 0; i < 1024; i++) {
		if (bit_table[i] == 0)
			cnt++;
	}
	return cnt;
}

/*
Summary: show disk usage condition
*/
void show_disk()
{
	for (int i = 0; i < 1024; i++)
		cout << bit_table[i];
	cout << endl;
	int count = get_available_count();
	cout << "There are " << count << " data blocks available  ( " << count / 4 << "KB )" << endl;
}

/*
Summary: read the data of the pages and output them on screen
Parameters:
	pages: the pages need to be shown
*/
void open_data(vector<int> pages)
{
	vector<int>::iterator pit;
	DataPipe *pipe = new DataPipe();
	pipe->open();
	for (pit = pages.begin(); pit < pages.end(); pit++) {
		int page = *pit;
		DataBlock *block = new DataBlock();
		if (pipe->read(block, page))
			cout << block->data;
		delete block;
	}
	cout << endl;
	pipe->close();
	delete pipe;
}

/*
Summary: show a inputable panel, save the input string into data file and return the pages it occupied
Parameter:
	lplength: last page length
Return: pages the new input content occupied
*/
vector<int> new_data(int &lplength)
{
	string str, input;
	vector<int> pages;
	DataPipe *pipe = new DataPipe();
	pipe->open(true);
	//input the string, use ctrl+z to quit and save
	cout << "use ctrl+z to quit and save" << endl;
	while (getline(cin, input)) {
		str = str + input + "\n";
	}
	//reset cin to make cin can be used again
	cin.clear();
	int LENGTH = 254;
	do {
		//cut the input string into pieces that can be writen in data-block
		string data;
		if (str.length() > LENGTH) {
			data = str.substr(0, LENGTH);
			str = str.substr(LENGTH);
		}
		else {
			lplength = str.length();
			data = str.substr(0, str.length());
			str = str.substr(str.length());
		}
		//apply for a data-block to store the piece data
		int page_id = find_first_emptypage();
		DataBlock *block = new DataBlock();
		block->block_id = page_id;
		strcpy(block->data, data.c_str());
		pipe->write(block, page_id);
		bit_table[page_id] = 1;
		pages.push_back(page_id);
		delete block;
	} while (str.length() > LENGTH);
	pipe->close();
	delete pipe;
	return pages;
}

/*
Summary: get the help document (all or one)
Parameters:
	op: the operation that needs to be query, default 'all' to get all operations' help
*/
void help(string op = "all")
{
	cout << "HELP" << endl;
	cout << left << setw(10) << "Command" << left << setw(20) << "Parameter" << "Description" << endl;
	if(op == "all")
	{
		cout << left << setw(10) << "cd" << left << setw(20) << "target|.. " << "change working directory to 'target'(absolutely and relatively)|go to upper directory" << endl;
		cout << left << setw(10) << "pwd" << left << setw(20) << " " << "print working directory" << endl;
		cout << left << setw(10) << "help" << left << setw(20) << " " << "show the help document" << endl;
		cout << left << setw(10) << "clear" << left << setw(20) << " " << "clear the screen" << endl;
		cout << left << setw(10) << "history" << left << setw(20) << " " << "show the command history" << endl;
		cout << left << setw(10) << "exit" << left << setw(20) << " " << "exit the file system" << endl;
		cout << left << setw(10) << "mkdir" << left << setw(20) << "dir_name" << "make an new directory named 'dir_name' at working directory" << endl;
		cout << left << setw(10) << "rmdir" << left << setw(20) << "[-f] dir_name" << "remove the directory named 'dir_name' which should be empty. Do mvdir with -f to remove 'dir_name' no matter it is empty or not" << endl;
		cout << left << setw(10) << "rndir" << left << setw(20) << "old_name new_name" << "rename the directory named 'old_name' to 'new_name'" << endl;
		cout << left << setw(10) << "ls" << left << setw(20) << "[-l]" << "list the files and directoris at working directory. Do ls with -l to list the specific informations" << endl;
		cout << left << setw(10) << "chmod" << left << setw(20) << "file_name mode" << "change the mode of 'file_name' to 'mode' (mode - RWX)" << endl;
		cout << left << setw(10) << "vi" << left << setw(20) << "file_name" << "edit 'file_name' if it exists, else create a file named 'file_name' and edit it" << endl;
		cout << left << setw(10) << "rm" << left << setw(20) << "file_name" << "remove the file named 'file_name' which should be excutable" << endl;
		cout << left << setw(10) << "rn" << left << setw(20) << "old_name new_name" << "rename the file named 'old_name' to 'new_name'" << endl;
		cout << left << setw(10) << "mv" << left << setw(20) << "d/f_name target" << "move the directory or file named 'd/f_name' to 'target' directory" << endl;
		cout << left << setw(10) << "cp" << left << setw(20) << "d/f_name target" << "copy the directory or file named 'd/f_name' to 'target' directory" << endl;
		cout << left << setw(10) << "super" << left << setw(20) << " " << "change role to super to get more authorities" << endl;
		cout << left << setw(10) << "desuper" << left << setw(20) << " " << "change role to user to operate safely" << endl;
		cout << left << setw(10) << "disk" << left << setw(20) << " " << "show the memory usage condition" << endl;
	}
	else if (op == "cd")	{
		cout << left << setw(10) << "cd" << left << setw(20) << "target|.. " << "change working directory to 'target'(absolutely and relatively)|go to upper directory" << endl;
	}
	else if (op == "pwd") {
		cout << left << setw(10) << "pwd" << left << setw(20) << " " << "print working directory" << endl;
	}
	else if (op == "help") {
		cout << left << setw(10) << "help" << left << setw(20) << " " << "show the help document" << endl;
	}
	else if (op == "clear"){
		cout << left << setw(10) << "clear" << left << setw(20) << " " << "clear the screen" << endl;
	}
	else if (op == "history") {
		cout << left << setw(10) << "history" << left << setw(20) << " " << "show the command history" << endl;
	}
	else if (op == "exit") {
		cout << left << setw(10) << "exit" << left << setw(20) << " " << "exit the file system" << endl;
	}
	else if (op == "mkdir") {
		cout << left << setw(10) << "mkdir" << left << setw(20) << "dir_name" << "make an new directory named 'dir_name' at working directory" << endl;
	}
	else if (op == "rmdir") {
		cout << left << setw(10) << "rmdir" << left << setw(20) << "[-f] dir_name" << "remove the directory named 'dir_name' which should be empty. Do mvdir with -f to remove 'dir_name' no matter it is empty or not" << endl;
	}
	else if (op == "rndir") {
		cout << left << setw(10) << "rndir" << left << setw(20) << "old_name new_name" << "rename the directory named 'old_name' to 'new_name'" << endl;
	}
	else if (op == "ls") {
		cout << left << setw(10) << "ls" << left << setw(20) << "[-l]" << "list the files and directoris at working directory. Do ls with -l to list the specific informations" << endl;
	}
	else if (op == "chmod") {
		cout << left << setw(10) << "chmod" << left << setw(20) << "file_name mode" << "change the mode of 'file_name' to 'mode' (mode - RWX)" << endl;
	}
	else if (op == "vi") {
		cout << left << setw(10) << "vi" << left << setw(20) << "file_name" << "edit 'file_name' if it exists, else create a file named 'file_name' and edit it" << endl;
	}
	else if (op == "rm") {
		cout << left << setw(10) << "rm" << left << setw(20) << "file_name" << "remove the file named 'file_name' which should be excutable" << endl;
	}
	else if (op == "rn") {
		cout << left << setw(10) << "rn" << left << setw(20) << "old_name new_name" << "rename the file named 'old_name' to 'new_name'" << endl;
	}
	else if (op == "mv") {
		cout << left << setw(10) << "mv" << left << setw(20) << "dir/file_name target" << "move the directory or file named 'dir.file_name' to 'target' directory" << endl;
	}
	else if (op == "cp") {
		cout << left << setw(10) << "cp" << left << setw(20) << "dir/file_name target" << "copy the directory or file named 'dir.file_name' to 'target' directory" << endl;
	}
	else if (op == "super") {
		cout << left << setw(10) << "super" << left << setw(20) << " " << "change role to super to get more authorities" << endl;
	}
	else if (op == "desuper") {
		cout << left << setw(10) << "desuper" << left << setw(20) << " " << "change role to user to operate safely" << endl;
	}
	else if (op == "disk") {
		cout << left << setw(10) << "disk" << left << setw(20) << " " << "show the memory usage condition" << endl;
	}
	else {
		cout << "there is no command named '" << op << "'" << endl;
	}
}

/*
Summary: clean the screen
*/
void clear()
{
	system("cls");
}

/*
Summary: show the history of the former commands
*/
void show_history()
{
	vector<string>::iterator it = history.begin();
	for (; it < history.end(); it++)
	{
		cout << *it << endl;
	}
}

/*
Summary: change the working directory using absolute path
Parameters:
	target: the target path
*/
void cd_absolute(string target)
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
			if (temp->name == target_sub && temp->get_type() == 1)
			{
				if (temp->get_mode() == 0 && role == "user")
				{
					cerr << "Error: You have no authority to access the directory " << target << endl;
					current = pre_path;		//go back to previous path
					return;
				}
				current = temp;
				found = true;
				break;
			}
		}
		if (!found)
		{
			cerr << "Error: There is no directory named " << target << endl;
			current = pre_path;		//go back to previous path
		}
	}
}

/*
Summary: go back to the upper directory
Parameters:
	cur: working directory
*/
void cd_backward(FCB* cur)
{
	string currentpath = cur->get_path();
	int index = currentpath.find_last_of("/");
	string parent = currentpath.substr(0, index);
	if (parent == "/")
		return;
	cd_absolute(parent);
}

/*
Summary: change the working directory using relative path
Parameters:
	target: the target path based on working directory
*/
void cd_relative(string target)
{
	FCB* pre_path = current;			//record the working path in order to go back if cd fails
	vector<string> paths = split(target, "/");
	vector<string>::iterator it;
	for (it = paths.begin(); it < paths.end(); it++)
	{
		bool found = false;
		string target_sub = *it;
		if (target_sub == "..")
		{
			cd_backward(current);
			continue;
		}
		vector<FCB*> subdir = current->subdir;
		vector<FCB*>::iterator fit;
		for (fit = subdir.begin(); fit < subdir.end(); fit++)
		{
			FCB* temp = *fit;
			if (temp->name == target_sub && temp->get_type() == 1)
			{
				if (temp->get_mode() == 0 && role == "user")
				{
					cerr << "Error: You have no authority to access the directory " << target << endl;
					current = pre_path;		//go back to previous path
					return;
				}
				current = temp;
				found = true;
				break;
			}
		}
		if (!found)
		{
			cerr << "Error: There is no directory named " << target << endl;
			current = pre_path;		//go back to previous path
		}
	}
}

/*
Summary: list the names of subfile and subdir of the working directory
*/
void list_simple()
{
	vector<FCB*> subdir = current->subdir;
	vector<FCB*>::iterator it;
	for (it = subdir.begin(); it < subdir.end(); it++)
	{
		cout << (*it)->name << "      ";
	}
	cout << endl;
}

/*
Summary: list the subfile and subdir of the working directory speceficly
*/
void list_specific()
{
	vector<FCB*> subdir = current->subdir;
	vector<FCB*>::iterator it;
	cout << left << setw(15) << "name" << left << setw(20) << "modified time" << left << setw(7) << "mode";
	cout << left << setw(12) << "type" << "size" << endl;
	for (it = subdir.begin(); it < subdir.end(); it++)
	{
		FCB fcb = **it;
		tm time = fcb.touch_time;
		string _touch_time = to_string(time.tm_year + 1900) + "/" + to_string(time.tm_mon + 1) + "/" + to_string(time.tm_mday) + " "
			+ to_string(time.tm_hour) + ":" + to_string(time.tm_min);
		cout << left << setw(15) << fcb.name << left << setw(20) << _touch_time << left << setw(7) << fcb.get_mode();
		if (fcb.get_type() == 1)
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

/*
Summary: print working directory
*/
void print_workingdir()
{
	cout << current->get_path() << endl;
}

/*
Summary: make a directory under the working directory
Parameters:
	dir_name: the new directory name
*/
void make_dir(string dir_name)
{
	vector<FCB*> subs = current->subdir;
	vector<FCB*>::iterator it = subs.begin();
	for (; it < subs.end(); it++)
	{
		if ((*it)->name == dir_name && (*it)->get_type() == 1)
		{
			cerr << "Error: Directory " << dir_name << " has already existed!" << endl;
			return;
		}
	}
	FCB *fcb = new FCB(current->get_path(), dir_name, 1);
	fcb->set_mode(7);
	current->subdir.push_back(fcb);
}

/*
Summary: release the data block memory --> set the bittable to let the data block page unused
Parameters:
	branch: the directory being removed
*/
void dispose_dir(FCB* branch)
{
	//set the data pages to unused
	vector<int> pages = branch->get_pages();
	vector<int>::iterator pit;
	for (pit = pages.begin(); pit < pages.end(); pit++)
	{
		int page = *pit;
		bit_table[page] = 0;
	}
	//set subdir's data pages to unused
	vector<FCB*> subdir = branch->subdir;
	vector<FCB*>::iterator fit;
	if (subdir.size() == 0)
		return;
	for (fit = subdir.begin(); fit < subdir.end(); fit++)
	{
		dispose_dir(*fit);
	}
}

/*
Summary: remove a directory under the working directory which should be empty
Parameters:
	dir_name:the directory name
*/
void remove_dir(string dir_name)	//!!!!!!!!!!!
{
	vector<FCB*> subdir = current->subdir;
	vector<FCB*>::iterator fit;
	bool found = false;		//if found
	for (fit = subdir.begin(); fit < subdir.end(); fit++)
	{
		if ((*fit)->name == dir_name && (*fit)->get_type() == 1)
		{
			if ((*fit)->get_mode() == 0 && role == "user")
			{
				cerr << "Error: Directory " << dir_name << " cannot be removed because you have no authority" << endl;
				return;
			}
			if ((*fit)->subdir.size() != 0)
			{
				cout << "Warning: " << dir_name << " is not an empty directory. If you still want to remove it, try to do it with -f " << endl;
				return;
			}
			found = true;
			//release the data the directory occupied
			dispose_dir(*fit);
			//erase it from fcb-tree
			fit = subdir.erase(fit);
			break;
		}
	}
	if (!found)
	{
		cerr << "Error: There is no " << dir_name << " directory!" << endl;
		return;
	}
	//update the subdir
	current->subdir = subdir;
}

/*
Summary: remove a directory under the working directory forcely
Parameters:
	dir_name:the directory name
*/
void remove_dir_force(string dir_name)
{
	vector<FCB*> subdir = current->subdir;
	vector<FCB*>::iterator fit;
	bool found = false;		//if found
	for (fit = subdir.begin(); fit < subdir.end(); fit++)
	{
		if ((*fit)->name == dir_name && (*fit)->get_type() == 1)
		{
			if ((*fit)->get_mode() == 0 && role == "user")
			{
				cerr << "Error: Directory " << dir_name << " cannot be removed because you have no authority" << endl;
				return;
			}
			found = true;
			//release the data the directory occupied
			dispose_dir(*fit);
			//erase it from fcb-tree
			fit = subdir.erase(fit);
			break;
		}
	}
	if (!found)
	{
		cerr << "Error: There is no " << dir_name << " directory!" << endl;
		return;
	}
	//update the subdir
	current->subdir = subdir;
}

/*
Summary: rename the subdir's path of fcb
Parameters:
	fcb: the fcb under operated
	new_parent: the changed parent path
*/
void rename_subdir(FCB* fcb, string new_parent)
{
	//update parent
	fcb->set_path(new_parent);
	//update path
	fcb->set_name(fcb->name);
	//update subdir's path
	vector<FCB*> subs = fcb->subdir;
	vector<FCB*>::iterator it;
	if (subs.size() == 0 && fcb->get_type() == 0)
		return;
	for (it = subs.begin(); it < subs.end(); it++) 
	{
		rename_subdir((*it), fcb->get_path());
	}
}

/*
Summary: rename a directory under the working directory
Parameters:
	dir_name: the source directory's name
	new_name: new name
*/
void rename_dir(string dir_name, string new_name)
{
	vector<FCB*> subdir = current->subdir;
	vector<FCB*>::iterator it;
	//find the source file
	FCB *source = new FCB();
	bool found = false;		//if found
	for (it = subdir.begin(); it < subdir.end(); it++)
	{
		if ((*it)->name == dir_name && (*it)->get_type() == 1)
		{
			if ((*it)->get_mode() == 0 && role == "user")
			{
				cerr << "Error: Directory " << dir_name << " cannot be renamed because you have no authority" << endl;
				return;
			}
			found = true;
			source = (*it);
		}
	}
	if (!found)
	{
		cout << "Error: There is no " << dir_name << " directory" << endl;
		return;
	}
	//rename the source file
	source->update_name(new_name);
	subdir = source->subdir;
	string parent = source->get_path();
	//update the path of subdir
	for (it = subdir.begin(); it < subdir.end(); it++)
	{
		rename_subdir((*it), parent);
	}
}

/*
Summary: change the authority mode of the source directory
Parameters:
	dir_name: the source directory's name
	_mode: new mode
*/
void change_mode(string file_name, int _mode)
{
	vector<FCB*> subs = current->subdir;
	vector<FCB*>::iterator it;
	bool found = false;
	//find the target file and change the mode of it
	for (it = subs.begin(); it < subs.end(); it++)
	{
		if ((*it)->name == file_name)
		{
			(*it)->set_mode(_mode);
			found = true;
			break;
		}
	}
	if (!found)
	{
		cerr << "Error: There is no file or directory named " << file_name << endl;
	}
}

/*
Summary: read the file's data if it is not empty, or create a file and write it if it is empty
Parameters:
	file_name: the target file name
*/
void vi(string file_name)
{
	vector<FCB*> subs = current->subdir;
	vector<FCB*>::iterator it = subs.begin();
	FCB* file = new FCB();
	int flag = 0;
	for (; it < subs.end(); it++)
	{
		if ((*it)->name == file_name && (*it)->get_type() == 0)
		{
			flag = 1;
			file = (*it);
			break;
		}
	}
	//when there is such file, read it
	if (flag == 1) {
		//about data operation
		if (file->get_mode() < 4 && role == "user") {
			cerr << "Error: File " << file_name << " cannot be read because you have no authority" << endl;
		}
		else {
			//update file's touch time
			time_t now = time(0);
			tm *gmtm = gmtime(&now);
			file->touch_time = *gmtm;
			//show the content
			vector<int> pages = file->get_pages();
			open_data(pages);
		}
	}
	//when there is no such file, create and write it
	else {
		FCB *new_fcb = new FCB(current->get_path(), file_name, 0);
		int lplength;
		new_fcb->set_mode(7);
		//about data operation
		cout << "Please input the content of " << file_name << endl;
		vector<int> pages = new_data(lplength);
		new_fcb->set_pages(pages);
		new_fcb->set_lplength(lplength);
		current->subdir.push_back(new_fcb);
		cout << file_name << " has been created" << endl;
	}
}

/*
Summary: remove the file under the working directory
Parameters:
	file_name: the target file name
*/
void remove(string file_name)
{
	vector<FCB*> subdir = current->subdir;
	vector<FCB*>::iterator it;
	FCB* file = new FCB();
	bool found = false;		//if found
	for (it = subdir.begin(); it < subdir.end(); it++)
	{
		if ((*it)->name == file_name && (*it)->get_type() == 0)
		{
			//RWX X-bit is 0
			if ((*it)->get_mode() % 2 == 0 && role == "user") {
				cerr << "Error: " << file_name << " cannot be removed because you have no authority" << endl;
				return;
			}
			found = true;
			file = *it;

			//(*it)->dispose();		//É¾³ýfcbºÍdata

			//´ÓsubdirÖÐÉ¾³ýdir_name
			it = subdir.erase(it);
			break;
		}
	}
	if (!found)
	{
		cerr << "Error: There is no file named " << file_name << endl;
		return;
	}
	//release the data-block file occupied
	vector<int> pages = file->get_pages();
	vector<int>::iterator pit = pages.begin();
	for (; pit<pages.end(); pit++)
	{
		bit_table[*pit] = 0;
	}
	current->subdir = subdir;
}

/*
Summary: rename the file
Parameters:
	old_name: the target file's name
	new_name: new name
*/
void rename(string old_name, string new_name)
{
	vector<FCB*> subdir = current->subdir;
	vector<FCB*>::iterator it;
	//find the target file
	bool found = false;		//if found
	for (it = subdir.begin(); it < subdir.end(); it++)
	{
		if ((*it)->name == old_name && (*it)->get_type() == 0)
		{
			//check the authority
			if ((*it)->get_mode() % 2 == 0 && role == "user") {
				cerr << "Error: " << old_name << " cannot be renamed because you have no authority" << endl;
				return;
			}
			found = true;
			(*it)->update_name(new_name);
			//update file's touch time
			time_t now = time(0);
			tm *gmtm = gmtime(&now);
			(*it)->touch_time = *gmtm;
			break;
		}
	}
	if (!found)
	{
		cerr << "Error: There is no file named " << old_name << endl;
		return;
	}
}

/*
Summary: move the file or directory to destination directory
Parameters:
	_source: the source file or directory
	destination: the destination
*/
void move(string _source, string destination)
{
	FCB* pre_path = current;			//record the working path in order to go back if cd fails
	//find destination
	if (destination[0] == '/')
		cd_absolute(destination);
	else cd_relative(destination);
	if (current->get_path() == pre_path->get_path())
		return;
	string new_parent = current->get_path();
	//find source
	FCB* source = new FCB();
	vector<FCB*> subs = pre_path->subdir;
	vector<FCB*>::iterator fit;
	bool found = false;
	for (fit = subs.begin(); fit < subs.end(); fit++)
	{
		if ((*fit)->name == _source) {
			found = true;
			//no granted dir
			if ((*fit)->get_type() == 1 && (*fit)->get_mode() == 0 && role == "user") {
				cerr << "Error: You have no authority to move the directory named " << _source << endl;
				return;
			}
			//no granted file
			if ((*fit)->get_type() == 0 && (*fit)->get_mode() % 2 == 0 && role == "user") {
				cerr << "Error: You have no authority to move the file named " << _source << endl;
				return;
			}
			source = *fit;
			//delete source from origin tree
			fit = subs.erase(fit);
			break;
		}
	}
	if (!found) {
		cerr << "Error: There is no file or directory named " << _source << endl;
		return;
	}
	pre_path->subdir = subs;
	//rename the subdir
	rename_subdir(source, new_parent);
	//repair the fcb tree
	current->subdir.push_back(source);
	//go back to origin directory
	current = pre_path;
}

/*
Summary: copy subdir and subfile of fcb
Parameters:
	fcb: the fcb under copying
	_parent: the destination path
Return: the fcb after copying building
*/
FCB* copy_subdir(FCB* fcb, string _parent)
{
	//create a fcb and copy the properties from source to copy_fcb
	FCB *copy = new FCB(_parent, fcb->name, fcb->get_type());
	copy->set_mode(fcb->get_mode());
	cout << "dd" << endl;
	DataPipe *pipe = new DataPipe();
	pipe->open(true);
	//create data-blocks and copy the content from source's data-blocks to copy_fcb's
	vector<int> pages = fcb->get_pages();
	vector<int> copy_pages;
	for (vector<int>::iterator it = pages.begin(); it < pages.end(); it++)
	{
		DataBlock *block = new DataBlock();
		int newpage = find_first_emptypage();
		pipe->read(block, (*it));
		block->block_id = newpage;
		pipe->write(block, newpage);
		delete block;
		copy_pages.push_back(newpage);
	}
	pipe->close();
	delete pipe;
	copy->set_pages(copy_pages);
	//copy subdir
	vector<FCB*> subs = fcb->subdir;
	if (subs.size() == 0 && fcb->get_type() == 0)	//file
		return copy;
	vector<FCB*>::iterator sit;
	for (sit = subs.begin(); sit < subs.end(); sit++)
	{
		FCB *leaf = copy_subdir((*sit), copy->get_path());
		copy->subdir.push_back(leaf);
	}
	copy->set_lplength(fcb->get_lplength());
	return copy;
}

/*
Summary: copy file or directory to destination directory
Parameters:
	_source: the source file or directory
	destination: the destination
*/
void copy(string _source, string destination)
{
	FCB* pre_path = current;			//record the working path in order to go back if cd fails
	//find destination
	if (destination[0] == '/')
		cd_absolute(destination);
	else cd_relative(destination);
	if (current->get_path() == pre_path->get_path())
		return;
	string new_parent = current->get_path();
	//find source
	FCB* source = new FCB();
	vector<FCB*> subs = pre_path->subdir;
	vector<FCB*>::iterator it;
	bool found = false;
	for (it = subs.begin(); it < subs.end(); it++)
	{
		if ((*it)->name == _source) {
			found = true;
			//no granted dir
			if ((*it)->get_type() == 1 && (*it)->get_mode() == 0 && role == "user") {
				cerr << "Error: You have no authority to copy the directory named " << _source << endl;
				return;
			}
			//no granted file
			if ((*it)->get_type() == 0 && (*it)->get_mode() % 2 == 0 && role == "user") {
				cerr << "Error: You have no authority to move the file named " << _source << endl;
				return;
			}
			source = *it;
			break;
		}
	}
	if (!found) {
		cerr << "Error: There is no file or directory named " << _source << endl;
		return;
	}
	//copy to destination
	FCB *copy = copy_subdir(source, new_parent);
	current->subdir.push_back(copy);
	current = pre_path;
}

/*
Summary: change role to super in order to obtain the highst authority
*/
void change2super()
{
	string pwd;
	cout << "Please enter the password: ";
	getline(cin, pwd);
	if (pwd == "123456")
	{
		role = "super";
	}
}

/*
Summary: change role to user in order to safely operate
*/
void change2user()
{
	role = "user";
}

int main()
{
	init();
	while (true)
	{
		//output the head
		cout << role << "@" << current->get_path() << ">";
		//input the command
		string cmd;
		getline(cin, cmd);
		//split the command to guide into operation
		vector<string> parts = split(cmd, " ");
		if (parts[0] == "exit")
		{
			exit();
			break;
		}
		else if (parts[0] == "cd") {
			if (parts.size() < 2) {
				cerr << "Error: Please input the target" << endl;
				continue;
			}
			if (parts[1][0] == '/')
				cd_absolute(parts[1]);
			else cd_relative(parts[1]);
		}
		else if (parts[0] == "pwd") {
			print_workingdir();
		}
		else if (parts[0] == "help") {
			help();
		}
		else if (parts[0] == "clear") {
			clear();
		}
		else if (parts[0] == "history") {
			show_history();
		}
		else if (parts[0] == "mkdir") {
			if (parts.size() < 2) {
				cerr << "Error: Please input the directory name" << endl;
				continue;
			}
			string dir_name = parts[1];
			make_dir(dir_name);
		}
		else if (parts[0] == "rmdir") {
			if (parts.size() < 2) {
				cerr << "Error: Please input the directory name that needs to be removed" << endl;
				continue;
			}
			if (parts[1][0] == '-')
			{
				if (parts[1] != "-f")
				{
					cerr << "Error: Command rmdir has no " << parts[1] << " parameter" << endl;
					continue;
				}
				if (parts.size() < 3) {
					cerr << "Error: Please input the directory name that needs to be removed in force" << endl;
					continue;
				}
				remove_dir_force(parts[2]);
			}
			else
			{
				remove_dir(parts[1]);
			}
		}
		else if (parts[0] == "rndir") {
			if (parts.size() < 3) {
				cerr << "Error: Please input the old directory name and the new directory name" << endl;
				continue;
			}
			//need improved
			else rename_dir(parts[1], parts[2]);
		}
		else if (parts[0] == "ls") {
			if (parts.size() == 2)
			{
				if (parts[1] == "-l")
					list_specific();
				else {
					cerr << "Error: Command ls has no parameter " << parts[1] << endl;
					continue;
				}
			}
			else list_simple();
		}
		else if (parts[0] == "chmod") {
			if (parts.size() < 3) {
				cerr << "Error: Please input the file name and the new mode" << endl;
				continue;
			}
			if (atoi(parts.at(2).c_str()) < 0 || atoi(parts.at(2).c_str()) > 7) {
				cerr << "Error: The second parameter should be an integer ranging from 0 to 7" << endl;
				continue;
			}
			change_mode(parts[1], atoi(parts.at(2).c_str()));
		}
		else if (parts[0] == "vi") {
			if (parts.size() < 2) {
				cerr << "Error: Please input the file name " << endl;
				continue;
			}
			vi(parts[1]);
		}
		else if (parts[0] == "rm") {
			if (parts.size() < 2) {
				cerr << "Error: Please input the file name " << endl;
				continue;
			}
			remove(parts[1]);
		}
		else if (parts[0] == "rn") {
			if (parts.size() < 3) {
				cerr << "Error: Please input the old name and the new name" << endl;
				continue;
			}
			rename(parts[1], parts[2]);
		}
		else if (parts[0] == "mv") {
			if (parts.size() < 3) {
				cerr << "Error: Please input the source and the destination" << endl;
				continue;
			}
			move(parts[1], parts[2]);
		}
		else if (parts[0] == "cp") {
			if (parts.size() < 3) {
				cerr << "Error: Please input the source and the destination" << endl;
				continue;
			}
			copy(parts[1], parts[2]);
		}
		else if (parts[0] == "?") {
			if (parts.size() < 2)
			{
				cerr << "Error: Please input the command that you want to know" << endl;
				continue;
			}
			help(parts[1]);
		}
		else if (parts[0] == "super") {
			change2super();
			cin.clear();
		}
		else if (parts[0] == "desuper") {
			change2user();
		}
		else if (parts[0] == "disk") {
			show_disk();
		}
		else {
			cerr << "Error: There is no command named " << parts[0] << endl;
		}
		//push the right commands into history
		history.push_back(cmd);
	}
	return 0;
}