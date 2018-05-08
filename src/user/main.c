//including header files
#include "main.h"

//u8 SD_Tx[MULTI_BUFFER_SIZE], SD_Rx[MULTI_BUFFER_SIZE];
 

 
 
int main()
{
	
	ticks_init();
	tft_init(0,BLACK,WHITE,GREEN);
	//SD_init();
	button_init();
	//uart_init(UART3,115200);
	uart_init(UART1,115200);
	uart_interrupt_init(UART1,uart_receive);
  u8 but = 0;
	u8 time = 0;
	uint32_t ticks = get_ticks();
	//crystal check
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

	u16 count=0;
	while(1)
	{
		TestFunction();
		/*for(int i=0;i<3;i++){
			tft_prints(0,i,"%d %d %d",getUpper_M().n[i*3+0]/128,getUpper_M().n[i*3+1]/128,getUpper_M().n[i*3+2]/128);
		}
		tft_update();
		while(1);*/
		
		
		
		
		
		
    if(get_ticks() != ticks)
		{
      ticks = get_ticks();
		if(ticks % 100 == 1 & but == 0){
		cameraTestTftDisplay();
		tft_update();
		tft_clear();
		//		count++;
		}
		//if(ticks%60 == 2 && but == 1){
		//	PositionUpdate();
		//}
			if(read_button(BUTTON1)){
				count ++;
				if(count>15){count=0;}
				send_LED(count);
				//u8 x1[5]={1,2,3,4,5};
				//u8 y1[5]={6,7,8,9,0};
				//send_target(x1,y1,5);
				
				

			}
			if(read_button(BUTTON2)){
			//	if(but==0){
					storeImage();
				//	tft_prints(0,9,"Done");
				//	but = 1;
				//}
				//storeImage();
				//else{computeDirty();
				//	but = 0;
				//}
				
				
				//but = ~(but&0x01);	
			//	send_position(count ,count);
				//count=2;
				//for(int i=0;i<3;i++){
				//ComputeCoordinate(x[i],y[i]);
				//while(!read_button(BUTTON2));
			//	}
			}
			if(read_button(BUTTON3)){
				uartImage();
				//while(1){PositionUpdate();}
			/*	time = get_ticks();
				send_ready();
				while(!get_ready()){}
				time = get_ticks() - time;
				tft_clear();
				tft_prints(0,7,"%d",time);
				tft_update();
				while(1);*/
				
			}
			if(read_button(BUTTON4)){
				computeDirty();
				//positionInit();
				//flashDetection(3,11,0);
				//count=4;
				//send_LED(0x0F);
			}
		//	pos = get_position();
			
			//tft_prints(0,0,"UART Testing");	
			//if( get_ready()){
				//tft_prints(0,1,"UART ReaDy");	
			//}
			//tft_prints(0,2,"LED: %d",get_LED());
			//tft_prints(0,3,"POS: %d %d",pos.n[0],pos.n[1]);

		/*	for(int i=0;i<getSize();i++){
				tft_prints(0,i,"%d %d",get_target_x()[i],get_target_y()[i]);
			}
			tft_prints(0,6,"%d",get_LED());
			tft_prints(0,7,"%d",get_ready());
			tft_prints(0,8,"%d",get_nextmove());
			vec3 f = get_position();
			tft_prints(0,9,"%d %d",f.n[0],f.n[1]);*/
			//if(but){
			
			
	/*		tft_prints(0,9,"%d %d",get_x_length(),get_y_length());
			tft_prints(0,5,"%d %d",getCorner(0).n[0],getCorner(0).n[1]);
			tft_prints(0,6,"%d %d",getCorner(1).n[0],getCorner(1).n[1]); 
			tft_prints(0,7,"%d %d",getCorner(2).n[0],getCorner(2).n[1]);
			tft_prints(0,8,"%d %d",getCorner(3).n[0],getCorner(3).n[1]);*/
			
			
		
			
		//	button_update();
		//}
	}}
}
