///
//	webserver.cpp -- libmicrohttpd web interface for ozwcp
//
//	Copyright (c) 2010 Greg Satz <satz@iranger.com>
//	All rights reserved.
//
// SOFTWARE NOTICE AND LICENSE
// This work (including software, documents, or other related items) is being
// provided by the copyright holders under the following license. By obtaining,
// using and/or copying this work, you (the licensee) agree that you have read,
// understood, and will comply with the following terms and conditions:
//
// Permission to use, copy, and distribute this software and its documentation,
// without modification, for any purpose and without fee or royalty is hereby
// granted, provided that you include the full text of this NOTICE on ALL
// copies of the software and documentation or portions thereof.
//
// THIS SOFTWARE AND DOCUMENTATION IS PROVIDED "AS IS," AND COPYRIGHT HOLDERS
// MAKE NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO, WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
// PURPOSE OR THAT THE USE OF THE SOFTWARE OR DOCUMENTATION WILL NOT INFRINGE
// ANY THIRD PARTY PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.
//
// COPYRIGHT HOLDERS WILL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL OR
// CONSEQUENTIAL DAMAGES ARISING OUT OF ANY USE OF THE SOFTWARE OR
// DOCUMENTATION.
//
// The name and trademarks of copyright holders may NOT be used in advertising
// or publicity pertaining to the software without specific, written prior
// permission.  Title to copyright in this software and any associated
// documentation will at all times remain with copyright holders.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>

#include <netdb.h>
#include <ifaddrs.h>

#include "Manager.h"
#include "Driver.h"
#include "Node.h"
#include "ValueBool.h"
#include "ValueByte.h"
#include "ValueDecimal.h"
#include "ValueInt.h"
#include "ValueList.h"
#include "ValueRaw.h"
#include "ValueShort.h"
#include "ValueString.h"
#include "tinyxml.h"

#include "Options.h"
#include "Manager.h"
#include "Node.h"
#include "Group.h"
#include "Notification.h"
#include "Log.h"

#include "microhttpd.h"
#include "ozwcp.h"
#include "webserver.h"
/*Changes made by ANANDARAJ for FwUpgrade */
#include "FwUpgradeStub.h"
#include "FwUpgradeUtil.h"
/*
#include <errno.h>
#include <iostream>
#include <string>
*/
#include "json.h"
#include "oboxhttp.h"

/*End line */
using namespace OpenZWave;

#define NBSP(str)	(str) == NULL || (*str) == '\0' ? "&nbsp;" : (str)
#define BLANK(str)	(str) == NULL || (*str) == '\0' ? "" : (str)
#define FNF "<html><head><title>File not found</title></head><body>File not found: %s</body></html>"
#define UNKNOWN "<html><head><title>Nothingness</title></head><body>There is nothing here. Sorry.</body></html>\n"
#define DEFAULT "<script type=\"text/javascript\"> document.location.href='/';</script>"
#define EMPTY "<html></html>"

typedef struct _conninfo {
		conntype_t conn_type;
		char const *conn_url;
		void *conn_arg1;
		void *conn_arg2;
		void *conn_arg3;
		void *conn_arg4;
		void *conn_res;
		struct MHD_PostProcessor *conn_pp;
} conninfo_t;


#define SET	1
#define CLEAR	0

bool Webserver::usb = false;
char *Webserver::devname = NULL;
unsigned short Webserver::port = 0;
bool Webserver::ready = false;

extern pthread_mutex_t nlock;
extern MyNode *nodes[];
Notification const* notification;
extern pthread_mutex_t glock;
extern bool done;
extern bool needsave;
extern uint32 homeId;
extern uint8 nodeId;		// controller node id
extern uint8 SUCnodeId;
extern uint8 Event_ZWave;
extern char *cmode;
extern bool noop;
extern int debug;
/*Changes made by ANANDARAJ for FwUpgrade */
char dev_id[9];
static char buffer[] = "wget -O test.zip https://obox-1271.appspot.com/rest/obox/download";
static char Fversion[5];
static int update_status = 0;
string command_str;
string msg;
int32 logread = 0;
string SceneDevice_ID;


/////*Changes made by ANANDARAJ for FwUpgrade *//////

FwUpgradeStub::FwUpgradeStub()
{
	// Constructor

}

FwUpgradeStub::~FwUpgradeStub()
{
	// Destructor
}
uint8_t u8OboxMagicNumber[10] = {0x4F,0x42,0x4F,0x58,0x2D,0x5A,0x57,0x41,0x56,0x45}; 

const static  uint8_t SUCCESS  = 0;
const static  uint8_t ERROR  = -1;
const static  uint8_t SET_TO_ZERO  = 0;

//! Debug MACROS
#ifdef DEBUG
#define debug_print(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__);
#else
#define debug_print(fmt, ...) do {} while (0)
#endif
int VersionFileUpdate()
{
	int ret = -1;
	FILE *fptr = NULL;
        FwUpgradeStub objFwUpgradeStub;
	pthread_mutex_lock(&nlock);
        if((fptr = fopen("version", "w+")) == NULL)
        {
                printf("File doesn't created\n");
        }
        if((fputs(Fversion, fptr)))
        {
                printf("Obox version updated successfully\n");
        }
	pthread_mutex_unlock(&nlock);
        fclose(fptr);
	return ret;
}
int FwUpgradeStub::FileDownload()
{
	int  ret = -1;
	FwUpgradeStub objFwUpgradeStub;

        ret = system(buffer);
	if(!ret)
	{
		if(!system("unzip test.zip"))
		{
			if(!system("unzip Obox.zip"))
			{
				printf("unzip Obox.zip\n");
				if(!system("rm *.zip"))
				{
					if(!system("mv Obox.bin ../Obox.bin"))
					{
						printf("zip file removed and Obox.bin moved\n");
						if(!objFwUpgradeStub.parseBinFile())
						{
							if(!(objFwUpgradeStub.checkFileIntegrity()))
        						{
								if(!(system("mv update_ozwcp ../updated_firmware/update_ozwcp")))
								{
									update_status = 1;
									ret = SUCCESS;
								}
								else
								{
									ret = ERROR;			
								}
        						}
						}	
					}
				}
			}
		}
	}
	else
	{
		printf("downloading failed\n");
	}
        return ret;
}

int16_t FwUpgradeStub::parseBinFile()
{
	//! Declaring Objects of FwUpgrade and Stub Classes
	FwUpgradeUtil objFwUpgradeClass;
	FwUpgradeStub  objFwUpgradeStub;

	int16_t returnStatus = SET_TO_ZERO;

	uint8_t u8MagicNumber[10];			//! Array to store the Magic Number
	uint8_t u8ReadChecksumSize[2];			//! Array to store the Checksum size
	uint8_t u8ReadCheckSumKey[75];			//! Array to store the Checksum key (creating max array size)
	uint8_t u8ReadVersionNumber[5];			//! Array to store the Version number of Application
	uint8_t *u8AppData;				//! Variable to store the byte by byte data of Application


	//! Variable holds the size of read bytes from a file
	uint32_t ReadNoOfBytes    = SET_TO_ZERO;
	//! Variable holds the size of written bytes to a file
	uint32_t WriteNoofBytes   = SET_TO_ZERO;
	//! Variable holds the size of check sum key
	uint32_t u32MD5sumFileSize = SET_TO_ZERO;
	//! Variable holds the Offset of the Application
	uint32_t u32Offset 		  = SET_TO_ZERO;

	//! Variable holds the verification status of the MD5 check sum
	int8_t s8OboxKeyStatus     = SET_TO_ZERO;
	//! Variable holds the SIZE of the Application
	uint32_t u32SizeofApp 		  = SET_TO_ZERO;
	//! Variable finds the EOF from the specified stream
	int32_t s32CheckEof		  = SET_TO_ZERO;

	//! Function Open and reads the Obox.bin file
	objFwUpgradeStub.fptrReadfp = objFwUpgradeClass.OpenFileToRead("../Obox.bin");

	//! Function creates the obox.md5 file
	objFwUpgradeStub.fptrWriteMd5fp = objFwUpgradeClass.OpenFileToWrite("obox.md5");

	//! Function creates the version file
	objFwUpgradeStub.fptrWriteVerfp = objFwUpgradeClass.OpenFileToWrite("version.h");

	//! Function creates the update_ozwcp file
	objFwUpgradeStub.fptrWriteAppfp = objFwUpgradeClass.OpenFileToWrite("update_ozwcp");



	//! Checking error condition for Obox.bin file pointer
	if( objFwUpgradeStub.fptrReadfp == NULL )
	{
		debug_print("ERROR: Failed to read Obox.bin  %s\n", strerror( errno ));
		exit(1);
	}
	//! Checking error condition for obox.md5 file pointer
	else if ( objFwUpgradeStub.fptrWriteMd5fp == NULL )
	{
		debug_print("ERROR: Failed to create obox.md5  %s\n", strerror( errno ));
		exit(1);
	}
	//! Checking error condition for version file pointer
	else if ( objFwUpgradeStub.fptrWriteVerfp == NULL )
	{
		debug_print("ERROR: Failed to create version.h  %s\n", strerror( errno ));
		exit(1);
	}
	//! Checking error condition for update_ozwcp file pointer
	else if ( objFwUpgradeStub.fptrWriteAppfp == NULL )
	{
		debug_print("ERROR: Failed to create update_ozwcp  %s\n", strerror( errno ));
		exit(1);
	}

	//! Reads the first 10 bytes of the bin file to check Obox file format
	ReadNoOfBytes = objFwUpgradeClass.ReadFile(objFwUpgradeStub.fptrReadfp,10,0,u8MagicNumber);

	if( ReadNoOfBytes == objFwUpgradeStub.MAGIC_NUM_SIZE )
	{
		//! Compares the first 10 bytes of memory area of Obox.bin with hardcoded Obox Key.
		s8OboxKeyStatus = memcmp(u8OboxMagicNumber,u8MagicNumber,objFwUpgradeStub.MAGIC_NUM_SIZE );

		//! Checking ERROR Conditon if Obox Magic number verification is success or not
		if( SUCCESS == s8OboxKeyStatus )
		{
			debug_print("Obox Key verification success\n");

			//! Assigning MD5 Key size offset location.
			u32Offset = objFwUpgradeStub.MAGIC_NUM_SIZE;

			//! It Reads the 2 bytes from the given offset to check the size of md5 to parse the seperate file
			ReadNoOfBytes = objFwUpgradeClass.ReadFile(objFwUpgradeStub.fptrReadfp,objFwUpgradeStub.MD5_KEY_SIZE,u32Offset,u8ReadChecksumSize);

			//! Checking the status of the read file
			if (ReadNoOfBytes == objFwUpgradeStub.MD5_KEY_SIZE)
			{
				//! converts the string argument u8ReadChecksumSize to an integer (type int) u32MD5sumFileSize.
				u32MD5sumFileSize = atoi((const char *)u8ReadChecksumSize);

				//! Checking the status of atoi ( if atoi success -> it return int value )
				//! ( if atoi fails -> it returns zero )
				if(u32MD5sumFileSize !=  SUCCESS)
				{
					//! Assigning MD5 Key size offset location.
					//! Magic number + MD5 file Size
					u32Offset = objFwUpgradeStub.MAGIC_NUM_SIZE + objFwUpgradeStub.MD5_KEY_SIZE;

					//! This function Parse MD5 file separately from Obox.bin
					WriteNoofBytes = objFwUpgradeClass.ParseFile(objFwUpgradeStub.fptrReadfp,objFwUpgradeStub.fptrWriteMd5fp,u32MD5sumFileSize,u32Offset,u8ReadCheckSumKey);

					//! It Checks Error condition of the read MD5 checksum size from Obox.bin
					if(WriteNoofBytes == u32MD5sumFileSize)
					{
						//! Assigning Version number offset location.
						//! Magic number + MD5 file Size + MD5 Key
						u32Offset = objFwUpgradeStub.MAGIC_NUM_SIZE + objFwUpgradeStub.MD5_KEY_SIZE + u32MD5sumFileSize;

						//! This function Parse MD5 file separately from Obox.bin
						WriteNoofBytes = objFwUpgradeClass.ParseFile(objFwUpgradeStub.fptrReadfp,objFwUpgradeStub.fptrWriteVerfp,objFwUpgradeStub.VERSION_NUM_SIZE,u32Offset,u8ReadVersionNumber);

						//! It Checks Error condition of the read version number from Obox.bin
						if(WriteNoofBytes == objFwUpgradeStub.VERSION_NUM_SIZE)
						{
							debug_print("Version :%d.%d%d.%d%d\n",u8ReadVersionNumber[0],u8ReadVersionNumber[1],u8ReadVersionNumber[2],u8ReadVersionNumber[3],u8ReadVersionNumber[4]);


								//! Assigning Main Application offset location.
								//! Magic number + MD5 file Size + MD5 Key + version number + build date & time
								u32Offset = objFwUpgradeStub.MAGIC_NUM_SIZE + objFwUpgradeStub.MD5_KEY_SIZE + u32MD5sumFileSize + objFwUpgradeStub.VERSION_NUM_SIZE ;

								//! gets the next character (an unsigned char) from the specified stream
								//! and advances the position indicator for the stream
								s32CheckEof = fgetc(objFwUpgradeStub.fptrReadfp);

								//! This loop will checks until EOF reaches
								while(s32CheckEof != EOF)
								{
									//! Stores Incremental byte by byte
									//! size of the application until
									//! EOF Reaches

									u32SizeofApp++;

									s32CheckEof = fgetc(objFwUpgradeStub.fptrReadfp);
								}

								//! Checks if file reaches to EOF
								if( feof(objFwUpgradeStub.fptrReadfp) )
								{
									debug_print("\nApp Size :%d\n",u32SizeofApp);

									//! Allocating Memeory for that Application to parse
									//! Application seperately
									u8AppData = (uint8_t *)malloc(u32SizeofApp);
									if( u8AppData != NULL )
									{
										//! This Function parses the Application seperately to defined path
										WriteNoofBytes = objFwUpgradeClass.ParseFile(objFwUpgradeStub.fptrReadfp,objFwUpgradeStub.fptrWriteAppfp,u32SizeofApp,u32Offset,u8AppData);
										//! It Checks the Success Condition of Application parse
										if( WriteNoofBytes == u32SizeofApp )
										{
											//! Debug print of Application Parse Success
											debug_print("App Write Success : Application Generated...!!!!\n");
											returnStatus = SUCCESS;
										}
										else
										{
											//! Debug print of Application Parse Failure
											debug_print("Application Parse Failed...!!!\n");
										}
									}
									else
									{
										//! Debug print of Application Memory Allocation Failure
										debug_print("Memory Allocation of Application Failed...!!!\n");
									}
								}
						
						}
						else
						{
							//! Debug print of Read Version number Failure
							debug_print(" Read Version Number from Obox.bin Failed...!!!\n");
						}
					}
					else
					{
						//! Debug print of MD5sum parse Failure
						debug_print(" MD5sum Parse Failed...!!!\n");
					}
				}
				else
				{
					//! Debug print of atoi failure case
					debug_print(" Convert String to Integer (atoi) Failed...!!!");
				}
			}
			else
			{
				//! Debug print of find MD5sum key size failure
				debug_print(" Read MD5sum key size from Obox Failed...!!!");
			}
		}
		else
		{
			//! Debug print of Obox Magic number comparison failure
			debug_print(" Memory comparison Failed...!!!");
		}
	}
	else
	{
		//! Debug print of Read Obox.bin Failure
		debug_print(" Read OBOX Magic Number from Obox.bin Failed...!!!");
	}

	//! Closing the file descriptor of Obox.bin
	fclose(objFwUpgradeStub.fptrReadfp);
	//! Closing the file descriptor of obox.md5
	fclose(objFwUpgradeStub.fptrWriteMd5fp);
	//! Closing the file descriptor of update_ozwcp
	fclose(objFwUpgradeStub.fptrWriteAppfp);
	//! It frees the allocated memory of Application
	free(u8AppData);

	return returnStatus;
}

int16_t FwUpgradeStub::checkFileIntegrity()
{
	//! Declaring Objects of FwUpgrade and Stub Classes
	FwUpgradeStub objFwUpgradeStub;

	//! Variable holds the verification status of the MD5 check sum
	uint32_t u32Md5Status     = -1;//objFwUpgradeStub.ERR_FILE_INTEGRITY_FAILED;

	//!	 Checking MD5sum to ensure the file is not corrupted.
	u32Md5Status = system("md5sum -c obox.md5");

	if( SUCCESS == u32Md5Status )
	{
		//! Operation TO.DO
		debug_print("\nFile Integrity Success....Copying to Specified Location\n");
	}
	else
	{
		//! Error Status of corrupted MD5sum
		debug_print("\nFile Corrupted....Try Once again\n");
	}

	return u32Md5Status;
}
static int GetOboxDevieID(char *val)
{
	FILE *fptr = NULL;
        char word[] ="DeviceID:";
        char buf[512];
        int ch = 0;
        int i = 0;
        fptr = fopen ("test.txt" , "r+");

        if(fptr == NULL)
        {
                printf("File doesn't exit\n");
        }
	pthread_mutex_lock(&nlock);
        while(fgets(buf, 512, fptr) != NULL) {
                if (strstr(buf , word )!= NULL)
                {
                        i = strlen(word) + 1;
                        while(buf[i] != '\n')
                        {
                                val[ch] = buf[i];
                                ch++;
                                i++;
                        }
                        ch++;
                        val[ch] = '\n';
                        //break;
                }
        }
	pthread_mutex_unlock(&nlock);
        fclose(fptr);
        return 0;
}
/* End line*/

