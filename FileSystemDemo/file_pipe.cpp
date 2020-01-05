/****************************
Copyright: Zebao Zhou
File name: file_pipe.cpp
Author: Zebao Zhou

Version: 1.0
Date: 2019-12-26
Description: filepipe class to write data to file in order to store
****************************/
#include"file_pipe.h"
#include<strstream>
#define LENGTH 4096		

/*
Summary: store the fcb-tree into a file 'fcb.dat'
Parameters:
	root: the root directory of the fcb-tree
Return: if the process success
*/
bool FCBPipe::store(FCB* root)
{
	char data[LENGTH];
	ostrstream outpipe(data, LENGTH);
	streampos spos = outpipe.tellp();
	spos = save_tree(root, data, spos);
	outpipe.seekp(LENGTH - 1);
	outpipe << ends;
	ofstream fout("fcb.dat", ios::out);
	fout.write(data, LENGTH);
	fout.close();
	return true;
}

/*
Summary: the detail of the process to save fcb_tree
Parameters:
	fcb: the fcb node under saving
	data: the template data memory
	spos: the streampos to locate the ostrstream
Return: new streampos after a round of saving
*/
streampos FCBPipe::save_tree(FCB* fcb, char data[], streampos spos)
{
	if (fcb != NULL)
	{
		ostrstream outpipe(data, LENGTH);
		outpipe.seekp(spos);
		outpipe << fcb->get_path() << " " << fcb->get_type() << " " << fcb->get_mode() << " " << fcb->tm2string() << " " << fcb->pages2string()
			<< " " << fcb->get_lplength() << endl;
		spos = outpipe.tellp();
		for (vector<FCB*>::iterator it = fcb->subdir.begin(); it < fcb->subdir.end(); it++)
		{
			spos = save_tree(*it, data, spos);
		}
		return spos;
	}
	return spos;
}

/*
Summary: load the fcb-tree from file 'fcb.dat'
Return: the root directory of the fcb-tree
*/
FCB* FCBPipe::load()
{
	char data[LENGTH];
	ifstream fin("fcb.dat", ios::in);
	fin.read(data, LENGTH);
	istrstream inpipe(data, LENGTH);

	string _path;
	int _type, _mode;
	string _pages, _touchtime;
	long _length;
	//load root /
	inpipe >> _path >> _type >> _mode >> _touchtime >> _pages >> _length;
	FCB *root = new FCB("", "/", 1);
	root->set_mode(_mode);
	root->string2tm(_touchtime);
	root->string2pages(_pages);
	root->set_lplength(_length);
	streampos spos = inpipe.tellg();
	FCB *parent = root;
	while (true)
	{
		inpipe >> _path >> _type >> _mode >> _touchtime >> _pages >> _length;
		int index = _path.find_last_of('/');
		if (index < 0)
			break;
		string ppath = _path.substr(0, index + 1);
		string name = _path.substr(index + 1, _path.length());
		FCB *temp = new FCB(ppath, name, _type);
		temp->set_mode(_mode);
		temp->string2tm(_touchtime);
		temp->string2pages(_pages);
		temp->set_lplength(_length);
		if (ppath == parent->get_path())
			parent->subdir.push_back(temp);
		else
		{
			vector<string> paths = temp->split(ppath, "/");
			vector<string>::iterator it;
			FCB *cur = root;
			for (it = paths.begin(); it < paths.end(); it++)
			{
				string target_sub = *it;
				vector<FCB*> subdir = cur->subdir;
				vector<FCB*>::iterator fit;
				for (fit = subdir.begin(); fit < subdir.end(); fit++)
				{
					FCB* temp1 = *fit;
					if (temp1->name == target_sub)
					{
						cur = temp1;
						break;
					}
				}
			}
			parent = cur;
			parent->subdir.push_back(temp);
		}

	}
	fin.close();
	return root;
}

/*
Summary: open data file to prepare to be writed and read
Parameters:
	bEdit: true -> allow to edit; default false
Return: if open data file successes
*/
bool DataPipe::open(bool bEdit)
{
	pfile = fopen("data.dat", bEdit ? "r+b" : "rb");
	if (pfile == NULL)
	{
		cerr << "can not open data.dat " << endl;
		return false;
	}
	fseek(pfile, 0, SEEK_END);
	length = (ftell(pfile) - 2048) / size;
	return true;
}

/*
Summary: read a data-block from data file
Parameters:
	block: the address of the block(to record the read block)
	index: the index of the page
Return: if read successes
*/
bool DataPipe::read(DataBlock* block, int index)
{
	if (index >= length)
	{
		//already finished reading
		return false;
	}
	fseek(pfile, 2048 + index*size, SEEK_SET);
	fread(block, size, 1, pfile);
	return true;
}

/*
Summary:write a data-block to data file
Parameters:
	block: the address of the block waiting for saving
	index: the index of the page
Return: if write successes
*/
bool DataPipe::write(DataBlock* block, int index)
{
	fseek(pfile, 2048 + index*size, SEEK_SET);
	fwrite(block, size, 1, pfile);
	return true;
}

/*
Summary: close the data file to stop reading and writing
*/
void DataPipe::close()
{
	fclose(pfile);
}

/*
Summary: read the bit_table from data file 
Parameters:
	bit_table: the list to record if the pages have been occupied (out)
Return: if read bittable successes
*/
bool DataPipe::read_bittable(short* bit_table)
{
	fseek(pfile, 0, SEEK_SET);
	fread(bit_table, 2048, 1, pfile);
	return true;
}

/*
Summary: write the bit_table to data file
Parameters:
	bit_table: the list to record if the pages have been occupied
Return: if write bittable successes
*/
bool DataPipe::write_bittable(short* bit_table)
{
	fseek(pfile, 0, SEEK_SET);
	fwrite(bit_table, 2048, 1, pfile);
	return true;
}