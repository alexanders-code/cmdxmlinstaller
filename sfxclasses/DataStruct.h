#include <Windows.h>

struct HeaderSelf
//� ��������� exe ����� 
{
	DWORD OffsetStartData;
	DWORD SizeAllDataBlock;
};

struct DataBlock
// ������ ��� ���������� exe
{
	char NameFile[ _MAX_PATH ];
	char ExtractTo[ _MAX_PATH ];
	DWORD FileSize;
};