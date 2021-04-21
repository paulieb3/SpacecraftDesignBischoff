#ifndef _FATFS
#define _FATFS 64180
#ifdef __cplusplus
extern "C"
{
#endif
#include "integer.h"
#include "ffconf.h"

#define LD2PD(vol) (B)(vol)
#define LD2PT(vol) 0
	typedef char TCHAR;
	typedef struct
	{
		B fs_type;
		B drv;
		B csize;
		B n_fats;
		B wflag;
		B fsi_flag;
		W id;
		W n_rootdir;
		DW last_clust;
		DW free_clust;
		DW n_fatent;
		DW fsize;
		DW volbase;
		DW fatbase;
		DW dirbase;
		DW database;
		DW winsect;
		B win[_MAX_SS];
	} FATFS;
	typedef struct
	{
		FATFS *fs;
		W id;
		B flag;
		B err;
		DW fptr;
		DW fsize;
		DW sclust;
		DW clust;
		DW dsect;
		DW dir_sect;
		B *dir_ptr;
	} FIL;
	typedef struct
	{
		FATFS *fs;
		W id;
		W index;
		DW sclust;
		DW clust;
		DW sect;
		B *dir;
		B *fn;
		WC *lfn;
		W lfn_idx;
	} FATFS_DIR;
	typedef struct
	{
		DW fsize;
		W fdate;
		W ftime;
		B fattrib;
		char fname[13];
		char *lfname;
		UI lfsize;
	} FILINFO;
	typedef enum
	{
		FR_OK = 0,
		FR_DISK_ERR,
		FR_INT_ERR,
		FR_NOT_READY,
		FR_NO_FILE,
		FR_NO_PATH,
		FR_INVALID_NAME,
		FR_DENIED,
		FR_EXIST,
		FR_INVALID_OBJECT,
		FR_WRITE_PROTECTED,
		FR_INVALID_DRIVE,
		FR_NOT_ENABLED,
		FR_NO_FILESYSTEM,
		FR_MKFS_ABORTED,
		FR_TIMEOUT,
		FR_LOCKED,
		FR_NOT_ENOUGH_CORE,
		FR_TOO_MANY_OPEN_FILES,
		FR_INVALID_PARAMETER
	} FRESULT;
	FRESULT f_open(FIL *fp, const char *path, B mode);
	FRESULT f_close(FIL *fp);
	FRESULT f_read(FIL *fp, void *buff, UI btr, UI *br);
	FRESULT f_write(FIL *fp, const void *buff, UI btw, UI *bw);
	FRESULT f_lseek(FIL *fp, DW ofs);
	FRESULT f_sync(FIL *fp);
	FRESULT f_opendir(FATFS_DIR *dp, const char *path);
	FRESULT f_closedir(FATFS_DIR *dp);
	FRESULT f_readdir(FATFS_DIR *dp, FILINFO *fno);
	FRESULT f_mkdir(const char *path);
	FRESULT f_unlink(const char *path);
	FRESULT f_utime(const char *path, const FILINFO *fno);
	FRESULT f_mount(FATFS *fs, const char *path, B opt);
	int f_printf(FIL *fp, const char *str, ...);

#ifndef EOF
#define EOF (-1)
#endif
#if _USE_LFN
	WC ff_convert(WC chr, UI dir);
	WC ff_wtoupper(WC chr);
#endif
#define FA_READ 0x01
#define FA_WRITE 0x02
#define FA_CREATE_NEW 0x04
#define FA_CREATE_ALWAYS 0x08
#define FA_OPEN_ALWAYS 0x10
#define FA__WRITTEN 0x20
#define FS_FAT12 1
#define FS_FAT16 2
#define FS_FAT32 3
#define AM_RDO 0x01
#define AM_HID 0x02
#define AM_SYS 0x04
#define AM_VOL 0x08
#define AM_LFN 0x0F
#define AM_DIR 0x10
#define AM_ARC 0x20
#define AM_MASK 0x3F
#define LD_WORD(ptr) (W)(((W) * ((B *)(ptr) + 1) << 8) | (W) * (B *)(ptr))
#define LD_DWORD(ptr) (DW)(((DW) * ((B *)(ptr) + 3) << 24) | ((DW) * ((B *)(ptr) + 2) << 16) | ((W) * ((B *)(ptr) + 1) << 8) | *(B *)(ptr))
#define ST_WORD(ptr, val)         \
	*(B *)(ptr) = (B)(val); \
	*((B *)(ptr) + 1) = (B)((W)(val) >> 8)
#define ST_DWORD(ptr, val)                             \
	*(B *)(ptr) = (B)(val);                      \
	*((B *)(ptr) + 1) = (B)((W)(val) >> 8);   \
	*((B *)(ptr) + 2) = (B)((DW)(val) >> 16); \
	*((B *)(ptr) + 3) = (B)((DW)(val) >> 24)
#ifdef __cplusplus
}
#endif
#endif
