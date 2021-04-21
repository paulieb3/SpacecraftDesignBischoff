#include "pxt.h"
#include "mbed.h"
#include "SDFileSystem.h"
#include "MicroBit.h"

namespace im01
{
//%
void _mkdir(String s)
{
    SDFileSystem sd(P0_21, P0_22, P0_23, P0_16, "sd");
    microbit_create_heap(MICROBIT_SD_GATT_TABLE_START + MICROBIT_SD_GATT_TABLE_SIZE, MICROBIT_SD_LIMIT);
    mkdir((const char *)s->getUTF8Data(), 0777);
}


//%
String _read(String s)
{
	char * _word;
	char* error_no_file = "ERROR! NO FILE";
	char* cant_read_file = "ERROR! CANT READ FILE";
	//char* path = "/sd/im01/";
	int lSize;
	size_t b_read;

    SDFileSystem sd(P0_21, P0_22, P0_23, P0_16, "sd");
	FILE *fp = fopen((const char *)s->getUTF8Data(), "rb");

	if (fp == NULL)
	{
        return mkString(error_no_file, strlen(error_no_file));
	}else{

	// obtain file size:
	fseek (fp , 0 , SEEK_END);
	lSize = ftell(fp);
	rewind (fp);

	_word = (char*) malloc (sizeof(char)*(lSize));
	b_read = fread(_word, sizeof(char), lSize, fp);

	fclose(fp);
	}

	if(b_read != (lSize))
	{
        return mkString(cant_read_file, strlen(cant_read_file));
	}

	String str = mkString(_word, lSize);

	free(_word);
	return str;
}
} // namespace im01