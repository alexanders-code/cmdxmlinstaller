// ************************************************************************ 
//   PackageBuilder.h - v 0.0.1
// ************************************************************************ 
#pragma once

#ifndef _CMDINST_PACKAGEBUILDER_H
#define _CMDINST_PACKAGEBUILDER_H

#include <vector>

#include "DataStruct.h"
#include "DocInstaller.h"
#include "FileOperations.h"

class PackageBuilder
{
public:

	PackageBuilder( DocInstaller* di );
	virtual ~PackageBuilder(void);

	bool CalculateAllSize();

	bool BuildSelfInstaller( void );
	
private:
		
	HeaderSelf _hs;
	
	DataBlock _currblock;
	std::vector< DataBlock > _datablockarray;

	std::vector< CommandBlock > _commandblockarray;

	DocInstaller* _installerfile;

	std::string _fullnameinstaller;

	PackageBuilder() {};

	ULARGE_INTEGER CalculateAllFileSize( void );

	void FillDataBlockArr( void );

	void FillCommandBlockArr( void );

	void ClearCurrDataBlock( void );

	void FillHeaderSelf( void );

	void GetNameInstaller( void );

	void ExtractStub( void );

	bool SetInfoResource( void );

	void WriteDataInStub( FileOperations* f );

	bool AddAllFileToStub( FileOperations* f );
};

#endif //_CMDINST_PACKAGEBUILDER_H