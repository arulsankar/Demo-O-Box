//---------------------------------------------------------------------------
// FILE NAME:   FwUpgradeStub.h
// AUTHOR:      
// DESCRIPTION: Stub header file
// Copyright (C).  All rights reserved.
//---------------------------------------------------------------------------

#ifndef FWUPGRADESTUB_H_
#define FWUPGRADESTUB_H_


using namespace std;

class FwUpgradeStub
{
	public:

		FwUpgradeStub();
		virtual ~FwUpgradeStub();

		FILE *fptrReadfp;
		FILE *fptrWriteMd5fp;
		FILE *fptrWriteAppfp;
		FILE *fptrWriteVerfp;

		const static  uint8_t MAGIC_NUM_SIZE  		= 10;
		const static  uint8_t MD5_KEY_SIZE    		= 2;
		const static  uint8_t VERSION_NUM_SIZE    	= 5;
		const static  uint8_t BUILD_DATE_TIME_SIZE  = 25;
		const static  uint8_t READ_ONE_BYTE	  		= 1;
		const static  uint8_t PARSE_BIN_FAILED		= -1;
		const static  uint8_t ERR_FILE_INTEGRITY_FAILED = -1;

		int16_t parseBinFile();
		int16_t checkFileIntegrity();
		int FileDownload();

	private :

		FILE *fptrIn,*fptrOut1,*fptrOut2;

};



#endif

/* FWUPGRADESTUB_H_ */
