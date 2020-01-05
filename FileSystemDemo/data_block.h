/****************************
Copyright: Zebao Zhou
File name: data_block.h
Author: Zebao Zhou

Version: 1.0
Date: 2019-12-26
Description: memory datablock
****************************/
class DataBlock
{
public:
	int block_id;		//page id in data memory
	char data[256];		//data in page
	DataBlock() {}
	~DataBlock() {}
};