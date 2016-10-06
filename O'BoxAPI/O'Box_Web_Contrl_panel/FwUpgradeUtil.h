//---------------------------------------------------------------------------
// FILE NAME:   FwUpgradeUtil.h
// AUTHOR:      Sanmina
// DESCRIPTION: Class used to read and parse the binary files
// Copyright (C) 2015 Sanmina IDC.  All rights reserved.
//---------------------------------------------------------------------------

#ifndef FwUpgradeUtil_H_
#define FWUPGRADEUTIL_H_

///////////////////////////////////////////////////////////////////////////////
// INCLUDES
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// CLASS
///////////////////////////////////////////////////////////////////////////////

class FwUpgradeUtil
{
	public:

		FwUpgradeUtil();

		virtual ~FwUpgradeUtil();

		//! Function Declarations

		FILE* OpenFileToRead(const char* ptru8ReadFilePath);

		FILE* OpenFileToWrite(const char* ptru8WriteFilePath);

		uint32_t ReadFile(FILE* fptr,size_t m_u32NoOfBytestoRead,uint16_t m_u16Offset,uint8_t *ReadData);

		uint32_t ParseFile(FILE* fptrIn,FILE* fptrOut,size_t m_u32NoOfBytestoWrite,uint16_t m_u16Offset,uint8_t *WriteDataToFile);

	private:

		//! Class Member Declarations

		//! File Pointers for Input/Output files
		FILE *fptrIn;
		FILE *fptrOut;
		//! Holds the Return Value of functions
	    uint32_t  m_u32ReturnValue;
	    //! Holds the size of read bytes from a file
	    uint32_t  m_u32ReadSizeInBytes;
	    //! Holds the size of write bytes from a file
	    uint32_t  m_u32WriteSizeInBytes;

	    //! Error Cases
	    const static  int8_t FAIL_TO_READ  = -1;
	    const static  int8_t FAIL_TO_PARSE = -2;

};


#endif /* FwUpgradeUtil_H_ */
