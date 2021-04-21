#include "mbed.h"

#include "ffconf.h"

#include "FATFileSystem.h"
#include "FATFileHandle.h"
#include "FATDirHandle.h"

DW get_fattime(void)
{
    time_t rawtime;
    time(&rawtime);
    struct tm *ptm = localtime(&rawtime);
    return (DW)(ptm->tm_year - 80) << 25 | (DW)(ptm->tm_mon + 1) << 21 | (DW)(ptm->tm_mday) << 16 | (DW)(ptm->tm_hour) << 11 | (DW)(ptm->tm_min) << 5 | (DW)(ptm->tm_sec / 2);
}

FATFileSystem *FATFileSystem::_ffs[_VOLUMES] = {0};

FATFileSystem::FATFileSystem(const char *n) : FileSystemLike(n)
{
    for (int i = 0; i < _VOLUMES; i++)
    {
        if (_ffs[i] == 0)
        {
            _ffs[i] = this;
            _fsid[0] = '0' + i;
            _fsid[1] = '\0';
            f_mount(&_fs, _fsid, 0);
            return;
        }
    }
}

FATFileSystem::~FATFileSystem()
{
    for (int i = 0; i < _VOLUMES; i++)
    {
        if (_ffs[i] == this)
        {
            _ffs[i] = 0;
            f_mount(NULL, _fsid, 0);
        }
    }
}

FileHandle *FATFileSystem::open(const char *name, int flags)
{
    char n[64];
    sprintf(n, "%s:/%s", _fsid, name);


    B openmode;
    if (flags & O_RDWR)
    {
        openmode = FA_READ | FA_WRITE;
    }
    else if (flags & O_WRONLY)
    {
        openmode = FA_WRITE;
    }
    else
    {
        openmode = FA_READ;
    }
    if (flags & O_CREAT)
    {
        if (flags & O_TRUNC)
        {
            openmode |= FA_CREATE_ALWAYS;
        }
        else
        {
            openmode |= FA_OPEN_ALWAYS;
        }
    }

    FIL fh;
    FRESULT res = f_open(&fh, n, openmode);
    if (res)
    {
        return NULL;
    }
    if (flags & O_APPEND)
    {
        f_lseek(&fh, fh.fsize);
    }
    return new FATFileHandle(fh);
}

int FATFileSystem::remove(const char *filename)
{
    FRESULT res = f_unlink(filename);
    if (res)
    {
        return -1;
    }
    return 0;
}

DirHandle *FATFileSystem::opendir(const char *name)
{
    FATFS_DIR dir;
    FRESULT res = f_opendir(&dir, name);
    if (res != 0)
    {
        return NULL;
    }
    return new FATDirHandle(dir);
}

int FATFileSystem::mkdir(const char *name, mode_t mode)
{
    FRESULT res = f_mkdir(name);
    return res == 0 ? 0 : -1;
}

int FATFileSystem::mount()
{
    FRESULT res = f_mount(&_fs, _fsid, 1);
    return res == 0 ? 0 : -1;
}