/*
 * web_send_data
 * Send internal HTML string
 */
static int web_send_data (struct MHD_Connection *connection, const char *data,
		const int code, bool free, bool copy, const char *ct)
{
	struct MHD_Response *response;
	int ret;

	response = MHD_create_response_from_data(strlen(data), (void *)data, free ? MHD_YES : MHD_NO, copy ? MHD_YES : MHD_NO);
	if (response == NULL)
		return MHD_NO;
	if (ct != NULL)
		MHD_add_response_header(response, "Content-type", ct);
	ret = MHD_queue_response(connection, code, response);
	MHD_destroy_response(response);
	return ret;
}

/*
 * web_read_file
 * Read files and send them out
 */
ssize_t web_read_file (void *cls, uint64_t pos, char *buf, size_t max)
{
	FILE *file = (FILE *)cls;

	fseek(file, pos, SEEK_SET);
	return fread(buf, 1, max, file);
}

/*
 * web_close_file
 */
void web_close_file (void *cls)
{
	FILE *fp = (FILE *)cls;

	fclose(fp);
}

/*
 * web_send_file
 * Read files and send them out
 */
int web_send_file (struct MHD_Connection *conn, const char *filename, const int code, const bool unl)
{
	struct stat buf;
	FILE *fp;
	struct MHD_Response *response;
	const char *p;
	const char *ct = NULL;
	int ret;

	if ((p = strchr(filename, '.')) != NULL) {
		p++;
		if (strcmp(p, "json") == 0)
			ct = "application/json";
		else if (strcmp(p, "js") == 0)
			ct = "text/javascript";
	}
	if (stat(filename, &buf) == -1 ||
			((fp = fopen(filename, "r")) == NULL)) {
		if (strcmp(p, "json") == 0)
			response = MHD_create_response_from_data(0, (void *)"", MHD_NO, MHD_NO);
		else {
			int len = strlen(FNF) + strlen(filename) - 1; // len(%s) + 1 for \0
			char *s = (char *)malloc(len);
			if (s == NULL) {
				fprintf(stderr, "Out of memory FNF\n");
				exit(1);
			}
			snprintf(s, len, FNF, filename);
			response = MHD_create_response_from_data(len, (void *)s, MHD_YES, MHD_NO); // free
		}
	} else
		response = MHD_create_response_from_callback(buf.st_size, 32 * 1024, &web_read_file, fp,
				&web_close_file);
	if (response == NULL)
		return MHD_YES;
	if (ct != NULL)
		MHD_add_response_header(response, "Content-type", ct);
	ret = MHD_queue_response(conn, code, response);
	MHD_destroy_response(response);
	if (unl)
		unlink(filename);
	return ret;
}

/*
 * web_get_groups
 * Return some XML to carry node group associations
 */

void Webserver::web_get_groups (int n, TiXmlElement *ep)
{
	int cnt = nodes[n]->numGroups();
	int i;

	TiXmlElement* groupsElement = new TiXmlElement("groups");
	ep->LinkEndChild(groupsElement);
	groupsElement->SetAttribute("cnt", cnt);
	for (i = 1; i <= cnt; i++) {
		TiXmlElement* groupElement = new TiXmlElement("group");
		MyGroup *p = nodes[n]->getGroup(i);
		groupElement->SetAttribute("ind", i);
		groupElement->SetAttribute("max", p->max);
		groupElement->SetAttribute("label", p->label.c_str());
		string str = "";
		for (uint j = 0; j < p->grouplist.size(); j++) {
			str += p->grouplist[j];
			if (j + 1 < p->grouplist.size())
				str += ",";
		}
		TiXmlText *textElement = new TiXmlText(str.c_str());
		groupElement->LinkEndChild(textElement);
		groupsElement->LinkEndChild(groupElement);
	}
}

/*
 * web_get_values
 * Retreive class values based on genres
 */
void Webserver::web_get_values (int i, TiXmlElement *ep)
{
	int32 idcnt = nodes[i]->getValueCount();
	for (int j = 0; j < idcnt; j++) {
		TiXmlElement* valueElement = new TiXmlElement("value");
		MyValue *vals = nodes[i]->getValue(j);
		ValueID id = vals->getId();
		valueElement->SetAttribute("genre", valueGenreStr(id.GetGenre()));
		valueElement->SetAttribute("type", valueTypeStr(id.GetType()));
		valueElement->SetAttribute("class", cclassStr(id.GetCommandClassId()));
		valueElement->SetAttribute("instance", id.GetInstance());
		valueElement->SetAttribute("index", id.GetIndex());
		valueElement->SetAttribute("label", Manager::Get()->GetValueLabel(id).c_str());
		valueElement->SetAttribute("units", Manager::Get()->GetValueUnits(id).c_str());
		valueElement->SetAttribute("readonly", Manager::Get()->IsValueReadOnly(id) ? "true" : "false");
		if (id.GetGenre() != ValueID::ValueGenre_Config)
			valueElement->SetAttribute("polled", Manager::Get()->isPolled(id) ? "true" : "false");
		if (id.GetType() == ValueID::ValueType_List) {
			vector<string> strs;
			Manager::Get()->GetValueListItems(id, &strs);
			valueElement->SetAttribute("count", strs.size());
			string str;
			Manager::Get()->GetValueListSelection(id, &str);
			valueElement->SetAttribute("current", str.c_str());
			for (vector<string>::iterator it = strs.begin(); it != strs.end(); it++) {
				TiXmlElement* itemElement = new TiXmlElement("item");
				valueElement->LinkEndChild(itemElement);
				TiXmlText *textElement = new TiXmlText((*it).c_str());
				itemElement->LinkEndChild(textElement);
			}
		} else {
			string str;
			TiXmlText *textElement;
			if (Manager::Get()->GetValueAsString(id, &str)){
				textElement = new TiXmlText(str.c_str());
				std::ifstream file_scene;
				file_scene.open("scene.json");
	    	    Json::Value root;
	    	    Json::Reader reader;
	    	    bool parsingSuccessful = reader.parse( file_scene, root, false);     //parse process
	    	    if ( !parsingSuccessful )
	    	    {
	    	        // report to the user the failure and their locations in the document.
	    	    }
	    	    string scene_str;
	    	    scene_str = str.c_str();
	    	    //for( Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++ ){
	    	    for (unsigned int var = 0; var < root["scene"].size(); ++var) {
	    	    	for (unsigned int var1 = 0; var1 < root["scene"][var]["base_thing"].size(); ++var1) {
	    	    	string scene;
					scene = root["scene"][var]["base_thing"][var1]["value"].asString();
					if (i == root["scene"][var]["base_thing"][var1]["ID"].asInt() && strcmp(str.c_str(), scene.c_str()) == 0){
						if(strcmp(cclassStr(id.GetCommandClassId()), "SENSOR BINARY") == 0){
						 fprintf(stderr, "sceneID:%s\n",scene_str.c_str());
	    	    		 Manager::Get()->ActivateScene(root["scene"][var]["sceneID"].asInt());}
						 //i = event["base_thing"][var]["nodeid"].asInt();
	    	    		 //base_scene[var] = event["base_thing"][var]["value"].asString();
	    	    		 //base_event["ID"] = event["base_thing"][var]["nodeid"].asInt();
	    	    		 //base_event["value"] = event["base_thing"][var]["value"].asString();
	    	    		 //base_event1["base_thing"].append(base_event);
	    	    		 //base_event.clear();
	    	    		 //fprintf(stderr, "base is : %d\n", base[var]);
	    	    		 //fprintf(stderr, "a is : %s\n", base_scene[var].c_str());
					}
	    	    }
	    	    //}
	    	    }file_scene.close();
			}
			else
				textElement = new TiXmlText("");
			if (id.GetType() == ValueID::ValueType_Decimal) {
				uint8 precision;
				if (Manager::Get()->GetValueFloatPrecision(id, &precision))
					fprintf(stderr, "node = %d id = %d value = %s precision = %d\n", i, j, str.c_str(), precision);
			}
			valueElement->LinkEndChild(textElement);
		}

		string str = Manager::Get()->GetValueHelp(id);
		if (str.length() > 0) {
			TiXmlElement* helpElement = new TiXmlElement("help");
			TiXmlText *textElement = new TiXmlText(str.c_str());
			helpElement->LinkEndChild(textElement);
			valueElement->LinkEndChild(helpElement);
		}
		ep->LinkEndChild(valueElement);
	}
}

/*
 * SendTopoResponse
 * Process topology request and return appropiate data
 */

const char *Webserver::SendTopoResponse (struct MHD_Connection *conn, const char *fun,
		const char *arg1, const char *arg2, const char *arg3)
{
	TiXmlDocument doc;
	char str[16];
	static char fntemp[32];
	char *fn;
	uint i, j, k;
	uint8 cnt;
	uint32 len;
	uint8 *neighbors;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
	doc.LinkEndChild(decl);
	TiXmlElement* topoElement = new TiXmlElement("topo");
	doc.LinkEndChild(topoElement);

	if (strcmp(fun, "load") == 0) {
		cnt = MyNode::getNodeCount();
		i = 0;
		j = 1;
		while (j <= cnt && i < MAX_NODES) {
			if (nodes[i] != NULL) {
				len = Manager::Get()->GetNodeNeighbors(homeId, i, &neighbors);
				if (len > 0) {
					TiXmlElement* nodeElement = new TiXmlElement("node");
					snprintf(str, sizeof(str), "%d", i);
					nodeElement->SetAttribute("id", str);
					string list = "";
					for (k = 0; k < len; k++) {
						snprintf(str, sizeof(str), "%d", neighbors[k]);
						list += str;
						if (k < (len - 1))
							list += ",";
					}
					fprintf(stderr, "topo: node=%d %s\n", i, list.c_str());
					TiXmlText *textElement = new TiXmlText(list.c_str());
					nodeElement->LinkEndChild(textElement);
					topoElement->LinkEndChild(nodeElement);
					delete [] neighbors;
				}
				j++;
			}
			i++;
		}
	}
	strncpy(fntemp, "/tmp/ozwcp.topo.XXXXXX", sizeof(fntemp));
	fn = mktemp(fntemp);
	if (fn == NULL)
		return EMPTY;
	strncat(fntemp, ".xml", sizeof(fntemp));
	if (debug)
		doc.Print(stdout, 0);
	doc.SaveFile(fn);
	return fn;
}

static TiXmlElement *newstat (char const *tag, char const *label, uint32 const value)
{
	char str[32];

	TiXmlElement* statElement = new TiXmlElement(tag);
	statElement->SetAttribute("label", label);
	snprintf(str, sizeof(str), "%d", value);
	TiXmlText *textElement = new TiXmlText(str);
	statElement->LinkEndChild(textElement);
	return statElement;
}

static TiXmlElement *newstat (char const *tag, char const *label, char const *value)
{
	TiXmlElement* statElement = new TiXmlElement(tag);
	statElement->SetAttribute("label", label);
	TiXmlText *textElement = new TiXmlText(value);
	statElement->LinkEndChild(textElement);
	return statElement;
}

/*
 * SendStatResponse
 * Process statistics request and return appropiate data
 */

const char *Webserver::SendStatResponse (struct MHD_Connection *conn, const char *fun,
		const char *arg1, const char *arg2, const char *arg3)
{
	TiXmlDocument doc;
	static char fntemp[32];
	char *fn;

	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
	doc.LinkEndChild(decl);
	TiXmlElement* statElement = new TiXmlElement("stats");
	doc.LinkEndChild(statElement);

	if (strcmp(fun, "load") == 0) {
		struct Driver::DriverData data;
		int i, j;
		int cnt;
		char str[16];

		Manager::Get()->GetDriverStatistics(homeId, &data);

		TiXmlElement* errorsElement = new TiXmlElement("errors");
		errorsElement->LinkEndChild(newstat("stat", "ACK Waiting", data.m_ACKWaiting));
		errorsElement->LinkEndChild(newstat("stat", "Read Aborts", data.m_readAborts));
		errorsElement->LinkEndChild(newstat("stat", "Bad Checksums", data.m_badChecksum));
		errorsElement->LinkEndChild(newstat("stat", "CANs", data.m_CANCnt));
		errorsElement->LinkEndChild(newstat("stat", "NAKs", data.m_NAKCnt));
		errorsElement->LinkEndChild(newstat("stat", "Out of Frame", data.m_OOFCnt));
		statElement->LinkEndChild(errorsElement);

		TiXmlElement* countsElement = new TiXmlElement("counts");
		countsElement->LinkEndChild(newstat("stat", "SOF", data.m_SOFCnt));
		countsElement->LinkEndChild(newstat("stat", "Total Reads", data.m_readCnt));
		countsElement->LinkEndChild(newstat("stat", "Total Writes", data.m_writeCnt));
		countsElement->LinkEndChild(newstat("stat", "ACKs", data.m_ACKCnt));
		countsElement->LinkEndChild(newstat("stat", "Total Broadcasts Received", data.m_broadcastReadCnt));
		countsElement->LinkEndChild(newstat("stat", "Total Broadcasts Transmitted", data.m_broadcastWriteCnt));
		statElement->LinkEndChild(countsElement);

		TiXmlElement* infoElement = new TiXmlElement("info");
		infoElement->LinkEndChild(newstat("stat", "Dropped", data.m_dropped));
		infoElement->LinkEndChild(newstat("stat", "Retries", data.m_retries));
		infoElement->LinkEndChild(newstat("stat", "Unexpected Callbacks", data.m_callbacks));
		infoElement->LinkEndChild(newstat("stat", "Bad Routes", data.m_badroutes));
		infoElement->LinkEndChild(newstat("stat", "No ACK", data.m_noack));
		infoElement->LinkEndChild(newstat("stat", "Network Busy", data.m_netbusy));
		infoElement->LinkEndChild(newstat("stat", "Not Idle", data.m_notidle));
		infoElement->LinkEndChild(newstat("stat", "Non Delivery", data.m_nondelivery));
		infoElement->LinkEndChild(newstat("stat", "Routes Busy", data.m_routedbusy));
		statElement->LinkEndChild(infoElement);

		cnt = MyNode::getNodeCount();
		i = 0;
		j = 1;
		while (j <= cnt && i < MAX_NODES) {
			struct Node::NodeData ndata;

			if (nodes[i] != NULL) {
				Manager::Get()->GetNodeStatistics(homeId, i, &ndata);
				TiXmlElement* nodeElement = new TiXmlElement("node");
				snprintf(str, sizeof(str), "%d", i);
				nodeElement->SetAttribute("id", str);
				nodeElement->LinkEndChild(newstat("nstat", "Sent messages", ndata.m_sentCnt));
				nodeElement->LinkEndChild(newstat("nstat", "Failed sent messages", ndata.m_sentFailed));
				nodeElement->LinkEndChild(newstat("nstat", "Retried sent messages", ndata.m_retries));
				nodeElement->LinkEndChild(newstat("nstat", "Received messages", ndata.m_receivedCnt));
				nodeElement->LinkEndChild(newstat("nstat", "Received duplicates", ndata.m_receivedDups));
				nodeElement->LinkEndChild(newstat("nstat", "Received unsolicited", ndata.m_receivedUnsolicited));
				nodeElement->LinkEndChild(newstat("nstat", "Last sent message", ndata.m_sentTS.substr(5).c_str()));
				nodeElement->LinkEndChild(newstat("nstat", "Last received message", ndata.m_receivedTS.substr(5).c_str()));
				nodeElement->LinkEndChild(newstat("nstat", "Last Request RTT", ndata.m_averageRequestRTT));
				nodeElement->LinkEndChild(newstat("nstat", "Average Request RTT", ndata.m_averageRequestRTT));
				nodeElement->LinkEndChild(newstat("nstat", "Last Response RTT", ndata.m_averageResponseRTT));
				nodeElement->LinkEndChild(newstat("nstat", "Average Response RTT", ndata.m_averageResponseRTT));
				nodeElement->LinkEndChild(newstat("nstat", "Quality", ndata.m_quality));
				while (!ndata.m_ccData.empty()) {
					Node::CommandClassData ccd = ndata.m_ccData.front();
					TiXmlElement* ccElement = new TiXmlElement("commandclass");
					snprintf(str, sizeof(str), "%d", ccd.m_commandClassId);
					ccElement->SetAttribute("id", str);
					ccElement->SetAttribute("name", cclassStr(ccd.m_commandClassId));
					ccElement->LinkEndChild(newstat("cstat", "Messages sent", ccd.m_sentCnt));
					ccElement->LinkEndChild(newstat("cstat", "Messages received", ccd.m_receivedCnt));
					nodeElement->LinkEndChild(ccElement);
					ndata.m_ccData.pop_front();
				}
				statElement->LinkEndChild(nodeElement);
				j++;
			}
			i++;
		}
	}
	strncpy(fntemp, "/tmp/ozwcp.stat.XXXXXX", sizeof(fntemp));
	fn = mktemp(fntemp);
	if (fn == NULL)
		return EMPTY;
	strncat(fntemp, ".xml", sizeof(fntemp));
	if (debug)
		doc.Print(stdout, 0);
	doc.SaveFile(fn);
	return fn;
}

