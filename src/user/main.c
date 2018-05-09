//including header files
#include "main.h"

//u8 SD_Tx[MULTI_BUFFER_SIZE], SD_Rx[MULTI_BUFFER_SIZE];
 
 /*FATFS fs; 
 FIL fnew; 
 FRESULT res_sd; 
 UINT fnum; 
 u8 ReadBuffer[FF_MAX_SS]= {0}; 
 u8 WriteBuffer[] = "Thank you very much";*/


/*
					if(read_button(BUTTON1)){

					}
					if(read_button(BUTTON2)){ 

					}
					if(read_button(BUTTON3)){
					
					}
					if(read_button(BUTTON4)){
					
					}




*/


u8 Buffer_TX[512];
u8 Buffer_RX[512];
 
int main()
{
	
	ticks_init();
	tft_init(0,BLACK,WHITE,GREEN);
	button_init();
	SD_Init();
	uart_init(UART1,115200);
	
	
	//SD_init();
	//while(1);
	//uart_init(UART3,115200);

	uart_interrupt_init(UART1,uart_receive);
  u8 but = 0;
	u32 time = 0;
	uint32_t ticks = get_ticks();
	//crystal check
	 u16 state = 0;
	while(ticks + 10 >= get_ticks())
	{
		tft_clear();
		tft_prints(0,0,"%d",get_ticks());
		tft_prints(0,1,"Crystal Broken");
		tft_update();
	}

//	u8 flash_test = 0;
	tft_clear();
	tft_prints(0,0,"Camera initing ");
	tft_update();
	while(!cameraInit(GreyScale));
	tft_clear();
	tft_prints(0,0,"Camera init done");
	//TestFunction();
	tft_update();
	u8 user_point_index = 0 ;
	u8 count=0;
	u8 init_bit = 0;
	
	
	//SD_Erase(0x00000000,0x000FFFFF);
//	tft_prints(0,1,"Done Erasing");
//	tft_update();
//	SD_save(0x0000000);
//	SD_uart(0);
	/*u8 On9[512];
	u8 y[512];
	u8* On8 = y;
	for(u8 i = 0;i<255;i++){
		On9[i]=i;
	}
	
	SD_WriteBlock(On9,0x00000000,512);
	SD_WaitWriteOperation();
	SD_ReadBlock(On8,0,512);
	SD_WaitReadOperation();
	
	for(u16 i = 0;i<512;i++){
			uart_tx(UART1,"%d%d ",On9[i],*On8);
			On8++;
	}*/
	
	//tft_prints(0,2,"Done Saving");
	tft_update();
	while(1)
	{
		//SD_Init();
	//	res_sd = f_mount(&fs,"0:",1);

		
		tft_clear();
		/*if(res_sd == FR_OK){
				tft_prints(0,0,"Initialized");
		}else if(res_sd == FR_NO_FILESYSTEM){
				tft_prints(0,0,"no filesystem");
			res_sd=f_mkfs("0:",0,0,ReadBuffer,sizeof(ReadBuffer));
				if (res_sd == FR_OK) {
					tft_prints(0,1,"SUCCESS");
					res_sd = f_mount(NULL,"0:",1);
					tft_update();
					while(1);
				}
				else{
					tft_prints(0,1,"%d",res_sd);
					tft_update();
					while(1);
				
				}
			
			
			
			
		}else if(res_sd == FR_NOT_ENABLED){
				tft_prints(0,0,"FR_NOT_ENABLED");
		}else if(res_sd ==  FR_NOT_READY){
				tft_prints(0,0," FR_NOT_READY");
		}else if(res_sd ==  FR_DISK_ERR){
				tft_prints(0,0," FR_DISK_ERR");
		}else{
				tft_prints(0,0,"WTF");
		}
		tft_update();
		while(1);
		*/
		
		//TestFunction();

		
    if(get_ticks() != ticks)
		{
			ticks = get_ticks();

		//if(ticks%60 == 2 && but == 1){
		//	PositionUpdate();
		//}
		/*	if(read_button(BUTTON1)){
				count ++;
				if(count>15){count=0;}
				send_LED(count);

			}
			if(read_button(BUTTON2)){
				positionInit();
				

			}
			if(read_button(BUTTON3)){
				uartImage();

			}
			if(read_button(BUTTON4)){
				computeDirty();

			}*/
			switch(state){
				case 0:{
					if(read_button(BUTTON1)){
						while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)){
							if(get_ticks()>time+1000){break;}
						
						}
						tft_clear();
						tft_prints(0,8,"UARTing to PC");
						tft_update();
						for(u16 i=0;i<readnum();i++){
							SD_uart(i*512*150);
						}
						writenum(0);
					}
					if(read_button(BUTTON2)){ // go to init state 
						state = 1;
					}
					if(read_button(BUTTON3)){
						if(!init_bit){
							time = get_ticks();
							tft_clear();
							tft_prints(0,9,"Please init");
							tft_update();
							while(1){
							
								if(get_ticks()>time+1000){
									break;
								}
							
							}
							break;
						}
						computeDirty();
						while(get_ready()){
						PositionUpdate();}
						reset_ready();
						storeImage();
					}
					if(read_button(BUTTON4)){
						while(1)
						{
							if(read_button(BUTTON4)){
								SetBackToZero();
							}
						PositionUpdate();
						}
					/*	u16 count = 0;
						u8 BUFFER[512];
								SD_ReadBlock(Buffer,0,512);
								SD_WaitReadOperation();
								SD_ReadBlock(BUFFER,512,512);
								SD_WaitReadOperation();
						
						for(u16 i=0;i<512;i++){
								if(BUFFER[i]-Buffer[i]==0){
									count++;
								}
						
						}
						tft_prints(0,7,"%d",count);
						tft_update();
						while(1);*/
						//SD_SD_SD();
						//SD_uart(0);
					}
					//tft_prints(0,5,"%d",get_x_length());
					//tft_prints(6,5,"%d",get_y_length());
					 tft_prints(6,7,"INIT");
						tft_prints(5,8,"CLEAN");
						tft_prints(0,8,"UART");
					
					
					break;
				}
				case 1:{ // init state 
					if(read_button(BUTTON1)){ // auto init 
						init_bit = 1;
						reset_user_flag();
						positionInit();
						state = 0;
						storeImage();
					}
					if(read_button(BUTTON2)){ 
						state = 2;
						set_user_flag();
						user_point_index = 0 ;

					}
					if(read_button(BUTTON3)){
						send_LED(15);
						time = get_ticks();
						while(time>get_ticks()-30);
						send_LED(0);
					}
					if(read_button(BUTTON4)){ // back
						state = 0;
					}
					tft_prints(2,6,"AUTO");
					tft_prints(0,7,"BACK"); tft_prints(6,7,"USER");
					tft_prints(2,8,"LED");
					break;
				}
				case(2):{
					if(read_button(BUTTON1)){
						time = get_ticks();
						while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)){
							if(get_ticks()>time+1000){
								user_point_index++;
								if(user_point_index == 4){
									init_bit = 1;
									set_user_flag();
									UserPositionInit();
									state = 0;
									storeImage();
								}
								break;
							}
						
						}
						user_point_Y_decrease(user_point_index);
					}
					if(read_button(BUTTON2)){ 
						user_point_X_increase(user_point_index);
					}
					if(read_button(BUTTON3)){
						time = get_ticks();
						while(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)){
							if(get_ticks()>time+1000){
									state = 1;
									break;
								
								
							}
						
						}
						user_point_Y_increase(user_point_index);
					}
					if(read_button(BUTTON4)){
						user_point_X_decrease(user_point_index);
					}
					tft_prints(4,6,"UP");
					tft_prints(0,7,"LEFT"); tft_prints(6,7,"RIGHT");
					tft_prints(4,8,"DOWN");
					
					tft_prints(0,9,"%d",user_point_index);
					tft_update();
					break;
				}
			
			
			
			}
			
			if(ticks % 100 == 1){
			cameraTestTftDisplay();
			tft_update();
			tft_clear();
			}
		}
	}
}




