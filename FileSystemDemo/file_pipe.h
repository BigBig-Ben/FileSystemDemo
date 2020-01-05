/****************************
Copyright: Zebao Zhou
File name: file_pipe.h
Author: Zebao Zhou

Version: 1.0
Date: 2019-12-26
Description: filepipe class to write data to file in order to store
****************************/
#include<fstream>
#include"data_block.h"
#include"fcb_core.h"
using namespace std;

class FCBPipe
{
public:
	FCBPipe() {}
	~FCBPipe() {}
	bool store(FCB* root);
	FCB* load();
private:
	streampos save_tree(FCB* fcb, char data[], streampos spos);
};


class DataPipe
{
private:
	FILE *pfile;		//data stored file
	int length;			//the length of blocks
	int size;			//data block size
public:
	DataPipe() { size = sizeof(DataBlock); length = 1024; }
	~DataPipe() {}
	bool open(bool bEdit = false);
	bool read(DataBlock* block, int index);
	bool write(DataBlock* block, int index);
	void close();
	bool read_bittable(short* bit_table);
	bool write_bittable(short* bit_table);
};