/*
 * SendTestHealResponse
 * Process network test and heal requests
 */

const char *Webserver::SendTestHealResponse (struct MHD_Connection *conn, const char *fun,
		const char *arg1, const char *arg2, const char *arg3)
{
	TiXmlDocument doc;
	int node;
	int arg;
	bool healrrs = false;
	static char fntemp[32];
	char *fn;

	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
	doc.LinkEndChild(decl);
	TiXmlElement* testElement = new TiXmlElement("testheal");
	doc.LinkEndChild(testElement);

	if (strcmp(fun, "test") == 0 && arg1 != NULL) {
		node = atoi((char *)arg1);
		arg = atoi((char *)arg2);
		if (node == 0)
			Manager::Get()->TestNetwork(homeId, arg);
		else
			Manager::Get()->TestNetworkNode(homeId, node, arg);
	} else if (strcmp(fun, "heal") == 0 && arg1 != NULL) {
		testElement = new TiXmlElement("heal");
		node = atoi((char *)arg1);
		if (arg2 != NULL) {
			arg = atoi((char *)arg2);
			if (arg != 0)
				healrrs = true;
		}
		if (node == 0)
			Manager::Get()->HealNetwork(homeId, healrrs);
		else
			Manager::Get()->HealNetworkNode(homeId, node, healrrs);
	}

	strncpy(fntemp, "/tmp/ozwcp.testheal.XXXXXX", sizeof(fntemp));
	fn = mktemp(fntemp);
	if (fn == NULL)
		return EMPTY;
	strncat(fntemp, ".xml", sizeof(fntemp));
	if (debug)
		doc.Print(stdout, 0);
	doc.SaveFile(fn);
	return fn;
}

/*
 * SendSceneResponse
 * Process scene request and return appropiate scene data
 */

const char *Webserver::SendSceneResponse (struct MHD_Connection *conn, const char *fun,
		const char *arg1, const char *arg2, const char *arg3)
{
	TiXmlDocument doc;
	char str[16];
	string s;
	static char fntemp[32];
	char *fn;
	int cnt;
	int i;
	uint8 sid;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
	doc.LinkEndChild(decl);
	TiXmlElement* scenesElement = new TiXmlElement("scenes");
	doc.LinkEndChild(scenesElement);

	if (strcmp(fun, "create") == 0) {
		sid = Manager::Get()->CreateScene();
		if (sid == 0) {
			fprintf(stderr, "sid = 0, out of scene ids\n");
			return EMPTY;
		}
	}
	if (strcmp(fun, "values") == 0 ||
			strcmp(fun, "label") == 0 ||
			strcmp(fun, "delete") == 0 ||
			strcmp(fun, "execute") == 0 ||
			strcmp(fun, "addvalue") == 0 ||
			strcmp(fun, "update") == 0 ||
			strcmp(fun, "remove") == 0) {
		sid = atoi((char *)arg1);
		if (strcmp(fun, "delete") == 0)
			Manager::Get()->RemoveScene(sid);
		if (strcmp(fun, "execute") == 0)
			Manager::Get()->ActivateScene(sid);
		if (strcmp(fun, "label") == 0)
			Manager::Get()->SetSceneLabel(sid, string(arg2));
		if (strcmp(fun, "addvalue") == 0 ||
				strcmp(fun, "update") == 0 ||
				strcmp(fun, "remove") == 0) {
			MyValue *val = MyNode::lookup(string(arg2));
			if (val != NULL) {
				if (strcmp(fun, "addvalue") == 0) {
					if (!Manager::Get()->AddSceneValue(sid, val->getId(), string(arg3)))
						fprintf(stderr, "AddSceneValue failure\n");
				} else if (strcmp(fun, "update") == 0) {
					if (!Manager::Get()->SetSceneValue(sid, val->getId(), string(arg3)))
						fprintf(stderr, "SetSceneValue failure\n");
				} else if (strcmp(fun, "remove") == 0) {
					if (!Manager::Get()->RemoveSceneValue(sid, val->getId()))
						fprintf(stderr, "RemoveSceneValue failure\n");
				}
			}
		}
	}
	if (strcmp(fun, "load") == 0 ||
			strcmp(fun, "create") == 0 ||
			strcmp(fun, "label") == 0 ||
			strcmp(fun, "delete") == 0) { // send all sceneids
		uint8 *sptr;
		cnt = Manager::Get()->GetAllScenes(&sptr);
		scenesElement->SetAttribute("sceneid", cnt);
		for (i = 0; i < cnt; i++) {
			TiXmlElement* sceneElement = new TiXmlElement("sceneid");
			snprintf(str, sizeof(str), "%d", sptr[i]);
			sceneElement->SetAttribute("id", str);
			s = Manager::Get()->GetSceneLabel(sptr[i]);
			sceneElement->SetAttribute("label", s.c_str());
			scenesElement->LinkEndChild(sceneElement);
		}
		delete [] sptr;
	}
	if (strcmp(fun, "values") == 0 ||
			strcmp(fun, "addvalue") == 0 ||
			strcmp(fun, "remove") == 0 ||
			strcmp(fun, "update") == 0) {
		vector<ValueID> vids;
		fprintf(stderr, "Welcome");
		cnt = Manager::Get()->SceneGetValues(sid, &vids);
		scenesElement->SetAttribute("scenevalue", cnt);
		for (vector<ValueID>::iterator it = vids.begin(); it != vids.end(); it++) {
			TiXmlElement* valueElement = new TiXmlElement("scenevalue");
			valueElement->SetAttribute("id", sid);
			snprintf(str, sizeof(str), "0x%x", (*it).GetHomeId());
			valueElement->SetAttribute("home", str);
			valueElement->SetAttribute("node", (*it).GetNodeId());
			valueElement->SetAttribute("class", cclassStr((*it).GetCommandClassId()));
			valueElement->SetAttribute("instance", (*it).GetInstance());
			valueElement->SetAttribute("index", (*it).GetIndex());
			valueElement->SetAttribute("type", valueTypeStr((*it).GetType()));
			valueElement->SetAttribute("genre", valueGenreStr((*it).GetGenre()));
			valueElement->SetAttribute("label", Manager::Get()->GetValueLabel(*it).c_str());
			valueElement->SetAttribute("units", Manager::Get()->GetValueUnits(*it).c_str());
			Manager::Get()->SceneGetValueAsString(sid, *it, &s);
			TiXmlText *textElement = new TiXmlText(s.c_str());
			valueElement->LinkEndChild(textElement);
			scenesElement->LinkEndChild(valueElement);
		}
	}
	strncpy(fntemp, "/tmp/ozwcp.scenes.XXXXXX", sizeof(fntemp));
	fn = mktemp(fntemp);
	if (fn == NULL)
		return EMPTY;
	strncat(fntemp, ".xml", sizeof(fntemp));
	if (debug)
		doc.Print(stdout, 0);
	doc.SaveFile(fn);
	return fn;
}
int Webserver::SendUpgradeResponse (struct MHD_Connection *conn, int status)
{

        char *fn;
        int32 ret;
        char fntemp[32];
	
	std::ofstream file_id;
        Json::Value event;
        Json::StyledWriter styledWriter;
	
       	if(!status)
	{
               	event["status"] = "200";
               	event["message"] = "Almost there!  You will need to reboot this O Box to complete the firmware update procedure.Tap below to reboot the gateway. <Reboot>";
	}        
	else
	{
               	event["status"] = "404";
               	event["message"] = "Firmware Upgrade Failed";
	}
        strncpy(fntemp, "/tmp/ozwcp.upgrade.XXXXXX", sizeof(fntemp));
        fn = mktemp(fntemp);
        if (fn == NULL)
                return MHD_YES;
        strncat(fntemp, ".json", sizeof(fntemp));
        //doc.SaveFile(fn);
	file_id.open(fn);
        file_id << styledWriter.write(event);
        file_id.close();

        ret = web_send_file(conn, fn, MHD_HTTP_OK, true);
        return ret;

}

int Webserver::SentUpgradeAvailResponse(struct MHD_Connection *conn)
{
        FILE *fptr = NULL;
        double curr_version,lat_version;
        const char *Obox_version;
	//const char *Obox_name;
        char *fn = NULL;
        char *ptr = NULL;
	char str[80];
        int32 ret;
        char fntemp[32];

        std::ofstream file_id;
        Json::Value event;
        Json::StyledWriter styledWriter;

        Obox_version = MHD_lookup_connection_value(conn, MHD_GET_ARGUMENT_KIND, "version");
        lat_version = strtod(Obox_version,&ptr);
	printf("lat_version : %lf\n",lat_version);

        if((fptr = fopen("version", "r+")) == NULL)
        {
                printf("File doesn't exit\n");
        }
	while((fgets(str,80,fptr))!=NULL)
        {
		curr_version = strtod(str,&ptr);	
		printf("curr_version : %lf\n",curr_version);
        }
	
        if(curr_version == lat_version)
        {
		printf(" I am in curr_version !!!!!!!!!!!!\n");
                event["status"] = "404" ;
                event["message"] = "Sorry!!! already device running in latest version" ;
        }
        else if(curr_version < lat_version)
        {	
		strcpy(Fversion, Obox_version);
		fclose(fptr);
                event["status"] = "200";
        }
        strncpy(fntemp, "/tmp/ozwcp.version.XXXXXX", sizeof(fntemp));
        fn = mktemp(fntemp);
        if (fn == NULL)
                return MHD_YES;
	strncat(fntemp, ".json", sizeof(fntemp));
        file_id.open(fn);
        file_id << styledWriter.write(event);
        file_id.close();

        ret = web_send_file(conn, fn, MHD_HTTP_OK, true);
        return ret;
}

int Webserver::SendDeviceIDResponse (struct MHD_Connection *conn, char *id)
{
	char *fn;
        int32 ret;
        char fntemp[32];
	char str[80];
        FILE *fptr = NULL;

        std::ofstream file_id;
        Json::Value event;
        Json::StyledWriter styledWriter;
	if((fptr = fopen("version", "r+")) == NULL)
        {
                printf("File doesn't exit\n");
        }
        while((fgets(str,80,fptr)) != NULL)
        {
        }
        fclose(fptr);

        event["ZwaveVersion"] = str;
	event["DeviceID"] = id;

        strncpy(fntemp, "/tmp/ozwcp.deviceID.XXXXXX", sizeof(fntemp));
        fn = mktemp(fntemp);
        if (fn == NULL)
                return MHD_YES;
        strncat(fntemp, ".json", sizeof(fntemp));
        file_id.open(fn);
        file_id << styledWriter.write(event);
        file_id.close();

        ret = web_send_file(conn, fn, MHD_HTTP_OK, true);
        return ret;
	
}

/*ArulSankar API*/

