#ifndef MBED_MEMFILESYSTEM_H
#define MBED_MEMFILESYSTEM_H

#include "FATFileSystem.h"

namespace mbed
{

class MemFileSystem : public FATFileSystem
{
public:
    char *sectors[2000];

    MemFileSystem(const char *name) : FATFileSystem(name)
    {
        memset(sectors, 0, sizeof(sectors));
    }

    virtual ~MemFileSystem()
    {
        for (int i = 0; i < 2000; i++)
        {
            if (sectors[i])
            {
                free(sectors[i]);
            }
        }
    }

    virtual int disk_read(char *buffer, int sector)
    {
        if (sectors[sector] == 0)
        {

            memset(buffer, 0, 512);
        }
        else
        {
            memcpy(buffer, sectors[sector], 512);
        }
        return 0;
    }

    virtual int disk_write(const char *buffer, int sector)
    {

        char zero[512];
        memset(zero, 0, 512);
        if (memcmp(zero, buffer, 512) == 0)
        {
            if (sectors[sector] != 0)
            {
                free(sectors[sector]);
                sectors[sector] = 0;
            }
            return 0;
        }

        if (sectors[sector] == 0)
        {
            char *sec = (char *)malloc(512);
            if (sec == 0)
            {
                return 1;
            }
            sectors[sector] = sec;
        }
        memcpy(sectors[sector], buffer, 512);
        return 0;
    }

    virtual int disk_sectors()
    {
        return sizeof(sectors) / sizeof(sectors[0]);
    }
};

}

#endif