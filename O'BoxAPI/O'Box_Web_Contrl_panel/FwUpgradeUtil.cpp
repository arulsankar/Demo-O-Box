//---------------------------------------------------------------------------
// FILE NAME:   FwUpgradeUtil.cpp
// AUTHOR:      Sanmina
// DESCRIPTION: Read and Parse the binary files
// Copyright (C) 2015 Sanmina IDC.  All rights reserved.
//---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// INCLUDES
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#include "FwUpgradeUtil.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Constructor for the Fw Upgrade class
///////////////////////////////////////////////////////////////////////////////
FwUpgradeUtil::FwUpgradeUtil()
{
	// Constructor
}

///////////////////////////////////////////////////////////////////////////////
// Destructor for the Fw Upgrade class
///////////////////////////////////////////////////////////////////////////////

FwUpgradeUtil::~FwUpgradeUtil()
{
	// Destructor
}

///////////////////////////////////////////////////////////////////////////////
// CLASS FUNCTION DEFINITIONS
///////////////////////////////////////////////////////////////////////////////

//! OpenFileToRead function opens the file names which are mentioned
//! and associates a stream with it,upon successful completion fopen(),
//! It returns FILE pointer with read binary mode .
//! Otherwise,NULL is returned and errno is set to  indicate the error.
//!
//! \param[in]	   : ptru8ReadFilePath -> pointer to the file path to read.
//!
//! \return 	   : fptrIn 		   -> I/P read file pointer
//!
FILE* FwUpgradeUtil::OpenFileToRead(const char* ptru8ReadFilePath)
{

    fptrIn = fopen(ptru8ReadFilePath,"rb");
    //! If Open fails in read mode checking error condition
    if( fptrIn == NULL )
    {
          perror( ptru8ReadFilePath );
          exit( EXIT_FAILURE );
    }
    //! return File pointer of Opened file to read the data
    return fptrIn;
}

//! OpenFileToWrite function opens the file names which are mentioned
//! and associates a stream with it,upon successful completion fopen(),
//! It returns FILE pointer with write mode ( The file is created if it does
//! not exist, otherwise it is truncated.  The stream is
//! positioned at the beginning of the file..)
//! Otherwise,NULL is returned and errno is set to  indicate the error.
//!
//! \param[in]	   : ptru8WriteFilePath -> pointer to the file path to write.
//!
//! \return 	   : fptrOut 		   -> O/P write file pointer
//!
FILE* FwUpgradeUtil::OpenFileToWrite(const char* ptru8WriteFilePath)
{

	fptrOut = fopen(ptru8WriteFilePath,"w+");
    //! If Open fails in write mode checking error condition
    if( fptrOut == NULL )
    {
          perror( ptru8WriteFilePath );
          exit( EXIT_FAILURE );
    }
    //! return File pointer of Opened file to write the data
    return fptrOut;

}

//! This ReadFile Function reads number of bytes with an offset
//! from a binary file and store to pointer to character array.
//! using fseek weare setting file position,fread will reads the
//! data from a file using filepointer
//!
//! \param[in]	   : fptr -> file pointer of the file for read data.
//! \param[in]	   : m_u32NoOfBytestoRead -> holds the number of bytes to read.
//! \param[in]     : m_u16Offset		  -> holds the offset of the file.
//! \param[out]    : ptru8ReadDataFromFile     -> pointer to character array which stores the read data.
//!
//! \return 	   : m_u32ReturnValue [SUCCESS] -> size of bytes we read from a file
//!									  [FAILURE] -> FAIL_TO_READ
//!
uint32_t FwUpgradeUtil::ReadFile(FILE* fptr,size_t m_u32NoOfBytestoRead,uint16_t m_u16Offset, uint8_t *ptru8ReadDataFromFile)
{
	m_u32ReturnValue = FAIL_TO_READ; //! ERROR Case will return if read fails

	//! Checking filepointer's are valid are not
    if (fptr != NULL)
    {
    	//! Sets the file position of the fptrMd5FileSize to the given
    	//! offset( first byte of the file).
    	//! if Success  -> Returns 0
    	//! if it fails -> Return Non-Zero value
		if ( !(fseek(fptr,m_u16Offset,SEEK_SET) ) )
		{
			//! reads size_t bytes data from the given stream into the ptru8WriteDataToFile array pointed to, by fptr.
			m_u32ReadSizeInBytes = fread((void*)ptru8ReadDataFromFile, sizeof(uint8_t), m_u32NoOfBytestoRead, fptr);

			//! The total number of elements successfully read are returned as a size_t object,which is an integral data type.
			//! If this number differs from the m_u32NoOfBytestoRead parameter, then either an error had occurred or the End Of File was reached.
			if(m_u32ReadSizeInBytes == m_u32NoOfBytestoRead)
			{
				//! copying size of read bytes to return value
				m_u32ReturnValue = m_u32ReadSizeInBytes;
			}
		}
    }
    //! return no of bytes we read from the given stream
    return m_u32ReturnValue;
}

