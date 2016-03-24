#include <Windows.h>

struct HeaderSelf
//в заголовок exe файла 
{
	DWORD OffsetStartData;
	DWORD SizeAllDataBlock;
};

struct DataBlock
// данные для распаковки exe
{
	char NameFile[ _MAX_PATH ];
	char ExtractTo[ _MAX_PATH ];
	DWORD FileSize;
};