#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED
#ifdef __cplusplus
extern "C"
{
#endif
#define _USE_WRITE 1
#define _USE_IOCTL 1
#include "integer.h"
	typedef B DSTATUS;
	typedef enum
	{
		RES_OK = 0,
		RES_ERROR,
		RES_WRPRT,
		RES_NOTRDY,
		RES_PARERR
	} DRESULT;
	DSTATUS disk_initialize(B pdrv);
	DSTATUS disk_status(B pdrv);
	DRESULT disk_read(B pdrv, B *buff, DW sector, UI count);
	DRESULT disk_write(B pdrv, const B *buff, DW sector, UI count);
	DRESULT disk_ioctl(B pdrv, B cmd, void *buff);
#define STA_NOINIT 0x01
#define STA_PROTECT 0x04
#define CTRL_SYNC 0
#define GET_SECTOR_COUNT 1
#define GET_BLOCK_SIZE 3
#ifdef __cplusplus
}
#endif
#endif
