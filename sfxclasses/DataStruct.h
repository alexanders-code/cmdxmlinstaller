// ************************************************************************ 
//   DataStruct.h - v 0.0.1
// ************************************************************************
#pragma once

#ifndef _CMDINST_DATASTRUCT_H
#define _CMDINST_DATASTRUCT_H

#include <Windows.h>


struct HeaderSelf
//в заголовок exe файла 
{
	DWORD OffsetStartData;
	DWORD SizeAllCommandBlock;
	DWORD SizeAllDataBlock;
};


struct DataBlock
// данные для распаковки exe
{
	char NameFile[ _MAX_PATH ];
	char ExtractTo[ _MAX_PATH ];
	DWORD FileSize;
	time_t ModifyTime; //дата последней модификации файла
	DWORD Attributes;
};


static const int CommandMaxLength = 30;
static const int ParameterMaxLength = 20;

struct CommandBlock
//команды и параметры, которые будут выполниться при запусте инсталлятора
//формат команды элементы xml разделенные - / interface/progress
{
	char Command[ CommandMaxLength ];
	char Parameter[ ParameterMaxLength ];
};

#endif // #ifndef _CMDINST_DATASTRUCT_H