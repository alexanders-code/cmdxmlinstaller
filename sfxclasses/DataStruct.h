// ************************************************************************ 
//   DataStruct.h - v 0.0.1
// ************************************************************************
#pragma once

#ifndef _CMDINST_DATASTRUCT_H
#define _CMDINST_DATASTRUCT_H

#include <Windows.h>


struct HeaderSelf
//� ��������� exe ����� 
{
	DWORD OffsetStartData;
	DWORD SizeAllCommandBlock;
	DWORD SizeAllDataBlock;
};


struct DataBlock
// ������ ��� ���������� exe
{
	char NameFile[ _MAX_PATH ];
	char ExtractTo[ _MAX_PATH ];
	DWORD FileSize;
	time_t ModifyTime; //���� ��������� ����������� �����
	DWORD Attributes;
};


static const int CommandMaxLength = 30;
static const int ParameterMaxLength = 20;

struct CommandBlock
//������� � ���������, ������� ����� ����������� ��� ������� ������������
//������ ������� �������� xml ����������� - / interface/progress
{
	char Command[ CommandMaxLength ];
	char Parameter[ ParameterMaxLength ];
};

#endif // #ifndef _CMDINST_DATASTRUCT_H