//! This ReadFile Function reads number of bytes with an offset
//! from a binary file and store to pointer to character array.
//! using fseek weare setting file position,fread will reads the
//! data from a file using filepointer
//!
//! \param[in]	   : fptrIn  -> I/P file pointer of the file for read data.
//! \param[in]	   : fptrOut -> O/P file pointer of the file to write data.
//! \param[in]	   : m_u32NoOfBytestoWrite -> holds the number of bytes to write.
//! \param[in]     : m_u16Offset		   -> holds the offset of the file.
//! \param[out]    : ptru8WriteDataToFile       -> pointer to character array which stores the written data.
//!
//! \return 	   : m_u32ReturnValue [SUCCESS] -> size of bytes we read/write from/to a file
//!									  [FAILURE] -> FAIL_TO_PARSE
//!
uint32_t FwUpgradeUtil::ParseFile(FILE* fptrIn,FILE* fptrOut,size_t m_u32NoOfBytestoWrite,uint16_t m_u16Offset,uint8_t *ptru8WriteDataToFile)
{
	m_u32ReturnValue = FAIL_TO_PARSE; //! ERROR Case will return if parsing fails

	//! Checking filepointer's are valid are not
    if (fptrIn != NULL || fptrOut != NULL)
    {
    	//! Sets the file position of the fptrMd5FileSize to the given
    	//! offset( first byte of the file).
    	//! if Success  -> Returns 0
    	//! if it fails -> Return Non-Zero value
		if ( !(fseek(fptrIn,m_u16Offset,SEEK_SET) ) )
		{
			//! reads size_t bytes data from the given stream into the ptru8WriteDataToFile array pointed to, by fptrIn.
			m_u32ReadSizeInBytes = fread((void*)ptru8WriteDataToFile, sizeof(uint8_t), m_u32NoOfBytestoWrite, fptrIn);

			//! The total number of elements successfully read are returned as a size_t object,which is an integral data type.
			//! If this number differs from the m_u32NoOfBytestoWrite parameter, then either an error had occurred or the End Of File was reached.
			if(m_u32ReadSizeInBytes == m_u32NoOfBytestoWrite)
			{
				//! writes data from the array pointed to, by fptrOut to the given stream.
				m_u32WriteSizeInBytes = fwrite((void*)ptru8WriteDataToFile, sizeof(uint8_t), m_u32NoOfBytestoWrite, fptrOut);
				//! This function returns the total number of elements successfully returned as a size_t object, which is an integral data type.
				//! If this number differs from the m_u32NoOfBytestoWrite parameter, it will show an error.
				if(m_u32WriteSizeInBytes == m_u32NoOfBytestoWrite)
				{
					//! copying size of written bytes to return value
					m_u32ReturnValue = m_u32WriteSizeInBytes;
				}
			}
		}
    }
    //! return no of bytes we read/write from the given stream
    return m_u32ReturnValue;
}
