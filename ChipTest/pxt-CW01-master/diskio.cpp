#include "diskio.h"
#include "FATFileSystem.h"
using namespace mbed;
DSTATUS disk_status(
    B pdrv)
{
    return (DSTATUS)FATFileSystem::_ffs[pdrv]->disk_status();
}
DSTATUS disk_initialize(
    B pdrv)
{
    return (DSTATUS)FATFileSystem::_ffs[pdrv]->disk_initialize();
}
DRESULT disk_read(
    B pdrv,
    B *buff,
    DW sector,
    UI count)
{
    if (FATFileSystem::_ffs[pdrv]->disk_read((uint8_t *)buff, sector, count))
        return RES_PARERR;
    else
        return RES_OK;
}
#if _USE_WRITE
DRESULT disk_write(
    B pdrv,
    const B *buff,
    DW sector,
    UI count)
{
    if (FATFileSystem::_ffs[pdrv]->disk_write((uint8_t *)buff, sector, count))
        return RES_PARERR;
    else
        return RES_OK;
}
#endif
#if _USE_IOCTL
DRESULT disk_ioctl(
    B pdrv,
    B cmd,
    void *buff)
{
    switch (cmd)
    {
    case CTRL_SYNC:
        if (FATFileSystem::_ffs[pdrv] == NULL)
        {
            return RES_NOTRDY;
        }
        else if (FATFileSystem::_ffs[pdrv]->disk_sync())
        {
            return RES_ERROR;
        }
        return RES_OK;
    case GET_SECTOR_COUNT:
        if (FATFileSystem::_ffs[pdrv] == NULL)
        {
            return RES_NOTRDY;
        }
        else
        {
            DW res = FATFileSystem::_ffs[pdrv]->disk_sectors();
            if (res > 0)
            {
                *((DW *)buff) = res;
                return RES_OK;
            }
            else
            {
                return RES_ERROR;
            }
        }
    case GET_BLOCK_SIZE:
        *((DW *)buff) = 1;
        return RES_OK;
    }
    return RES_PARERR;
}
#endif
