/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "sdcard.h"
#include "tft.h"
#include <string.h>

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

extern  SD_CardInfo SDCardInfo;


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	stat &= ~STA_NOINIT;
	
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS status = STA_NOINIT;
	if (SD_Init()==SD_OK) {
		 status &= ~STA_NOINIT;
 } else {
 status = STA_NOINIT;
 }


	return status;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{

	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;
	
	 if ((DWORD)buff&3) {
		DRESULT res = RES_OK;
		DWORD scratch[SD_BLOCKSIZE / 4];

			while (count--) {
				res = disk_read(0,(void *)scratch, sector++, 1);

				if (res != RES_OK) {
					break;
				}
				memcpy(buff, scratch, SD_BLOCKSIZE);
				buff += SD_BLOCKSIZE;
			}
 return res;
 }


 
 SD_state=SD_ReadMultiBlocks(buff,sector*SD_BLOCKSIZE,SD_BLOCKSIZE,count);
 

 
 if (SD_state==SD_OK) {
 /* Check if the Transfer is finished */

	 
 SD_state=SD_WaitReadOperation();
	 

	 
// while (SD_GetStatus() != SD_TRANSFER_OK);
 }
 

 
 if (SD_state!=SD_OK){
 status = RES_PARERR;}
 else{
  status = RES_OK;}

	return status;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	
	DRESULT status = RES_PARERR;
 SD_Error SD_state = SD_OK;

 if (!count) {
	return RES_PARERR; /* Check parameter */
 }
	
if ((DWORD)buff&3) {
 DRESULT res = RES_OK;
 DWORD scratch[SD_BLOCKSIZE / 4];

 while (count--) {
 memcpy( scratch,buff,SD_BLOCKSIZE);
 res = disk_write(0,(void *)scratch, sector++, 1);
 if (res != RES_OK) {
 break;
 }
 buff += SD_BLOCKSIZE;
 }
 return res;
 }

 SD_state=SD_WriteMultiBlocks((uint8_t *)buff,sector*SD_BLOCKSIZE,SD_BLOCKSIZE,count);
 if (SD_state==SD_OK) {
	 /* Check if the Transfer is finished */
 SD_state=SD_WaitReadOperation();

 /* Wait until end of DMA transfer */
 //while (SD_GetStatus() != SD_TRANSFER_OK);
 }
 if (SD_state!=SD_OK){
 status = RES_PARERR;}
 else{
 status = RES_OK;}
 
 
 return status;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	
	 DRESULT status = RES_PARERR;

	
switch (cmd) {
 // Get R/W sector size (WORD)
 case GET_SECTOR_SIZE :
 *(WORD * )buff = SD_BLOCKSIZE;
 break;
 // Get erase block size in unit of sector (DWORD)
 case GET_BLOCK_SIZE :
 *(DWORD * )buff = SDCardInfo.CardBlockSize;
 break;

 case GET_SECTOR_COUNT:
 *(DWORD*)buff =SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
 break;
				case CTRL_SYNC :
		break;
	}
	status = RES_OK;
return status;
}