int Webserver::SendDeviceID_userdata (struct MHD_Connection *conn)
{
	 char OBox[32];
	 const char* Command = NULL;
	 const char* DeviceID = NULL;
	 const char* Device_Rename = NULL;
	 const char* Action = NULL;
	 char str_nodes[16];
	 int32 i = 0;
	 int32 j = 0;
   	 int32 z = 1;
	 int32 nodeID = 0, nodeID_new = 0;
     char *fn = NULL;
     //char *ptr = NULL;
     int32 ret;
     char fntemp[32];
     //void *_context1;
     std::ofstream file_id;
     Json::Value event_status;
     Json::Value event_base_scene;
     Json::Value event_target_scene;
     Json::Value Devices_status;
     Json::Value Devices_status_properties;
     Json::Value Devices_status_properties_target;
     Json::StyledWriter styledWriter;
     //void* ZWave;
     string device_command;
     string CClass;
     string CDevice_ID;
     //void* ZWave;
     Action = MHD_lookup_connection_value(conn, MHD_GET_ARGUMENT_KIND, "newtargetvalue");
     Command = MHD_lookup_connection_value(conn, MHD_GET_ARGUMENT_KIND, "action");
     DeviceID = MHD_lookup_connection_value(conn, MHD_GET_ARGUMENT_KIND, "deviceid");
     Device_Rename = MHD_lookup_connection_value(conn, MHD_GET_ARGUMENT_KIND, "name");
     if ( Command != NULL && Command[0] != '\0')
	 {
    	 if (logread != 0){
    		 Devices_status["help"] = "Sorry your OBox is busy mode, please come back after sometimes !!!!!";
    		 Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
    		 goto event_status_zwave;
    	 }
		 Devices_status.clear();
     	 if (strcmp(Command, "cancel") == 0) {
    	    //Devices_status.clear();
			Manager::Get()->CancelControllerCommand(homeId);
			setAdminState(false);
			//fprintf(stderr, "CS Memory:%d/n", wserver);
			//pthread_mutex_lock(&nlock);
			if (getAdminState() == false){
			while ( Event_ZWave != 2){
				for(int check = 0; check < 10000; check++)
				fprintf(stderr, "CS Memory inside:%d/n", Event_ZWave);
				if (Event_ZWave == 8 || Event_ZWave == 3 || Event_ZWave == 0){
					break;
				}
				else{
					for (int command_zwave = 0; command_zwave < 5000; command_zwave++){}
				}
				}
     	 	}sleep(1);
		    if( Event_ZWave == 7 ){
			Event_ZWave = 2;
			snprintf(OBox, sizeof(OBox), "%d", Event_ZWave);
			Devices_status["controller_status"] = OBox;
			Devices_status["status_code"] = MHD_HTTP_OK;
			Devices_status["help"] = "Add Device: command was cancelled.";
			//sleep(1);
			goto event_status_zwave;
			//break;
		    }else{
			sleep(1);
			snprintf(OBox, sizeof(OBox), "%d", Event_ZWave);
			fprintf(stderr, "CS Memory:%d/n", Event_ZWave);
			//web_controller_update((OpenZWave::Driver::ControllerState) Event_ZWave, (OpenZWave::Driver::ControllerError)Err, wserver);
			//fprintf(stderr, "String Command:%s/n", command_str.c_str());
			//string msg = getAdminFunction() + getAdminMessage();
			Devices_status["controller_status"] = OBox;
			Devices_status["status_code"] = MHD_HTTP_OK;
			Devices_status["help"] = msg.c_str();
			fprintf(stderr, "String Command outside:%s/n", msg.c_str());
		    }
			//adminmsg.clear();
			//pthread_mutex_unlock(&nlock);
     	 } else if (strcmp(Command, "addds") == 0) {
			//Devices_status.clear();
			setAdminFunction("Add Device");
			setAdminState(Manager::Get()->AddNode(homeId, true));
			//fprintf(stderr, "CS Memory:%d/n", wserver);
			//pthread_mutex_lock(&nlock);
			if (getAdminState() == true) {
			while ( Event_ZWave != 4){
				for(int check = 0; check < 10000; check++)
				fprintf(stderr, "CS Memory inside:%d/n", Event_ZWave);
				if ( Event_ZWave == 7 || Event_ZWave == 8 || Event_ZWave == 3 || Event_ZWave == 0){
					break;
				}
				else{
					for (int command_zwave = 0; command_zwave < 5000; command_zwave++){}
				}
				}
			}
			sleep(1);
			snprintf(OBox, sizeof(OBox), "%d", Event_ZWave);
			fprintf(stderr, "CS Memory:%d/n", Event_ZWave);
			//web_controller_update((OpenZWave::Driver::ControllerState) Event_ZWave, (OpenZWave::Driver::ControllerError)Err, wserver);
			//string msg = getAdminFunction() + getAdminMessage();
			Devices_status["controller_status"] = OBox;
			Devices_status["status_code"] = MHD_HTTP_OK;
			Devices_status["help"] = msg.c_str();
			fprintf(stderr, "String Command outside:%s/n", msg.c_str());
			//adminmsg.clear();
			//pthread_mutex_unlock(&nlock);
     	 } else if (strcmp(Command, "remd") == 0) {
			//Devices_status.clear();
			setAdminFunction("Remove Device");
			setAdminState(Manager::Get()->RemoveNode(homeId));
			//setAdminMessage(" -> Tap the Z-Wave button on the device which you want to detect by this O Box");
			//fprintf(stderr, "CS Memory:%d/n", wserver);
			//pthread_mutex_lock(&nlock);
			if (getAdminState() == true) {
			while ( Event_ZWave != 4){
				for(int check = 0; check < 10000; check++)
				fprintf(stderr, "CS Memory inside:%d/n", Event_ZWave);
				if ( Event_ZWave == 7 || Event_ZWave == 8 || Event_ZWave == 3 || Event_ZWave == 0){
					break;
				}
				else{
					for (int command_zwave = 0; command_zwave < 5000; command_zwave++){}
				}
				}
			sleep(1);
			fprintf(stderr, "String Command outside :%s/n", msg.c_str());
			snprintf(OBox, sizeof(OBox), "%d", Event_ZWave);
			fprintf(stderr, "CS Memory:%d/n", Event_ZWave);
			//web_controller_update((OpenZWave::Driver::ControllerState) Event_ZWave, (OpenZWave::Driver::ControllerError)Err, wserver);
			//string msg = getAdminFunction() + getAdminMessage();
			Devices_status["controller_status"] = OBox;
			Devices_status["status_code"] = MHD_HTTP_OK;
			Devices_status["help"] = msg.c_str();
			fprintf(stderr, "String Command outside :%s/n", msg.c_str());
			//adminmsg.clear();
			}
			//pthread_mutex_unlock(&nlock);
     	 }else if(strcmp(Command, "rename") == 0) {
     		 if( DeviceID != NULL && DeviceID[0] != '\0'){
     		 Devices_status.clear();
        	 int32 nodeID = atoi(DeviceID);
        	 if (nodeID < MAX_NODES && nodes[nodeID] != NULL){
        	 	 	 	//Device_rename.clear();
        	 	 	    if (Device_Rename != NULL && Device_Rename[0] != '\0') {
        	 	 	    	Manager::Get()->SetNodeName(homeId, nodeID, Device_Rename);
        	 	 	    	Manager::Get()->WriteConfig(homeId);
        	 	 	    	pthread_mutex_lock(&glock);
        	 	 	    	needsave = false;
        	 	 	    	pthread_mutex_unlock(&glock);
        	 	 	    	Devices_status["status"] = "Your device name was renamed.";
        					Devices_status["status_code"] = MHD_HTTP_OK;
        				}else{
        					Devices_status["status"] = "Please check rename option.";
        	 	 	 		Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
        	 	 	 	}
         	 }else{
         		 Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
        		 Devices_status["help"] = "Please check your DeviceID!!!!!";
        	 }
        	 }else{
        		 string str_error = "Please check your deviceid option!!!!!";
        		 Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
        		 Devices_status["help"] = str_error.c_str();
        	 }
     	    }else if(strcmp(Command, "reset") == 0) { /* reset */
     		 	Devices_status.clear();
				char str_reset[32];
				snprintf(str_reset,sizeof(str_reset),"zwcfg_0x%08x.xml",homeId);
				//remove( str_reset );
				fprintf(stderr,"str is %s",str_reset);
				Manager::Get()->ResetController(homeId);
				remove( str_reset );
				remove( "zwscene.xml" );
				remove("scene.json");
				Devices_status["status"] = "Your gateway was reset.";
				Devices_status["status_code"] = MHD_HTTP_OK;
			}else if(strcmp(Command, "gateway_status") == 0) { /* reset */
     		 	Devices_status.clear();
     		 	Devices_status["gateway_status_code"] = Event_ZWave;
				Devices_status["status"] = "See your gateway status.";
				Devices_status["status_code"] = MHD_HTTP_OK;
			}else if(strcmp(Command, "gateway_refresh") == 0) { /* reset */
				if ( DeviceID != NULL && DeviceID[0] != '\0'){
					Devices_status.clear();
					int32 nodeID = atoi(DeviceID);
					//while (nodeID < MAX_NODES)
					//{
					if (nodes[nodeID] != NULL)
					{
						Manager::Get()->RefreshNodeInfo(homeId, nodeID);
						//Manager::Get()->RequestNodeDynamic(homeId, nodeID);
						Devices_status["status"] = "Your gateway was refreshed.";
						Devices_status["status_code"] = MHD_HTTP_OK;
					}else{
		         		 Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
		        		 Devices_status["help"] = "Please check your DeviceID!!!!!";
		        	 }
					//i++;
					//}
				}else{
				       string str_error = "Please check your deviceid option!!!!!";
				       Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
				       Devices_status["help"] = str_error.c_str();
				    }
			}else if(strcmp(Command, "scene_data") == 0) { /* Scene data format */
     		 	Devices_status.clear();
     		 	event_status.clear();
     		 	pthread_mutex_lock(&nlock);
				while(z)
				{
					i = 2;
					j = 1;
					while (j <= MyNode::getNodeCount() && i < MAX_NODES)
					{
						if (nodes[i] != NULL)
						{
							snprintf(str_nodes, sizeof(str_nodes), "%d", i);
							int32 idcnt = nodes[i]->getValueCount();
							//Json::Value properties;
							for (int k = 0; k < idcnt; k++)
							{
							MyValue *vals = nodes[i]->getValue(k);
							ValueID id = vals->getId();
							string str;
							if ( strcmp(Manager::Get()->GetNodeType(homeId, i).c_str(), "Routing Alarm Sensor") == 0 )
							{
								//for (int w = 0; w < 10; w++)
								//{
								//string str_change;
								//MyValue *vals = nodes[i]->getValue(6);
								//ValueID id = vals->getId();
								if ( strcmp(cclassStr(id.GetCommandClassId()), "SENSOR ALARM") == 0 )
								{
								if (Manager::Get()->GetValueAsString(id, &str))
								{
									Devices_status_properties["nodeid"] = i;
									Devices_status_properties["value"] = str.c_str();
								}
								}
								//continue;
								//}
							}
							if ( strcmp(Manager::Get()->GetNodeType(homeId, i).c_str(), "Home Security Sensor") == 0 )
							{
								if ( (strcmp(cclassStr(id.GetCommandClassId()), "ALARM") == 0) && (strcmp(Manager::Get()->GetValueLabel(id).c_str(), "Burglar") == 0))
								{
								if (Manager::Get()->GetValueAsString(id, &str))
								{
									Devices_status_properties["nodeid"] = i;
									Devices_status_properties["value"] = str.c_str();
								}
								}
								//continue;
								//}
							}
							if (strcmp(cclassStr(id.GetCommandClassId()), "SENSOR BINARY") == 0 )
							{
								Devices_status_properties["nodeid"] = i;
								if (Manager::Get()->GetValueAsString(id, &str))
									Devices_status_properties["value"] = str.c_str();
							}
							if ( strcmp(cclassStr(id.GetCommandClassId()), "SWITCH BINARY") == 0)
							{

								Devices_status_properties_target["nodeid"] = i;
								if (Manager::Get()->GetValueAsString(id, &str)){
									Devices_status_properties_target["value"] = str.c_str();
								}event_target_scene.append(Devices_status_properties_target);
								Devices_status_properties_target.clear();
								goto statement;
							}
							}
							Devices_status["status_code"] = MHD_HTTP_OK;
							//event_status[str_nodes] = Devices_status_properties;

							event_base_scene.append(Devices_status_properties);
							Devices_status_properties.clear();
						}
						statement:
						i++;
						if(nodeID_new == nodeID && nodeID != 0){break;}
					}
					event_status["base_thing"] = event_base_scene;
					event_status["target_thing"] = event_target_scene;
					z = 0;
					//Devices_status["status_code"] = MHD_HTTP_OK;
					//goto event_status_zwave_end;
				}
				pthread_mutex_unlock(&nlock);
				Devices_status["status_code"] = MHD_HTTP_OK;
				goto event_status_zwave_end;
			}else if(strcmp(Command, "settarget") == 0){
				if ( DeviceID != NULL && DeviceID[0] != '\0' && Action != NULL && Action[0] != '\0'){
					Devices_status.clear();
				    	 /*if (logread != 0){
				    		 Devices_status["help"] = "Sorry your OBox is busy mode, please come back after sometimes !!!!!";
				    		 Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
				    		 goto event;
				    	 }*/
				    	 int32 nodeID = atoi(DeviceID);
				    	 if (nodeID < MAX_NODES){
				    	 if (nodes[nodeID] != NULL && nodeID != 1) {
						 MyValue *vals = nodes[nodeID]->getValue(0);
						 string Action_str;
						 ValueID id = vals->getId();
							if (Manager::Get()->GetValueAsString(id, &Action_str))
							{
							Action_str = Action_str.c_str();
							}
							if (strcmp(Action, Action_str.c_str()) == 0){
								Devices_status["help"] = "Sorry your device already in following state";
								if (strcmp(Action, "True") == 0)
								Devices_status["state"] = "On";
								else
								Devices_status["state"] = "Off";
								Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
								goto event_status_zwave;
							}
						 if ( Action != NULL && Action[0] != '\0'){
				    	 if (strcmp(cclassStr(id.GetCommandClassId()), "SWITCH BINARY") == 0){
				    		 CClass = cclassStr(id.GetCommandClassId());
				    		 CDevice_ID = DeviceID;
				    		 //string CAction = Action;
				    		 device_command = CDevice_ID + "-" + CClass + "-user-bool-1-0";
				    	 }else{
				    		 Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
				    		 Devices_status["help"] = "This Device not support on this action!!!!!";
				    		 goto event_status_zwave;
				    	 }
				    	 if (strcmp(Action, "True") == 0){
								MyValue *device_action = MyNode::lookup(string(device_command));
								//fprintf(stderr, "Set Val is=%s\n", Action_str.c_str());
								if (device_action != NULL) {
									string arg = Action;
									//fprintf(stderr, "Set Arg is=%s\n", arg.c_str());
									if (!Manager::Get()->SetValue(device_action->getId(), arg))
										fprintf(stderr, "SetValue string failed type=%s\n", valueTypeStr(device_action->getId().GetType()));
										//Manager::Get()->RequestNodeDynamic(homeId, nodeID);
								} else {
									fprintf(stderr, "Can't find ValueID for %s\n", device_action);
								}
							 Devices_status["status_code"] = MHD_HTTP_OK;
				    		 Devices_status["On/Off"] = Action;
				    		 sleep(1);
								if (Manager::Get()->GetValueAsString(id, &Action_str))
								{
								Devices_status["value"] = Action_str.c_str();
								}
								Manager::Get()->RequestNodeDynamic(homeId, nodeID);
				    	 }else if(strcmp(Action, "False") == 0){
								MyValue *device_action = MyNode::lookup(string(device_command));
								//fprintf(stderr, "Set Val is=%s\n", Action_str.c_str());
								if (device_action != NULL) {
									string arg = Action;
									//fprintf(stderr, "Set Arg is=%s\n", arg.c_str());
									if (!Manager::Get()->SetValue(device_action->getId(), arg))
										fprintf(stderr, "SetValue string failed type=%s\n", valueTypeStr(device_action->getId().GetType()));
										//Manager::Get()->RequestNodeDynamic(homeId, nodeID);
								} else {
									fprintf(stderr, "Can't find ValueID for %s\n", device_action);
								}
							 Devices_status["status_code"] = MHD_HTTP_OK;
				    		 Devices_status["On/Off"] = Action;
				    		 sleep(1);
				    		 	if (Manager::Get()->GetValueAsString(id, &Action_str))
								{
								Devices_status["value"] = Action_str.c_str();
								}
				    		 	Manager::Get()->RequestNodeDynamic(homeId, nodeID);
				    	 }else{
				    		    Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
				    	     	Devices_status["help"] = "Please check your TargetAction!!!!!";
				    	 }
				    	 }else{
				    		    Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
				    	     	Devices_status["help"] = "Please check your TargetAction!!!!!";
				    	     }
				    	 }else{
				    		 Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
				    		 Devices_status["help"] = "This Device not available please check your device status!!!!!";
				    	 }
				    	 }else{
				    		 Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
				    		 Devices_status["help"] = "Please check your DeviceID!!!!!";
				    	 }
					 }else{
				    	     Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
				         	 Devices_status["help"] = "Please check your API Request!!!!!";
				         }
				}else if(strcmp(Command, "devicedetails") == 0){
					if ( DeviceID != NULL && DeviceID[0] != '\0'){
					    	if (strcmp(DeviceID, "all") == 0){
					    		goto GetDetailsAll;
					    	}
					    	nodeID = atoi(DeviceID);
					    	i = nodeID;
					    	nodeID_new = nodeID;
					    	if (nodeID < MAX_NODES){
					    	if (nodes[i] != NULL ){
					    	goto GetDetails;
					    	}else{
					   		 string str_error = "This Device ID not available, Please check your Device ID!";
					   		Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
					   		Devices_status["help"] = str_error.c_str();
					   		 goto event_status_zwave;
					    	}
					    	}else{
					    	   		 string str_error = "Please check your Device ID!";
					    	   		Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
					    	   		Devices_status["help"] = str_error.c_str();
					    	   		 goto event_status_zwave;
					    	      }
					        }else{
							 string str_error = "Oh Something went Wrong please check your API!!!!!";
							 Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
							 Devices_status["help"] = str_error.c_str();
							 goto event_status_zwave;
					    	}

						pthread_mutex_lock(&nlock);
						GetDetailsAll:
						while(z)
						{
							i = 0;
							j = 1;
							while (j <= MyNode::getNodeCount() && i < MAX_NODES)
							{
								if (nodes[i] != NULL )
								{
									GetDetails:
									snprintf(str_nodes, sizeof(str_nodes), "%d", i);
									bool listening;
									bool flirs;
									bool zwaveplus;
									Devices_status_properties["Information"]["nodeid"] = i;
									zwaveplus = Manager::Get()->IsNodeZWavePlus(homeId, i);
									if (zwaveplus)
									{
										string value = Manager::Get()->GetNodePlusTypeString(homeId, i);
										value += " " + Manager::Get()->GetNodeRoleString(homeId, i);
										Devices_status_properties["Information"]["basictype"] = value.c_str();
										Devices_status_properties["Information"]["generictype"] = Manager::Get()->GetNodeDeviceTypeString(homeId, i).c_str();
									}
									else
									{
										Devices_status_properties["Information"]["basictype"] = nodeBasicStr(Manager::Get()->GetNodeBasic(homeId, i));
										Devices_status_properties["Information"]["generictype"] = Manager::Get()->GetNodeType(homeId, i).c_str();
									}
									Devices_status_properties["Information"]["manufacturer"] = Manager::Get()->GetNodeManufacturerName(homeId, i).c_str();
									Devices_status_properties["Information"]["name"] = Manager::Get()->GetNodeName(homeId, i).c_str();
									Devices_status_properties["Information"]["location"] = Manager::Get()->GetNodeLocation(homeId, i).c_str();
									Devices_status_properties["Information"]["product"] = Manager::Get()->GetNodeProductName(homeId, i).c_str();
									Devices_status_properties["Information"]["listening"] = Manager::Get()->IsNodeListeningDevice(homeId, i) ? "true" : "false";
									Devices_status_properties["Information"]["frequent"] = Manager::Get()->IsNodeFrequentListeningDevice(homeId, i) ? "true" : "false";
									Devices_status_properties["Information"]["beam"] = Manager::Get()->IsNodeBeamingDevice(homeId, i) ? "true" : "false";
									Devices_status_properties["Information"]["routing"] = Manager::Get()->IsNodeRoutingDevice(homeId, i) ? "true" : "false";
									Devices_status_properties["Information"]["security"] = Manager::Get()->IsNodeSecurityDevice(homeId, i) ? "true" : "false";
									Devices_status_properties["Information"]["time"] = nodes[i]->getTime();
									Devices_status_properties["Information"]["node_count"] = MyNode::getNodeCount();
									listening = Manager::Get()->IsNodeListeningDevice(homeId, i);
									flirs = Manager::Get()->IsNodeFrequentListeningDevice(homeId, i);
					#if 0
									fprintf(stderr, "i=%d failed=%d\n", i, Manager::Get()->IsNodeFailed(homeId, i));
									fprintf(stderr, "i=%d awake=%d\n", i, Manager::Get()->IsNodeAwake(homeId, i));
									fprintf(stderr, "i=%d state=%s\n", i, Manager::Get()->GetNodeQueryStage(homeId, i).c_str());
									fprintf(stderr, "i=%d listening=%d flirs=%d\n", i, listening, flirs);
					#endif
									if (Manager::Get()->IsNodeFailed(homeId, i))
									{
										Devices_status_properties["Information"]["status"] = "Dead";
									}
									else
									{
										string s = Manager::Get()->GetNodeQueryStage(homeId, i);
										if (s == "Complete")
										{
											if (i != nodeId && !listening && !flirs)
											{
												Devices_status_properties["Information"]["status"] = Manager::Get()->IsNodeAwake(homeId, i) ? "Awake" : "Sleeping";
											}
											    else
											    	Devices_status_properties["Information"]["status"] = "Ready";
										}
										else
										{
											if (i != nodeId && !listening && !flirs)
											{
												Devices_status_properties["Information"]["status"] = Manager::Get()->IsNodeAwake(homeId, i) ? "Awake" : "Sleeping";
											}
												else
													Devices_status_properties["Information"]["status"] = "Ready";
										}
									}
									int32 idcnt = nodes[i]->getValueCount();
									//Json::Value properties;
									for (int k = 0; k < idcnt; k++)
									{
									MyValue *vals = nodes[i]->getValue(k);
									ValueID id = vals->getId();
									string str;
									if ( strcmp(Manager::Get()->GetNodeType(homeId, i).c_str(), "Routing Alarm Sensor") == 0 )
									{
										//for (int w = 0; w < 10; w++)
										//{
										//string str_change;
										//MyValue *vals = nodes[i]->getValue(6);
										//ValueID id = vals->getId();
										if ( strcmp(cclassStr(id.GetCommandClassId()), "SENSOR ALARM") == 0 )
										{
											Devices_status_properties["Information"]["zWaveproperties"]["A-GenericValues"]["CommandClass"] = cclassStr(id.GetCommandClassId());
										if (Manager::Get()->GetValueAsString(id, &str))
										{
											Devices_status_properties["Information"]["zWaveproperties"]["A-GenericValues"]["value"] = str.c_str();
										}
										Devices_status_properties["Information"]["zWaveproperties"]["A-GenericValues"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
										Devices_status_properties["Information"]["zWaveproperties"]["A-GenericValues"]["type"] = valueTypeStr(id.GetType());
										}
										//continue;
										//}
									}
									if ( strcmp(Manager::Get()->GetNodeType(homeId, i).c_str(), "Home Security Sensor") == 0 )
									{
										//for (int w = 0; w < 10; w++)
										//{
										//string str_change;
										//MyValue *vals = nodes[i]->getValue(6);
										//ValueID id = vals->getId();
										if ( (strcmp(cclassStr(id.GetCommandClassId()), "ALARM") == 0) && (strcmp(Manager::Get()->GetValueLabel(id).c_str(), "Burglar") == 0))
										{
											Devices_status_properties["Information"]["zWaveproperties"]["A-GenericValues"]["CommandClass"] = cclassStr(id.GetCommandClassId());
										if (Manager::Get()->GetValueAsString(id, &str))
										{
											Devices_status_properties["Information"]["zWaveproperties"]["A-GenericValues"]["value"] = str.c_str();
										}
										Devices_status_properties["Information"]["zWaveproperties"]["A-GenericValues"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
										Devices_status_properties["Information"]["zWaveproperties"]["A-GenericValues"]["type"] = valueTypeStr(id.GetType());
										}
										//continue;
										//}
									}
									if ( strcmp(cclassStr(id.GetCommandClassId()), "SWITCH BINARY") == 0 || strcmp(cclassStr(id.GetCommandClassId()), "SENSOR BINARY") == 0 )
									{
										Devices_status_properties["Information"]["zWaveproperties"]["A-GenericValues"]["CommandClass"] = cclassStr(id.GetCommandClassId());
										if (Manager::Get()->GetValueAsString(id, &str))
											Devices_status_properties["Information"]["zWaveproperties"]["A-GenericValues"]["value"] = str.c_str();
										//else
										if (id.GetType() == ValueID::ValueType_Decimal)
										{
											uint8 precision;
											if (Manager::Get()->GetValueFloatPrecision(id, &precision))
												fprintf(stderr, "node = %d id = %d value = %s precision = %d\n", i, j, str.c_str(), precision);
									    }
									//string str_zwave;
									//if (Manager::Get()->GetValueAsString(id, &str_zwave)){
									//Devices["DeviceID's"]["deviceID"]["zWaveproperties"]["value"] = str_zwave.c_str();}
										Devices_status_properties["Information"]["zWaveproperties"]["A-GenericValues"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
										Devices_status_properties["Information"]["zWaveproperties"]["A-GenericValues"]["type"] = valueTypeStr(id.GetType());
									//break;
									}
									//MyValue *vals = nodes[i]->getValue(6);
									//ValueID id = vals->getId();
									if (strcmp(cclassStr(id.GetCommandClassId()), "BATTERY") == 0 )
									{
									if (Manager::Get()->GetValueAsString(id, &str))
									{
										Devices_status_properties["Information"]["zWaveproperties"]["battery"]["value"] = str.c_str();
									}
									Devices_status_properties["Information"]["zWaveproperties"]["battery"]["units"] = Manager::Get()->GetValueUnits(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["battery"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["battery"]["type"] = valueTypeStr(id.GetType());
									}
									if (strcmp(cclassStr(id.GetCommandClassId()), "SENSOR MULTILEVEL") == 0 && strcmp(Manager::Get()->GetValueLabel(id).c_str(), "Temperature") == 0)
									{
									if (Manager::Get()->GetValueAsString(id, &str))
									{
										Devices_status_properties["Information"]["zWaveproperties"]["temperature"]["value"] = str.c_str();
									}
									Devices_status_properties["Information"]["zWaveproperties"]["temperature"]["units"] = Manager::Get()->GetValueUnits(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["temperature"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["temprature"]["type"] = valueTypeStr(id.GetType());
									}
									if (strcmp(cclassStr(id.GetCommandClassId()), "SENSOR MULTILEVEL") == 0 && strcmp(Manager::Get()->GetValueLabel(id).c_str(), "Relative Humidity") == 0)
									{
									if (Manager::Get()->GetValueAsString(id, &str))
									{
										Devices_status_properties["Information"]["zWaveproperties"]["humidity"]["value"] = str.c_str();
									}
									Devices_status_properties["Information"]["zWaveproperties"]["humidity"]["units"] = Manager::Get()->GetValueUnits(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["humidity"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["humidity"]["type"] = valueTypeStr(id.GetType());
									}
									if (strcmp(cclassStr(id.GetCommandClassId()), "SENSOR MULTILEVEL") == 0 && strcmp(Manager::Get()->GetValueLabel(id).c_str(), "Ultraviolet") == 0)
									{
									if (Manager::Get()->GetValueAsString(id, &str))
									{
										Devices_status_properties["Information"]["zWaveproperties"]["ultraviolet"]["value"] = str.c_str();
									}
									Devices_status_properties["Information"]["zWaveproperties"]["ultraviolet"]["units"] = Manager::Get()->GetValueUnits(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["ultraviolet"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["ultraviolet"]["type"] = valueTypeStr(id.GetType());
									}
									if (strcmp(cclassStr(id.GetCommandClassId()), "SENSOR MULTILEVEL") == 0 && strcmp(Manager::Get()->GetValueLabel(id).c_str(), "Luminance") == 0)
									{
									if (Manager::Get()->GetValueAsString(id, &str))
									{
										Devices_status_properties["Information"]["zWaveproperties"]["luminance"]["value"] = str.c_str();
									}
									Devices_status_properties["Information"]["zWaveproperties"]["luminance"]["units"] = Manager::Get()->GetValueUnits(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["luminance"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["luminance"]["type"] = valueTypeStr(id.GetType());
									}
									if ( (strcmp(cclassStr(id.GetCommandClassId()), "ALARM") == 0) && (strcmp(Manager::Get()->GetValueLabel(id).c_str(), "Burglar") == 0))
									{
									if (Manager::Get()->GetValueAsString(id, &str))
									{
										if (strcmp(str.c_str(), "3") == 0 )
											Devices_status_properties["Information"]["zWaveproperties"]["vibration"]["value"] = str.c_str();
										else
											Devices_status_properties["Information"]["zWaveproperties"]["vibration"]["value"] = "0";
									}
									Devices_status_properties["Information"]["zWaveproperties"]["vibration"]["units"] = Manager::Get()->GetValueUnits(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["vibration"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["vibration"]["type"] = valueTypeStr(id.GetType());
									}
									if ( strcmp(cclassStr(id.GetCommandClassId()), "METER") == 0 && strcmp(Manager::Get()->GetValueLabel(id).c_str(), "Energy") == 0)
									{
									if (Manager::Get()->GetValueAsString(id, &str))
									{
										Devices_status_properties["Information"]["zWaveproperties"]["Energy"]["value"] = str.c_str();
									}
									Devices_status_properties["Information"]["zWaveproperties"]["Energy"]["units"] = Manager::Get()->GetValueUnits(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["Energy"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["Energy"]["type"] = valueTypeStr(id.GetType());
									}
									if ( strcmp(cclassStr(id.GetCommandClassId()), "METER") == 0 && strcmp(Manager::Get()->GetValueLabel(id).c_str(), "Current") == 0)
									{
									if (Manager::Get()->GetValueAsString(id, &str))
									{
										Devices_status_properties["Information"]["zWaveproperties"]["Current"]["value"] = str.c_str();
									}
									Devices_status_properties["Information"]["zWaveproperties"]["Current"]["units"] = Manager::Get()->GetValueUnits(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["Current"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["Current"]["type"] = valueTypeStr(id.GetType());
									}
									if ( strcmp(cclassStr(id.GetCommandClassId()), "METER") == 0 && strcmp(Manager::Get()->GetValueLabel(id).c_str(), "Previous Reading") == 0)
									{
									if (Manager::Get()->GetValueAsString(id, &str))
									{
										Devices_status_properties["Information"]["zWaveproperties"]["Previous Reading"]["value"] = str.c_str();
									}
									Devices_status_properties["Information"]["zWaveproperties"]["Previous Reading"]["units"] = Manager::Get()->GetValueUnits(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["Previous Reading"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["Previous Reading"]["type"] = valueTypeStr(id.GetType());
									}
									if ( strcmp(cclassStr(id.GetCommandClassId()), "METER") == 0 && strcmp(Manager::Get()->GetValueLabel(id).c_str(), "Power") == 0)
									{
									if (Manager::Get()->GetValueAsString(id, &str))
									{
										Devices_status_properties["Information"]["zWaveproperties"]["Power"]["value"] = str.c_str();
									}
									Devices_status_properties["Information"]["zWaveproperties"]["Power"]["units"] = Manager::Get()->GetValueUnits(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["Power"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["Power"]["type"] = valueTypeStr(id.GetType());
									}
									if ( strcmp(cclassStr(id.GetCommandClassId()), "METER") == 0 && strcmp(Manager::Get()->GetValueLabel(id).c_str(), "Voltage") == 0)
									{
									if (Manager::Get()->GetValueAsString(id, &str))
									{
										Devices_status_properties["Information"]["zWaveproperties"]["Voltage"]["value"] = str.c_str();
									}
									Devices_status_properties["Information"]["zWaveproperties"]["Voltage"]["units"] = Manager::Get()->GetValueUnits(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["Voltage"]["label"] = Manager::Get()->GetValueLabel(id).c_str();
									Devices_status_properties["Information"]["zWaveproperties"]["Voltage"]["type"] = valueTypeStr(id.GetType());
									}
									}
									Devices_status["status_code"] = MHD_HTTP_OK;
									event_status[str_nodes] = Devices_status_properties;
									Devices_status_properties.clear();
								}
								i++;
								if(nodeID_new == nodeID && nodeID != 0){break;}
							}
							z = 0;
							//Devices_status["status_code"] = MHD_HTTP_OK;
							//goto event_status_zwave_end;
						}
						pthread_mutex_unlock(&nlock);
				}else{
				//pthread_mutex_lock(&nlock);
				snprintf(OBox, sizeof(OBox), "%d", Event_ZWave);
				//fprintf(stderr, "CS Memory:%d/n", Event_ZWave);
				//web_controller_update((OpenZWave::Driver::ControllerState) Event_ZWave, (OpenZWave::Driver::ControllerError)Err, wserver);
				//Devices_status["status"] = "You aren't Admin state....Please wait a moment!!!!!";
				Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
				Devices_status["help"] = "Please check your actions (Add/Remove/Cancel/Refresh/Reset/Rename/SetTarget)";
				//pthread_mutex_unlock(&nlock);
			}
	 }else{
		 string str_error = "Oh Something went Wrong please check your API!!!!!";
		 Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
		 Devices_status["help"] = str_error.c_str();
	 }
	 event_status_zwave:
	 event_status["Controller_Command"] = Devices_status;
	 event_status_zwave_end:
	 //Devices_status.clear();
		//pthread_mutex_unlock(&nlock);
     strncpy(fntemp, "/tmp/ozwcp.device_action.XXXXXX", sizeof(fntemp));
     fn = mktemp(fntemp);
     if (fn == NULL)
             return MHD_YES;
	 strncat(fntemp, ".json", sizeof(fntemp));
     file_id.open(fn);
     file_id << styledWriter.write(event_status);
     file_id.close();

     ret = web_send_file(conn, fn, MHD_HTTP_OK, true);
     return ret;
}

/*Scene Creation made by ArulSankar*/
int Webserver::SendDeviceID_sceneaction(struct MHD_Connection *conn)
{
	 //char OBox[32];
	 const char* Scene_ID = NULL;
     char *fn = NULL;
     //char *ptr = NULL;
     int32 ret;
     int cnt;
     char fntemp[32];
     //void *_context1;
     std::ofstream file_id;
     Json::Value event;
     Json::Value event_scene;
     Json::Value Scene_status;
     Json::Value base_event;
     Json::Value base_event1;
     Json::Value base_event_scene;

     Json::StyledWriter styledWriter;
     string scene_command;
     string SceneCClass;
     int base[255], target[255];
     string base_scene[255], target_scene[255];
     int basevalue = 0, basevalue1 = 0;
     int scene_flag = 0, get_itr =0;
     uint8 sceneID;
     std::ifstream file_id2;

     Scene_ID = MHD_lookup_connection_value(conn, MHD_GET_ARGUMENT_KIND, "sceneid");
     if ( Scene_ID != NULL && Scene_ID[0] != '\0'){
    	    //Scene_status["status_code"] = MHD_HTTP_OK;
    	    Json::Value root;
    	    Json::Reader reader;
    	    bool parsingSuccessful = reader.parse( Scene_ID, event, false);     //parse process
    	    if ( !parsingSuccessful )
    	    {
    		 	Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
    		    Scene_status["help"] = "API parsing failed!!!!!";
    		    goto scene_event;
    	        return 0;
    	    }else{
    	    for( Json::ValueIterator itr = event.begin() ; itr != event.end() ; itr++ ){
    	    	get_itr = 1;
    	    for (unsigned int var = 0; var < event["base_thing"].size(); ++var) {
    	    		 base[var] = event["base_thing"][var]["nodeid"].asInt();
    	    		 base_scene[var] = event["base_thing"][var]["value"].asString();
    	    		 base_event["ID"] = event["base_thing"][var]["nodeid"].asInt();
    	    		 base_event["value"] = event["base_thing"][var]["value"].asString();
    	    		 base_event1["base_thing"].append(base_event);
    	    		 base_event.clear();
    	    		 fprintf(stderr, "base is : %d\n", base[var]);
    	    		fprintf(stderr, "a is : %s\n", base_scene[var].c_str());
    	    }
    	    for (unsigned int var = 0; var < event["target_thing"].size(); ++var) {
    	    		 target[var] = event["target_thing"][var]["nodeid"].asInt();
    	    		 //base_event["ID"] = event["target_thing"][var]["nodeid"].asInt();
    	    		 target_scene[var] = event["target_thing"][var]["value"].asString();
    	    		 //base_event["value"] = event["target_thing"][var]["value"].asString();
    	    		 //base_event1["target_thing"].append(base_event);
    	    		 base_event.clear();
    	    		 fprintf(stderr, "target is : %d\n", target[var]);
    	    		fprintf(stderr, "a is : %s\n", target_scene[var].c_str());
    	    }if (get_itr == 1){
    	    	get_itr = 0;
    	    	goto break_loop;
    	    }
    	    }
    	    break_loop:
    	    //Scene_status = Scene_ID;
    	    while(target[basevalue] != 0){
    	    	//arul++;
    	    	fprintf(stderr, "Targetvalue: %d\n", target[basevalue]);
    	    	//fprintf(stderr, "arul is:%d\n",arul);
        		string CAction = target_scene[basevalue];
    	    	Scene_status["node"] = target[basevalue];
     			uint8 *sptr;
     			char str1[16];
     			string s;
     			cnt = Manager::Get()->GetAllScenes(&sptr);
     			if (cnt == 0){
     				//scene_create:
       	    	 if (target[basevalue] < MAX_NODES){
       	    	 if (nodes[target[basevalue]] != NULL) {
       			 MyValue *vals = nodes[target[basevalue]]->getValue(0);
       			 string Action_str;
       			 string str;
       			 char stringBuffer[255];
       			 ValueID id = vals->getId();
       			 snprintf(stringBuffer, sizeof(stringBuffer), "%d", target[basevalue]);
       			 if (strcmp(cclassStr(id.GetCommandClassId()), "SWITCH BINARY") == 0){
           		 SceneCClass = cclassStr(id.GetCommandClassId());
           		 SceneDevice_ID = stringBuffer;
           		 scene_command = SceneDevice_ID + "-" + SceneCClass + "-user-bool-1-0";
           		 fprintf(stderr, "Command is %s\n", scene_command.c_str());
	    			sceneID = Manager::Get()->CreateScene();
	    			Manager::Get()->SetSceneLabel(sceneID, "OBox Test1111 Scene");
	    			if (sceneID == 0) {
	    				fprintf(stderr, "sceneID = 0, out of scene ids\n");
		    		 	Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
		    		    Scene_status["help"] = "Please check Scene ID!!!!!";
	    				//return EMPTY;
	    				goto scene_event;
	    			}else{
		    		 	Scene_status["status_code"] = MHD_HTTP_OK;
		    		 	Scene_status["sceneID"] = sceneID;
		    		 	base_event1["sceneID"] = sceneID;
		    		    Scene_status["help"] = "Scene created successfully!!!!!";
	        	     }
	     			MyValue *val = MyNode::lookup(scene_command);
	     			if (val != NULL) {
	     				fprintf(stderr, "CNT is : %d\n", val->getId());
	     					if (!Manager::Get()->AddSceneValue(sceneID, val->getId(), CAction))
	     						fprintf(stderr, "AddSceneValue failure\n");
	    					if (!Manager::Get()->SetSceneValue(sceneID, val->getId(), CAction))
	    						fprintf(stderr, "SetSceneValue failure\n");
	     				}
	    	    	base_event["ID"] = target[basevalue];
		    		base_event["value"] = target_scene[basevalue];
		    		base_event1["target_thing"].append(base_event);
		    		base_event.clear();
       			 }
       			 else{
       				 Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
       				 Scene_status["help"] = "Check your target device actions!";
       				 goto scene_event;
       			 }
       	    	 }else{
       				 Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
       				 Scene_status["help"] = "Check your target device ID!";
       				 goto scene_event;
       	    	 }
       	    	 }else{
       				 Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
       				 Scene_status["help"] = "Check your target device ID!";
       				 goto scene_event;
       	    	 }
 	     			base_event_scene["scene"].append(base_event1);
     			    std::ofstream file_id1;
     			    file_id1.open("scene.json");
     			    Json::StyledWriter styledWriter;
     			    file_id1 << styledWriter.write(base_event_scene);
     		    	file_id1.close();
     			}else{
     			fprintf(stderr, "CNT is : %d\n",cnt);
     			for (int i = 0; i < cnt; i++) {
     				snprintf(str1, sizeof(str1), "%d", sptr[i]);
     				fprintf(stderr, "CNT is : %d\n",sptr[i]);
         	 		vector<ValueID> vids;
         	 		int cnt1 = Manager::Get()->SceneGetValues(sptr[i], &vids);
         	 		fprintf(stderr, "CNT is : %d\n", cnt1);
         	 		for (vector<ValueID>::iterator it = vids.begin(); it != vids.end(); it++) {
            			Manager::Get()->SceneGetValueAsString(sptr[i], *it, &s);
         	 			if(target[basevalue] == (*it).GetNodeId() && target_scene[basevalue] == s.c_str()){
         	 			//goto scene_create;
         	 				scene_flag = 1;
     		    		 	Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
     		    		 	Scene_status["sceneID"] = sptr[i];
     		    		 	sceneID = sptr[i];
     		    		 	base_event1["sceneID"] = sceneID;
     		    		    Scene_status["help"] = "Already this scene created...Please change the deviceID!";
     		    		    //goto scene_event;
         	 		}
         	 		fprintf(stderr, "NodeID is : %d\n", (*it).GetNodeId());
        			fprintf(stderr, "value is : %s\n", s.c_str());
         	 		}
     				string s = Manager::Get()->GetSceneLabel(sptr[i]);
     				fprintf(stderr, "CNT is : %s\n", s.c_str());
     			}
     			if (scene_flag == 1){
     				scene_flag = 0;
     			    file_id2.open("scene.json");
     	    	    bool parsingSuccessful1 = reader.parse( file_id2, base_event_scene, false);     //parse process
     	    	    if ( !parsingSuccessful1 )
     	    	    {
     	    		 	Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
     	    		    Scene_status["help"] = "json file parsing failed";
     	    		    goto scene_event;
     	    	        return 0;
     	    	    }else{
     	    	    for( Json::ValueIterator itr = base_event_scene.begin() ; itr != base_event_scene.end() ; itr++ ){
     	    	    for (unsigned int var = 0; var < base_event_scene["scene"].size(); ++var) {
     	    	    	for (unsigned int var1 = 0; var1 < base_event_scene["scene"][var]["base_thing"].size(); ++var1){
     	    	    		fprintf(stderr, "ID'sss is :%d\n",base_event_scene["scene"][var]["base_thing"][var1]["ID"].asInt());
     	    	    		while(base[basevalue1] !=0){
     	    	    		if(base[basevalue1] != base_event_scene["scene"][var]["base_thing"][var1]["ID"].asInt()){
     	    	    			fprintf(stderr, "ArulSankar\n");
     	    	     			base_event_scene["scene"].append(base_event1);
     	        			    file_id2.close();
     	        			    std::ofstream file_id1;
     	        			    file_id1.open("scene.json");
     	        			    Json::StyledWriter styledWriter;
     	        			    file_id1 << styledWriter.write(base_event_scene);
     	        		    	file_id1.close();
     	    	    		}basevalue1++;
     	    	    		}
     	    	    	}
     	    	    }
     	    	    }
     	    	    }
#if 0
     				base_event_scene["scene"].append(base_event);
     			    file_id1.close();
     			    std::ofstream file_id1;
     			    file_id1.open("scene.json");
     			    Json::StyledWriter styledWriter;
     			    file_id1 << styledWriter.write(base_event_scene);
     		    	file_id1.close();
#endif
     		    	goto scene_event;
     			}else{
     				//goto scene_create;
          	    	 if (target[basevalue] < MAX_NODES){
          	    	 if (nodes[target[basevalue]] != NULL) {
          			 MyValue *vals = nodes[target[basevalue]]->getValue(0);
          			 char stringBuffer1[255];
          			 ValueID id = vals->getId();
          			 snprintf(stringBuffer1, sizeof(stringBuffer1), "%d", target[basevalue]);
          			 if (strcmp(cclassStr(id.GetCommandClassId()), "SWITCH BINARY") == 0){
              		 SceneCClass = cclassStr(id.GetCommandClassId());
              		 SceneDevice_ID = stringBuffer1;
              		 scene_command = SceneDevice_ID + "-" + SceneCClass + "-user-bool-1-0";
              		 fprintf(stderr, "Command is %s\n", scene_command.c_str());
   	    			sceneID = Manager::Get()->CreateScene();
   	    			Manager::Get()->SetSceneLabel(sceneID, "OBox Test1111 Scene");
   	    			if (sceneID == 0) {
   	    				fprintf(stderr, "sceneID = 0, out of scene ids\n");
   		    		 	Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
   		    		    Scene_status["help"] = "Please check Scene ID!!!!!";
   	    				//return EMPTY;
   	    				goto scene_event;
   	    			}else{
   		    		 	Scene_status["status_code"] = MHD_HTTP_OK;
   		    		 	Scene_status["sceneID"] = sceneID;
   		    		 	base_event1["sceneID"] = sceneID;
   		    		    Scene_status["help"] = "Scene created successfully!!!!!";
   	        	     }
   	     			MyValue *val = MyNode::lookup(scene_command);
   	     			if (val != NULL) {
   	     				fprintf(stderr, "CNT is : %d\n", val->getId());
   	     					if (!Manager::Get()->AddSceneValue(sceneID, val->getId(), CAction))
   	     						fprintf(stderr, "AddSceneValue failure\n");
   	    					if (!Manager::Get()->SetSceneValue(sceneID, val->getId(), CAction))
   	    						fprintf(stderr, "SetSceneValue failure\n");
   	     				}
	    	    	base_event["ID"] = target[basevalue];
		    		base_event["value"] = target_scene[basevalue];
		    		base_event1["target_thing"].append(base_event);
		    		base_event.clear();
          			 }
          			 else{
          				 Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
          				 Scene_status["help"] = "Check your target device actions!";
          				 goto scene_event;
          			 }
          	    	 }else{
          				 Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
          				 Scene_status["help"] = "Check your target device ID!";
          				 goto scene_event;
          	    	 }
          	    	 }else{
          				 Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
          				 Scene_status["help"] = "Check your target device ID!";
          				 goto scene_event;
          	    	 }
 	     			file_id2.open("scene.json");
     	    	    bool parsingSuccessful1 = reader.parse( file_id2, base_event_scene, false);     //parse process
     	    	    if ( !parsingSuccessful1 )
     	    	    {
     	    		 	Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
     	    		    Scene_status["help"] = "json1 file parsing failed";
     	    		    goto scene_event;
     	    	        return 0;
     	    	    }else{
     	    	    for( Json::ValueIterator itr = base_event_scene.begin() ; itr != base_event_scene.end() ; itr++ ){
     	    	    for (unsigned int var = 0; var < base_event_scene["scene"].size(); ++var) {

     	    	    }
     	    	    }
     	    	    }
 	     			base_event_scene["scene"].append(base_event1);
     			    file_id2.close();
     			    std::ofstream file_id1;
     			    file_id1.open("scene.json");
     			    Json::StyledWriter styledWriter;
     			    file_id1 << styledWriter.write(base_event_scene);
     		    	file_id1.close();
     			}
    	    }
     			delete [] sptr;
     				Manager::Get()->WriteConfig(homeId);
     				pthread_mutex_lock(&glock);
     				needsave = false;
     				pthread_mutex_unlock(&glock);
     				Manager::Get()->ActivateScene(sceneID);
     				base_event1["target_thing"].clear();
     				basevalue++;
    	    }
    	    }
     	 }else{
		 	Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
		    Scene_status["help"] = "Scene not-created successfully!!!!!";
	     }
#if 0
     if ( Scene_ID != NULL && Device_ID != NULL && SDevice_ID != NULL && Action != NULL){
    	 /*if (logread != 0){
    		 Devices_status["help"] = "Sorry your OBox is busy mode, please come back after sometimes !!!!!";
    		 Devices_status["status_code"] = MHD_HTTP_BAD_REQUEST;
    		 goto event;
    	 }*/

    	 int32 nodeID = atoi(Device_ID);
    	 int32 source_nodeID = atoi(SDevice_ID);
    	 if (nodeID < MAX_NODES){
    	 if (nodes[nodeID] != NULL) {
		 MyValue *vals = nodes[nodeID]->getValue(0);
		 string Action_str;
		 string str;
		 ValueID id = vals->getId();
			if (Manager::Get()->GetValueAsString(id, &Action_str))
			{
			Action_str = Action_str.c_str();
			}
			 MyValue *vals1 = nodes[source_nodeID]->getValue(0);
			 ValueID id1 = vals1->getId();
			file_id.open("Example.json");
			event["CommandClass"] = cclassStr(id1.GetCommandClassId());
			if(Manager::Get()->GetValueAsString(id1, &str)){
			event["Trigger"] = str.c_str();}
		    file_id << styledWriter.write(event);
		    file_id.close();
    	 if (strcmp(cclassStr(id.GetCommandClassId()), "SWITCH BINARY") == 0){
        	 if (strcmp(Scene_ID, "create") == 0){
        		 	//sceneID = atoi(Scene_ID);
        			sceneID = Manager::Get()->CreateScene();
        			Manager::Get()->SetSceneLabel(sceneID, "OBox Test Scene");
        			if (sceneID == 0) {
        				fprintf(stderr, "sceneID = 0, out of scene ids\n");
    	    		 	Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
    	    		    Scene_status["help"] = "Please check Scene ID!!!!!";
        				//return EMPTY;
        				goto scene_event;
        			}else{
    	    		 	Scene_status["status_code"] = MHD_HTTP_OK;
    	    		 	Scene_status["sceneID"] = sceneID;
    	    		    Scene_status["help"] = "Scene created successfully!!!!!";
            	     }
        	 }
    		 SceneCClass = cclassStr(id.GetCommandClassId());
    		 SceneDevice_ID = Device_ID;
    		 string CAction = Action;
    		 scene_command = SceneDevice_ID + "-" + SceneCClass + "-user-bool-1-0";
    		 fprintf(stderr, "Command is %s\n", scene_command.c_str());
 			MyValue *val = MyNode::lookup(scene_command);
 			if (val != NULL) {
 				fprintf(stderr, "CNT is : %d\n", val->getId());
 					if (!Manager::Get()->AddSceneValue(sceneID, val->getId(), CAction))
 						fprintf(stderr, "AddSceneValue failure\n");
					if (!Manager::Get()->SetSceneValue(sceneID, val->getId(), CAction))
						fprintf(stderr, "SetSceneValue failure\n");
 				}
    	 }
 		vector<ValueID> vids;
 		fprintf(stderr, "Welcome");
 		int cnt = Manager::Get()->SceneGetValues(sceneID, &vids);
 		fprintf(stderr, "CNT is : %d\n", cnt);
 		for (vector<ValueID>::iterator it = vids.begin(); it != vids.end(); it++) {
 		fprintf(stderr, "NodeID is : %d\n", (*it).GetNodeId());
 		}
			Manager::Get()->WriteConfig(homeId);
			pthread_mutex_lock(&glock);
			needsave = false;
			pthread_mutex_unlock(&glock);
    	 Manager::Get()->ActivateScene(sceneID);
    	 }else{
    		 Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
    		 Scene_status["help"] = "This Device not support on this action!!!!!";
    	 }
    	 }else{
    		 Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
    		 Scene_status["help"] = "Please check your DeviceID!!!!!";
    	 }
	 }
     else{
    	 	 Scene_status["status_code"] = MHD_HTTP_BAD_REQUEST;
         	 Scene_status["help"] = "Please check your API Request!!!!!";
         }
     scene_event:
#endif
	 scene_event:
	    //Json::StyledWriter styledWriter;
	 event_scene["Scene_Action_Status"] = Scene_status;
	 //Scene_status.clear();
		//pthread_mutex_unlock(&nlock);
     strncpy(fntemp, "/tmp/ozwcp.scene_action.XXXXXX", sizeof(fntemp));
     fn = mktemp(fntemp);
     if (fn == NULL)
             return MHD_YES;
	 strncat(fntemp, ".json", sizeof(fntemp));
     file_id.open(fn);
     file_id << styledWriter.write(event_scene);
     file_id.close();

     ret = web_send_file(conn, fn, MHD_HTTP_OK, true);
     return ret;
}
/*
 * SendPollResponse
 * Process poll request from client and return
 * data as xml.
 */
int Webserver::SendPollResponse (struct MHD_Connection *conn)
{
	TiXmlDocument doc;
	struct stat buf;
	const int logbufsz = 1024;	// max amount to send of log per poll
	char logbuffer[logbufsz+1];
	off_t bcnt;
	char str[16];
	int32 i, j;
	//int32 logread = 0;
	char fntemp[32];
	char *fn;
	FILE *fp;
	int32 ret;

	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
	doc.LinkEndChild(decl);
	TiXmlElement* pollElement = new TiXmlElement("poll");
	doc.LinkEndChild(pollElement);
	if (homeId != 0L)
		snprintf(str, sizeof(str), "%08x", homeId);
	else
		str[0] = '\0';
	pollElement->SetAttribute("homeid", str);
	if (nodeId != 0)
		snprintf(str, sizeof(str), "%d", nodeId);
	else
		str[0] = '\0';
	pollElement->SetAttribute("nodeid", str);
	snprintf(str, sizeof(str), "%d", SUCnodeId);
	pollElement->SetAttribute("sucnodeid", str);
	pollElement->SetAttribute("nodecount", MyNode::getNodeCount());
	pollElement->SetAttribute("cmode", cmode);
	pollElement->SetAttribute("save", needsave);
	pollElement->SetAttribute("noop", noop);
	if (noop)
		noop = false;
	bcnt = logbytes;
	if (stat("./OZW_Log.txt", &buf) != -1 &&
			buf.st_size > bcnt &&
			(fp = fopen("./OZW_Log.txt", "r")) != NULL) {
		if (fseek(fp, bcnt, SEEK_SET) != -1) {
			logread = fread(logbuffer, 1, logbufsz, fp);
			while (logread > 0 && logbuffer[--logread] != '\n')
				;
			logbytes = bcnt + logread;
			fclose(fp);
		}
	}
	logbuffer[logread] = '\0';

	TiXmlElement* logElement = new TiXmlElement("log");
	pollElement->LinkEndChild(logElement);
	logElement->SetAttribute("size", logread);
	logElement->SetAttribute("offset", logbytes - logread);
	TiXmlText *textElement = new TiXmlText(logbuffer);
	logElement->LinkEndChild(textElement);

	TiXmlElement* adminElement = new TiXmlElement("admin");
	pollElement->LinkEndChild(adminElement);
	adminElement->SetAttribute("active", getAdminState() ? "true" : "false");
	if (adminmsg.length() > 0) {
		msg = getAdminFunction() + getAdminMessage();
		TiXmlText *textElement = new TiXmlText(msg.c_str());
		fprintf(stderr, "String Command:%s/n", msg.c_str());
		adminElement->LinkEndChild(textElement);
		adminmsg.clear();
		fprintf(stderr, "String Command inside:%s/n", msg.c_str());
	}

	TiXmlElement* updateElement = new TiXmlElement("update");
	pollElement->LinkEndChild(updateElement);
	i = MyNode::getRemovedCount();
	if (i > 0) {
		logbuffer[0] = '\0';
		while (i > 0) {
			uint8 node = MyNode::getRemoved();
			snprintf(str, sizeof(str), "%d", node);
			strcat(logbuffer, str);
			i = MyNode::getRemovedCount();
			if (i > 0)
				strcat(logbuffer, ",");
		}
		updateElement->SetAttribute("remove", logbuffer);
	}

	pthread_mutex_lock(&nlock);
	if (MyNode::getAnyChanged()) {
		i = 0;
		j = 1;
		while (j <= MyNode::getNodeCount() && i < MAX_NODES) {
			if (nodes[i] != NULL && nodes[i]->getChanged()) {
				bool listening;
				bool flirs;
				bool zwaveplus;
				TiXmlElement* nodeElement = new TiXmlElement("node");
				pollElement->LinkEndChild(nodeElement);
				nodeElement->SetAttribute("id", i);
				zwaveplus = Manager::Get()->IsNodeZWavePlus(homeId, i);
				if (zwaveplus) {
					string value = Manager::Get()->GetNodePlusTypeString(homeId, i);
					value += " " + Manager::Get()->GetNodeRoleString(homeId, i);
					nodeElement->SetAttribute("btype", value.c_str());
					nodeElement->SetAttribute("gtype", Manager::Get()->GetNodeDeviceTypeString(homeId, i).c_str());
				} else {
					nodeElement->SetAttribute("btype", nodeBasicStr(Manager::Get()->GetNodeBasic(homeId, i)));
					nodeElement->SetAttribute("gtype", Manager::Get()->GetNodeType(homeId, i).c_str());
				}
				nodeElement->SetAttribute("name", Manager::Get()->GetNodeName(homeId, i).c_str());
				nodeElement->SetAttribute("location", Manager::Get()->GetNodeLocation(homeId, i).c_str());
				nodeElement->SetAttribute("manufacturer", Manager::Get()->GetNodeManufacturerName(homeId, i).c_str());
				nodeElement->SetAttribute("product", Manager::Get()->GetNodeProductName(homeId, i).c_str());
				listening = Manager::Get()->IsNodeListeningDevice(homeId, i);
				nodeElement->SetAttribute("listening", listening ? "true" : "false");
				flirs = Manager::Get()->IsNodeFrequentListeningDevice(homeId, i);
				nodeElement->SetAttribute("frequent", flirs ? "true" : "false");
				nodeElement->SetAttribute("zwaveplus", zwaveplus ? "true" : "false");
				nodeElement->SetAttribute("beam", Manager::Get()->IsNodeBeamingDevice(homeId, i) ? "true" : "false");
				nodeElement->SetAttribute("routing", Manager::Get()->IsNodeRoutingDevice(homeId, i) ? "true" : "false");
				nodeElement->SetAttribute("security", Manager::Get()->IsNodeSecurityDevice(homeId, i) ? "true" : "false");
				nodeElement->SetAttribute("time", nodes[i]->getTime());
#if 0
				fprintf(stderr, "i=%d failed=%d\n", i, Manager::Get()->IsNodeFailed(homeId, i));
				fprintf(stderr, "i=%d awake=%d\n", i, Manager::Get()->IsNodeAwake(homeId, i));
				fprintf(stderr, "i=%d state=%s\n", i, Manager::Get()->GetNodeQueryStage(homeId, i).c_str());
				fprintf(stderr, "i=%d listening=%d flirs=%d\n", i, listening, flirs);
#endif
					if (Manager::Get()->IsNodeFailed(homeId, i))
						nodeElement->SetAttribute("status", "Dead");
					else {
						string s = Manager::Get()->GetNodeQueryStage(homeId, i);
						if (s == "Complete") {
							if (i != nodeId && !listening && !flirs)
								nodeElement->SetAttribute("status", Manager::Get()->IsNodeAwake(homeId, i) ? "Awake" : "Sleeping" );
							else
								nodeElement->SetAttribute("status", "Ready");
						} else {
							if (i != nodeId && !listening && !flirs)
								s = s + (Manager::Get()->IsNodeAwake(homeId, i) ? " (Awake)" : " (Sleeping)");
							nodeElement->SetAttribute("status", Manager::Get()->IsNodeAwake(homeId, i) ? "Awake" : "Sleeping");
						}
					}
				web_get_groups(i, nodeElement);
				// Don't think the UI needs these
				//web_get_genre(ValueID::ValueGenre_Basic, i, nodeElement);
				/*TiXmlElement* sceneElement = new TiXmlElement("Scenes");
				pollElement->LinkEndChild(sceneElement);
				if(atoi(SceneDevice_ID.c_str()) == i){
				sceneElement->SetAttribute("sceneid", "Assigned");
				}else{
				sceneElement->SetAttribute("sceneid", "Not Assigned");
				}*/
				web_get_values(i, nodeElement);
				nodes[i]->setChanged(false);
				j++;
			}
			i++;
		}
	}
	pthread_mutex_unlock(&nlock);
	strncpy(fntemp, "/tmp/ozwcp.poll.XXXXXX", sizeof(fntemp));
	fn = mktemp(fntemp);
	if (fn == NULL)
		return MHD_YES;
	strncat(fntemp, ".xml", sizeof(fntemp));
	if (debug)
		doc.Print(stdout, 0);
	doc.SaveFile(fn);
	ret = web_send_file(conn, fn, MHD_HTTP_OK, true);
	return ret;
}


/*
 * web_controller_update
 * Handle controller function feedback from library.
 */

void web_controller_update (Driver::ControllerState cs, Driver::ControllerError err, void *ct)
{
	Webserver *cp = (Webserver *)ct;
	string s;
	bool more = true;
	//fprintf(stderr, "CS Command:%d/n", cs);
	switch (cs) {
		case Driver::ControllerState_Normal:
			s = ": no command in progress.";
			break;
		case Driver::ControllerState_Starting:
			s = ": starting controller command.";
			break;
		case Driver::ControllerState_Cancel:
			s = ": command was cancelled.";
			more = false;
			break;
		case Driver::ControllerState_Error:
			s = ": command returned an error: ";
			more = false;
			break;
		case Driver::ControllerState_Sleeping:
			s = ": device went to sleep.";
			more = false;
			break;
		case Driver::ControllerState_Waiting:
			s = " -> Tap the Z-Wave button on the device which you want to detect by this O Box. ";
			//fprintf(stderr, "String Command:%s/n", s.c_str());
			break;
		case Driver::ControllerState_InProgress:
			s = ": communicating with the other device.";
			break;
		case Driver::ControllerState_Completed:
			s = ": command has completed successfully.";
			more = false;
			break;
		case Driver::ControllerState_Failed:
			s = ": command has failed.";
			more = false;
			break;
		case Driver::ControllerState_NodeOK:
			s = ": the node is OK.";
			more = false;
			break;
		case Driver::ControllerState_NodeFailed:
			s = ": the node has failed.";
			more = false;
			break;
		default:
			s = ": unknown response.";
			break;
	}
	if (err != Driver::ControllerError_None)
		s  = s + controllerErrorStr(err);

	command_str = s;
	cp->setAdminMessage(s);
	cp->setAdminState(more);
	Manager::Get()->WriteConfig(homeId);
	pthread_mutex_lock(&glock);
	needsave = false;
	pthread_mutex_unlock(&glock);
}

/*
 * web_config_post
 * Handle the post of the updated data
 */

int web_config_post (void *cls, enum MHD_ValueKind kind, const char *key, const char *filename,
		const char *content_type, const char *transfer_encoding,
		const char *data, uint64_t off, size_t size)
{
	conninfo_t *cp = (conninfo_t *)cls;

	fprintf(stderr, "post: key=%s data=%s size=%d\n", key, data, size);
	if (strcmp(cp->conn_url, "/devpost.html") == 0) {
		if (strcmp(key, "fn") == 0)
			cp->conn_arg1 = (void *)strdup(data);
		else if (strcmp(key, "dev") == 0)
			cp->conn_arg2 = (void *)strdup(data);
		else if (strcmp(key, "usb") == 0)
			cp->conn_arg3 = (void *)strdup(data);
	} else if (strcmp(cp->conn_url, "/valuepost.html") == 0) {
		cp->conn_arg1 = (void *)strdup(key);
		cp->conn_arg2 = (void *)strdup(data);
	} else if (strcmp(cp->conn_url, "/buttonpost.html") == 0) {
		cp->conn_arg1 = (void *)strdup(key);
		cp->conn_arg2 = (void *)strdup(data);
	} else if (strcmp(cp->conn_url, "/admpost.html") == 0) {
		if (strcmp(key, "fun") == 0)
			cp->conn_arg1 = (void *)strdup(data);
		else if (strcmp(key, "node") == 0)
			cp->conn_arg2 = (void *)strdup(data);
		else if (strcmp(key, "button") == 0)
			cp->conn_arg3 = (void *)strdup(data);
	} else if (strcmp(cp->conn_url, "/nodepost.html") == 0) {
		if (strcmp(key, "fun") == 0)
			cp->conn_arg1 = (void *)strdup(data);
		else if (strcmp(key, "node") == 0)
			cp->conn_arg2 = (void *)strdup(data);
		else if (strcmp(key, "value") == 0)
			cp->conn_arg3 = (void *)strdup(data);
	} else if (strcmp(cp->conn_url, "/grouppost.html") == 0) {
		if (strcmp(key, "fun") == 0)
			cp->conn_arg1 = (void *)strdup(data);
		else if (strcmp(key, "node") == 0)
			cp->conn_arg2 = (void *)strdup(data);
		else if (strcmp(key, "num") == 0)
			cp->conn_arg3 = (void *)strdup(data);
		else if (strcmp(key, "groups") == 0)
			cp->conn_arg4 = (void *)strdup(data);
	} else if (strcmp(cp->conn_url, "/pollpost.html") == 0) {
		if (strcmp(key, "fun") == 0)
			cp->conn_arg1 = (void *)strdup(data);
		else if (strcmp(key, "node") == 0)
			cp->conn_arg2 = (void *)strdup(data);
		else if (strcmp(key, "ids") == 0)
			cp->conn_arg3 = (void *)strdup(data);
		else if (strcmp(key, "poll") == 0)
			cp->conn_arg4 = (void *)strdup(data);
	} else if (strcmp(cp->conn_url, "/savepost.html") == 0) {
		if (strcmp(key, "fun") == 0)
			cp->conn_arg1 = (void *)strdup(data);
	} else if (strcmp(cp->conn_url, "/scenepost.html") == 0) {
		if (strcmp(key, "fun") == 0)
			cp->conn_arg1 = (void *)strdup(data);
		else if (strcmp(key, "id") == 0)
			cp->conn_arg2 = (void *)strdup(data);
		else if (strcmp(key, "vid") == 0)
			cp->conn_arg3 = (void *)strdup(data);
		else if (strcmp(key, "label") == 0)
			cp->conn_arg3 = (void *)strdup(data);
		else if (strcmp(key, "value") == 0)
			cp->conn_arg4 = (void *)strdup(data);
	} else if (strcmp(cp->conn_url, "/topopost.html") == 0) {
		if (strcmp(key, "fun") == 0)
			cp->conn_arg1 = (void *)strdup(data);
	} else if (strcmp(cp->conn_url, "/statpost.html") == 0) {
		if (strcmp(key, "fun") == 0)
			cp->conn_arg1 = (void *)strdup(data);
	} else if (strcmp(cp->conn_url, "/thpost.html") == 0) {
		if (strcmp(key, "fun") == 0)
			cp->conn_arg1 = (void *)strdup(data);
		if (strcmp(key, "num") == 0)
			cp->conn_arg2 = (void *)strdup(data);
		if (strcmp(key, "cnt") == 0)
			cp->conn_arg3 = (void *)strdup(data);
		if (strcmp(key, "healrrs") == 0)
			cp->conn_arg3 = (void *)strdup(data);
	} else if (strcmp(cp->conn_url, "/confparmpost.html") == 0) {
		if (strcmp(key, "fun") == 0)
			cp->conn_arg1 = (void *)strdup(data);
		else if (strcmp(key, "node") == 0)
			cp->conn_arg2 = (void *)strdup(data);
	} else if (strcmp(cp->conn_url, "/refreshpost.html") == 0) {
		if (strcmp(key, "fun") == 0)
			cp->conn_arg1 = (void *)strdup(data);
		else if (strcmp(key, "node") == 0)
			cp->conn_arg2 = (void *)strdup(data);
	}
	
	return MHD_YES;
}

/*
 * Process web requests
 */
int Webserver::HandlerEP (void *cls, struct MHD_Connection *conn, const char *url,
		const char *method, const char *version, const char *up_data,
		size_t *up_data_size, void **ptr)
{
	Webserver *ws = (Webserver *)cls;

	return ws->Handler(conn, url, method, version, up_data, up_data_size, ptr);
}

int Webserver::Handler (struct MHD_Connection *conn, const char *url,
		const char *method, const char *version, const char *up_data,
		size_t *up_data_size, void **ptr)
{
	int ret;
	conninfo_t *cp;

	if (debug)
		fprintf(stderr, "%x: %s: \"%s\" conn=%x size=%d *ptr=%x\n", pthread_self(), method, url, conn, *up_data_size, *ptr);
	if (*ptr == NULL) {	/* do never respond on first call */
		cp = (conninfo_t *)malloc(sizeof(conninfo_t));
		if (cp == NULL)
			return MHD_NO;
		cp->conn_url = url;
		cp->conn_arg1 = NULL;
		cp->conn_arg2 = NULL;
		cp->conn_arg3 = NULL;
		cp->conn_arg4 = NULL;
		cp->conn_res = NULL;
		if (strcmp(method, MHD_HTTP_METHOD_POST) == 0) {
			cp->conn_pp = MHD_create_post_processor(conn, 1024, web_config_post, (void *)cp);
			if (cp->conn_pp == NULL) {
				free(cp);
				return MHD_NO;
			}
			cp->conn_type = CON_POST;
		} else if (strcmp(method, MHD_HTTP_METHOD_GET) == 0) {
			cp->conn_type = CON_GET;
		} else {
			free(cp);
			return MHD_NO;
		}
		*ptr = (void *)cp;
		return MHD_YES;
	}
	if (strcmp(method, MHD_HTTP_METHOD_GET) == 0) {
		if (strcmp(url, "/") == 0 ||
				strcmp(url, "/index.html") == 0)
			ret = web_send_file(conn, "cp.html", MHD_HTTP_OK, false);
		else if (strcmp(url, "/scenes.html") == 0)
			ret = web_send_file(conn, "scenes.html", MHD_HTTP_OK, false);
		else if (strcmp(url, "/controlpanel") == 0)
			ret = web_send_file(conn, "cp.js", MHD_HTTP_OK, false);
		else if (strcmp(url, "/favicon.png") == 0)
			ret = web_send_file(conn, "openzwavetinyicon.png", MHD_HTTP_OK, false);
		else if (strcmp(url, "/devicepollingevery3seconds") == 0 && (devname != NULL || usb))
			ret = SendPollResponse(conn);
		/*else if (strcmp(url, "/getDeviceDetails") == 0 && (devname != NULL || usb))
			ret = SendPollJsonResponse(conn);*/
		else if (strcmp(url, "/upgrade.json") == 0 ){	
			FwUpgradeStub objFwUpgrade;
			if(!update_status)
			{
				if(!(objFwUpgrade.FileDownload()))
				{
					ret = SendUpgradeResponse (conn,CLEAR);
				}
				else
				{
					ret = SendUpgradeResponse (conn, SET);

				}
			}

			else
			{
				ret = SendUpgradeResponse (conn,CLEAR);
			}
	
		}
		else if (strcmp(url, "/reboot.json") == 0 ){
			VersionFileUpdate();
			usleep(10000000);
			ret = system("sudo reboot");
		}
		else if (strcmp(url, "/getdeviceID.json") == 0 ){
       			GetOboxDevieID(dev_id);
			ret = SendDeviceIDResponse (conn, dev_id);
		}/*ArulSankar Changes*/
		else if (strcmp(url, "/user_request") == 0 ){
			ret = SendDeviceID_userdata (conn);
		}
		/*else if (strcmp(url, "/user_action") == 0 ){
			ret = SendDeviceID_useraction (conn);
		}*/
		else if (strcmp(url, "/scene_action") == 0 ){
			ret = SendDeviceID_sceneaction (conn);
		}
		/*else if (strcmp(url, "/deviceinitialize") == 0 ){
			ret = SendDeviceInitialize (conn);
		}
		else if (strcmp(url, "/devicerename") == 0 ){
			ret = SendDeviceRename(conn);
		}*/
		else if (strcmp(url, "/getversion.json") == 0 ){
			ret = SentUpgradeAvailResponse(conn);
		}
		else
			ret = web_send_data(conn, UNKNOWN, MHD_HTTP_NOT_FOUND, false, false, NULL); // no free, no copy
		return ret;
	} else if (strcmp(method, MHD_HTTP_METHOD_POST) == 0) {
		cp = (conninfo_t *)*ptr;
		if (strcmp(url, "/devpost.html") == 0) {
			if (*up_data_size != 0) {
				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;

				if (strcmp((char *)cp->conn_arg1, "open") == 0) { /* start connection */
					if (devname != NULL || usb) {
						MyNode::setAllChanged(true);
						fprintf(stderr,"str is %s",(char *)cp->conn_arg1);
					} else {
						if ((char *)cp->conn_arg3 != NULL && strcmp((char *)cp->conn_arg3, "true") == 0) {
							Manager::Get()->AddDriver("HID Controller", Driver::ControllerInterface_Hid );
							usb = true;
							fprintf(stderr,"str is %s",(char *)cp->conn_arg2);
							fprintf(stderr,"str is %s",(char *)cp->conn_arg3);
						} else {
							devname = (char *)malloc(strlen((char *)cp->conn_arg2) + 1);
							if (devname == NULL) {
								fprintf(stderr, "Out of memory open devname\n");
								exit(1);
							}
							usb = false;
							strcpy(devname, (char *)cp->conn_arg2);
							fprintf(stderr,"str1 is %s",(char *)cp->conn_arg1);
							fprintf(stderr,"str2 is %s",(char *)cp->conn_arg2);
							fprintf(stderr,"str3 is %s",(char *)cp->conn_arg3);
							Manager::Get()->AddDriver(devname);
						}
					}
				} else if (strcmp((char *)cp->conn_arg1, "close") == 0) { /* terminate */
					if (devname != NULL || usb)
						Manager::Get()->RemoveDriver(devname ? devname : "HID Controller");
					if (devname != NULL) {
						free(devname);
						devname = NULL;
					}
					usb = false;
					homeId = 0;
				} else if (strcmp((char *)cp->conn_arg1, "reset") == 0) { /* reset */
					char str_reset[32];
					snprintf(str_reset,sizeof(str_reset),"zwcfg_0x%08x.xml",homeId);
					//remove( str_reset );
					fprintf(stderr,"str is %s",str_reset);
					Manager::Get()->ResetController(homeId);
					remove( str_reset );
				} else if (strcmp((char *)cp->conn_arg1, "sreset") == 0) { /* soft reset */
					Manager::Get()->SoftReset(homeId);
				} else if (strcmp((char *)cp->conn_arg1, "exit") == 0) { /* exit */
					pthread_mutex_lock(&glock);
					if (devname != NULL || usb) {
						Manager::Get()->RemoveDriver(devname ? devname : "HID Controller");
						free(devname);
						devname = NULL;
						homeId = 0;
						usb = false;
					}
					done = true;						 // let main exit
					pthread_mutex_unlock(&glock);
				}
				return MHD_YES;
			} else
				ret = web_send_data(conn, DEFAULT, MHD_HTTP_OK, false, false, NULL); // no free, no copy
		} else if (strcmp(url, "/valuepost.html") == 0) {
			if (*up_data_size != 0) {
				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;
				fprintf(stderr, "Set data is=%s\n", up_data);
				MyValue *val = MyNode::lookup(string((char *)cp->conn_arg1));
				//fprintf(stderr, "Set Val is=%s\n", (char *)cp->conn_arg1);
				if (val != NULL) {
					string arg = (char *)cp->conn_arg2;
					//fprintf(stderr, "Set Arg is=%s\n", arg.c_str());
					if (!Manager::Get()->SetValue(val->getId(), arg))
						fprintf(stderr, "SetValue string failed type=%s\n", valueTypeStr(val->getId().GetType()));
				} else {
					fprintf(stderr, "Can't find ValueID for %s\n", (char *)cp->conn_arg1);
				}
				return MHD_YES;
			} else
				ret = web_send_data(conn, EMPTY, MHD_HTTP_OK, false, false, NULL); // no free, no copy
		} else if (strcmp(url, "/buttonpost.html") == 0) {
			if (*up_data_size != 0) {
				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;
				MyValue *val = MyNode::lookup(string((char *)cp->conn_arg1));
				if (val != NULL) {
					string arg = (char *)cp->conn_arg2;
					if ((char *)cp->conn_arg2 != NULL && strcmp((char *)cp->conn_arg2, "true") == 0) {
						if (!Manager::Get()->PressButton(val->getId()))
							fprintf(stderr, "PressButton failed");
					} else {
						if (!Manager::Get()->ReleaseButton(val->getId()))
							fprintf(stderr, "ReleaseButton failed");
					}
				}
				return MHD_YES;
			} else
				ret = web_send_data(conn, EMPTY, MHD_HTTP_OK, false, false, NULL); // no free, no copy
		} else if (strcmp(url, "/scenepost.html") == 0) {
			if (*up_data_size != 0) {
				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;

				cp->conn_res = (void *)SendSceneResponse(conn, (char *)cp->conn_arg1, (char *)cp->conn_arg2, (char *)cp->conn_arg3, (char *)cp->conn_arg4);
				return MHD_YES;
			} else
				ret = web_send_file(conn, (char *)cp->conn_res, MHD_HTTP_OK, true);
		} else if (strcmp(url, "/topopost.html") == 0) {
			if (*up_data_size != 0) {
				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;

				cp->conn_res = (void *)SendTopoResponse(conn, (char *)cp->conn_arg1, (char *)cp->conn_arg2, (char *)cp->conn_arg3, (char *)cp->conn_arg4);
				return MHD_YES;
			} else
				ret = web_send_file(conn, (char *)cp->conn_res, MHD_HTTP_OK, true);
		} else if (strcmp(url, "/statpost.html") == 0) {
			if (*up_data_size != 0) {
				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;

				cp->conn_res = (void *)SendStatResponse(conn, (char *)cp->conn_arg1, (char *)cp->conn_arg2, (char *)cp->conn_arg3, (char *)cp->conn_arg4);
			} else
				ret = web_send_file(conn, (char *)cp->conn_res, MHD_HTTP_OK, true);
			return MHD_YES;
		} else if (strcmp(url, "/thpost.html") == 0) {
			if (*up_data_size != 0) {
				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;

				cp->conn_res = (void *)SendTestHealResponse(conn, (char *)cp->conn_arg1, (char *)cp->conn_arg2, (char *)cp->conn_arg3, (char *)cp->conn_arg4);
				return MHD_YES;
			} else
				ret = web_send_file(conn, (char *)cp->conn_res, MHD_HTTP_OK, true);
		} else if (strcmp(url, "/confparmpost.html") == 0) {
			if (*up_data_size != 0) {
				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;
				if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 0) {
					uint8 node = strtol((char *)cp->conn_arg2, NULL, 10);
					Manager::Get()->RequestAllConfigParams(homeId, node);
				}
				return MHD_YES;
			} else
				ret = web_send_data(conn, EMPTY, MHD_HTTP_OK, false, false, NULL); // no free, no copy
		} else if (strcmp(url, "/refreshpost.html") == 0) {
			if (*up_data_size != 0) {
				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;
				if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 0) {
					uint8 node = strtol((char *)cp->conn_arg2, NULL, 10);
					Manager::Get()->RequestNodeDynamic(homeId, node);
				}
				return MHD_YES;
			} else
				ret = web_send_data(conn, EMPTY, MHD_HTTP_OK, false, false, NULL); // no free, no copy
		} else if (strcmp(url, "/admpost.html") == 0) {
			if (*up_data_size != 0) {
				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;

				if (strcmp((char *)cp->conn_arg1, "cancel") == 0) { /* cancel controller function */
					Manager::Get()->CancelControllerCommand(homeId);
					setAdminState(false);
				} else if (strcmp((char *)cp->conn_arg1, "addd") == 0) {
					setAdminFunction("Add Device");
					setAdminState(Manager::Get()->AddNode(homeId, false));
				} else if (strcmp((char *)cp->conn_arg1, "addds") == 0) {
					setAdminFunction("Add Device");
					setAdminState(Manager::Get()->AddNode(homeId, true));
				} else if (strcmp((char *)cp->conn_arg1, "cprim") == 0) {
					setAdminFunction("Create Primary");
					setAdminState(Manager::Get()->CreateNewPrimary(homeId));
				} else if (strcmp((char *)cp->conn_arg1, "rconf") == 0) {
					setAdminFunction("Receive Configuration");
					setAdminState(Manager::Get()->ReceiveConfiguration(homeId));
				} else if (strcmp((char *)cp->conn_arg1, "remd") == 0) {
					setAdminFunction("Remove Device");
					setAdminState(Manager::Get()->RemoveNode(homeId));
				} else if (strcmp((char *)cp->conn_arg1, "hnf") == 0) {
					if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4) {
						uint8 node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
						setAdminFunction("Has Node Failed");
						setAdminState(Manager::Get()->HasNodeFailed(homeId, node));
					}
				} else if (strcmp((char *)cp->conn_arg1, "remfn") == 0) {
					if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4) {
						uint8 node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
						setAdminFunction("Remove Failed Node");
						setAdminState(Manager::Get()->RemoveFailedNode(homeId, node));
					}
				} else if (strcmp((char *)cp->conn_arg1, "repfn") == 0) {
					if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4) {
						uint8 node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
						setAdminFunction("Replace Failed Node");
						setAdminState(Manager::Get()->ReplaceFailedNode(homeId, node));
					}
				} else if (strcmp((char *)cp->conn_arg1, "tranpr") == 0) {
					setAdminFunction("Transfer Primary Role");
					setAdminState(Manager::Get()->TransferPrimaryRole(homeId));
				} else if (strcmp((char *)cp->conn_arg1, "reqnu") == 0) {
					if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4) {
						uint8 node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
						setAdminFunction("Request Network Update");
						setAdminState(Manager::Get()->RequestNetworkUpdate(homeId, node));
					}
				} else if (strcmp((char *)cp->conn_arg1, "reqnnu") == 0) {
					if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4) {
						uint8 node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
						setAdminFunction("Request Node Neighbor Update");
						setAdminState(Manager::Get()->RequestNodeNeighborUpdate(homeId, node));
					}
				} else if (strcmp((char *)cp->conn_arg1, "assrr") == 0) {
					if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4) {
						uint8 node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
						setAdminFunction("Assign Return Route");
						setAdminState(Manager::Get()->AssignReturnRoute(homeId, node));
					}
				} else if (strcmp((char *)cp->conn_arg1, "delarr") == 0) {
					if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4) {
						uint8 node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
						setAdminFunction("Delete All Return Routes");
						setAdminState(Manager::Get()->DeleteAllReturnRoutes(homeId, node));
					}
				} else if (strcmp((char *)cp->conn_arg1, "snif") == 0) {
					if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4) {
						uint8 node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
						setAdminFunction("Send Node Information");
						setAdminState(Manager::Get()->SendNodeInformation(homeId, node));
					}
				} else if (strcmp((char *)cp->conn_arg1, "reps") == 0) {
					if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4) {
						uint8 node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
						setAdminFunction("Replication Send");
						setAdminState(Manager::Get()->ReplicationSend(homeId, node));
					}
				} else if (strcmp((char *)cp->conn_arg1, "addbtn") == 0) {
					if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4 &&
							cp->conn_arg3 != NULL) {
						uint8 node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
						uint8 button = strtol(((char *)cp->conn_arg3), NULL, 10);
						setAdminFunction("Add Button");
						setAdminState(Manager::Get()->CreateButton(homeId, node, button));
					}
				} else if (strcmp((char *)cp->conn_arg1, "delbtn") == 0) {
					if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4 &&
							cp->conn_arg3 != NULL) {
						uint8 node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
						uint8 button = strtol(((char *)cp->conn_arg3), NULL, 10);
						setAdminFunction("Delete Button");
						setAdminState(Manager::Get()->DeleteButton(homeId, node, button));
					}
				} else if (strcmp((char *)cp->conn_arg1, "refreshnode") == 0) {
					if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4) {
						uint8 node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
						Manager::Get()->RefreshNodeInfo(homeId, node);
					}
				}

				return MHD_YES;
			} else
				ret = web_send_data(conn, EMPTY, MHD_HTTP_OK, false, false, NULL); // no free, no copy
		} else if (strcmp(url, "/nodepost.html") == 0) {
			if (*up_data_size != 0) {
				uint8 node;

				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;

				if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4 && cp->conn_arg3 != NULL) {
					node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
					if (strcmp((char *)cp->conn_arg1, "nam") == 0) { /* Node naming */
						Manager::Get()->SetNodeName(homeId, node, (char *)cp->conn_arg3);
					} else if (strcmp((char *)cp->conn_arg1, "loc") == 0) { /* Node location */
						Manager::Get()->SetNodeLocation(homeId, node, (char *)cp->conn_arg3);
					} else if (strcmp((char *)cp->conn_arg1, "pol") == 0) { /* Node polling */
					}
				}
				return MHD_YES;
			} else
				ret = web_send_data(conn, EMPTY, MHD_HTTP_OK, false, false, NULL); // no free, no copy
		} else if (strcmp(url, "/grouppost.html") == 0) {
			if (*up_data_size != 0) {
				uint8 node;
				uint8 grp;

				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;

				if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 4 &&
						cp->conn_arg3 != NULL && strlen((char *)cp->conn_arg3) > 0) {
					node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10);
					grp = strtol((char *)cp->conn_arg3, NULL, 10);
					if (strcmp((char *)cp->conn_arg1, "group") == 0) { /* Group update */
						pthread_mutex_lock(&nlock);
						nodes[node]->updateGroup(node, grp, (char *)cp->conn_arg4);
						pthread_mutex_unlock(&nlock);
					}
				}
				return MHD_YES;
			} else
				ret = web_send_data(conn, EMPTY, MHD_HTTP_OK, false, false, NULL); // no free, no copy
		} else if (strcmp(url, "/pollpost.html") == 0) {
			if (*up_data_size != 0) {
				uint8 node;

				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;

				if (cp->conn_arg2 != NULL && strlen((char *)cp->conn_arg2) > 0 &&
						cp->conn_arg3 != NULL && strlen((char *)cp->conn_arg3) > 0 &&
						cp->conn_arg4 != NULL && strlen((char *)cp->conn_arg4) > 0) {
					node = strtol(((char *)cp->conn_arg2) + 4, NULL, 10) + 1;
					if (strcmp((char *)cp->conn_arg1, "poll") == 0) { /* Poll update */
						pthread_mutex_lock(&nlock);
						nodes[node]->updatePoll((char *)cp->conn_arg3, (char *)cp->conn_arg4);
						pthread_mutex_unlock(&nlock);
					}
				}
				return MHD_YES;
			} else
				ret = web_send_data(conn, EMPTY, MHD_HTTP_OK, false, false, NULL); // no free, no copy
		} else if (strcmp(url, "/savepost.html") == 0) {
			if (*up_data_size != 0) {
				MHD_post_process(cp->conn_pp, up_data, *up_data_size);
				*up_data_size = 0;

				if (strcmp((char *)cp->conn_arg1, "save") == 0) { /* Save config */
					Manager::Get()->WriteConfig(homeId);
					pthread_mutex_lock(&glock);
					needsave = false;
					pthread_mutex_unlock(&glock);
				}
				return MHD_YES;
			} else
				ret = web_send_data(conn, EMPTY, MHD_HTTP_OK, false, false, NULL); // no free, no copy
		} else
			ret = web_send_data(conn, UNKNOWN, MHD_HTTP_NOT_FOUND, false, false, NULL); // no free, no copy
		return ret;
	} else
		return MHD_NO;
}

/*
 * web_free
 * Free up any allocated data after connection closed
 */

void Webserver::FreeEP (void *cls, struct MHD_Connection *conn,
		void **ptr, enum MHD_RequestTerminationCode code)
{
	Webserver *ws = (Webserver *)cls;

	ws->Free(conn, ptr, code);
}

void Webserver::Free (struct MHD_Connection *conn, void **ptr, enum MHD_RequestTerminationCode code)
{
	conninfo_t *cp = (conninfo_t *)*ptr;

	if (cp != NULL) {
		if (cp->conn_arg1 != NULL)
			free(cp->conn_arg1);
		if (cp->conn_arg2 != NULL)
			free(cp->conn_arg2);
		if (cp->conn_arg3 != NULL)
			free(cp->conn_arg3);
		if (cp->conn_arg4 != NULL)
			free(cp->conn_arg4);
		if (cp->conn_type == CON_POST) {
			MHD_destroy_post_processor(cp->conn_pp);
		}
		free(cp);
		*ptr = NULL;
	}
}

/*
 * Constructor
 * Start up the web server
 */

Webserver::Webserver (int const wport) : sortcol(COL_NODE), logbytes(0), adminstate(false)
{
	fprintf(stderr, "webserver starting port %d\n", wport);
	port = wport;
	devname = (char *)malloc(strlen("/dev/ttyACM0") + 1);
	if (devname == NULL) {
		fprintf(stderr, "Out of memory open devname\n");
		exit(1);
	}
	usb = false;
	strcpy(devname, "/dev/ttyACM0");
	Manager::Get()->AddDriver(devname);
	MyNode::setAllChanged(true);

	wdata = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG, port,
			NULL, NULL, &Webserver::HandlerEP, this,
			MHD_OPTION_NOTIFY_COMPLETED, Webserver::FreeEP, this, MHD_OPTION_END);

	if (wdata != NULL) {
		ready = true;
	}
}

/*
 * Destructor
 * Stop the web server
 */

Webserver::~Webserver ()
{
	if (wdata != NULL) {
		MHD_stop_daemon((MHD_Daemon *)wdata);
		wdata = NULL;
		ready = false;
	}
}
