#include "flash.h"

#define startAddress 			0x08009000
#define startAddress2    	0x0801D000

void writeRow(u8* array, u16 y){
	if(y%colSize!=0){return;}
	y/=colSize;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(startAddress+2048*y);
	for(int i=0;i<arraySize/2;i++){
		u16 word = ((uint16_t)(array[i*2])<<8)|(uint16_t)(array[i*2+1]);
		FLASH_ProgramHalfWord((startAddress+2048*y+i*2),word);
	}
	FLASH_Lock();
}

void writeRow2(u8* array, u16 y){
	if(y%colSize!=0){return;}
	y/=colSize;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(startAddress2+2048*y);
	for(int i=0;i<arraySize/2;i++){
		u16 word = ((uint16_t)(array[i*2])<<8)|(uint16_t)(array[i*2+1]);
		FLASH_ProgramHalfWord((startAddress2+2048*y+i*2),word);
	}
	FLASH_Lock();
}





u8 readImage(u16 x,u16 y){
	
	u32 offset = (y/colSize)*2048;
	y%=colSize;
	
	if(x%2==0){
	return (uint8_t)((*(uint16_t *)(startAddress+320*y+x+offset))>>8);
	}
	else{
	return (uint8_t)((*(uint16_t *)(startAddress+320*y+offset+x-1)));
	}
}

u8 readImage2(u16 x,u16 y){
	
	u32 offset = (y/colSize)*2048;
	y%=colSize;
	
	if(x%2==0){
	return (uint8_t)((*(uint16_t *)(startAddress2+320*y+x+offset))>>8);
	}
	else{
	return (uint8_t)((*(uint16_t *)(startAddress2+320*y+offset+x-1)));
	}
}