/*
void SD_save(u32 addr){
	u8 temp = 0;
	SD_Error Status = SD_OK;
	for(u32 i=0;i<320*240;i++){
		Buffer_TX[i%512] = readImage(i%320,i/320);
		temp = Buffer_TX[i%512];
		uart_tx(UART1,"%c%c",((temp&0xF0)>>4)+64,((temp&0x0F)>>0)+64);
		if((i+1)%512==0){
				SD_WriteBlock(Buffer_TX,addr+(i-511),512);
				SD_WaitWriteOperation();
		}
	}
}*/

/*
void SD_uart(u32 addr){
	//u8 temp = 0;
	SD_Error Status = SD_OK;
	for(u32 i=0;i<320*240;i++){
		
		//temp = Buffer[i%512];
		//uart_tx(UART1,"%c%c",((temp&0xF0)>>4)+64,((temp&0x0F)>>0)+64);
		if((i)%512==0){
				SD_ReadBlock(Buffer_RX,addr+(i*512),512);
				SD_WaitReadOperation();
		}
		uart_tx(UART1,"%c%c",((Buffer_RX[i%512] &0xF0)>>4)+64,((Buffer_RX[i%512] &0x0F)>>0)+64);
		
	}
}*/
/*
void SD_uart(u32 addr){
	u8 temp=0;
	SD_Error Status = SD_OK;
	Status = SD_ReadBlock(Buffer_RX,addr,512);
	Status = SD_WaitReadOperation();
	for(u32 i =0;i<512;i++){
		temp = Buffer_RX[i];
		uart_tx(UART1,"%c%c",((temp&0xF0)>>4)+64,((temp&0x0F)>>0)+64);
	}
	
}*/


/*void SD_SD_SD(void){
	for(u32 i=0;i<150;i++){
		SD_uart(i*512);
	}
}*/
