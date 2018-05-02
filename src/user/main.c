//including header files
#include "main.h"


 
int main()
{
	ticks_init();
	tft_init(0,BLACK,WHITE,GREEN);
	button_init();
	uart_init(UART3,115200);
	uart_interrupt_init(UART3,uart_receive);
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
	//vec3 pos;
	tft_clear();
	tft_prints(0,0,"OV7725 init fail");
	tft_update();
	while(!cameraInit(GreyScale));
	tft_clear();
	tft_prints(0,0,"OV7725 init done");
	//TestFunction();
	tft_update();
//	u8 x[500]={0,0,0,0,0};
//	u8 y[500]={0,0,0,0,0};
//	u8 array[arraySize];
//	u8 array1[arraySize];
//	for(u16 i=0;i<arraySize;i++){
//		array[i]=i/12;
	//	array1[i]=i/12+1;
//	}
	//tft_prints(0,0,"Array Construction");
	//tft_update();
	
	
//	writeRow(array,4);
//	for(u16 i=0;i<arraySize;i++){
//		if(readImage(i%320,(i/320)+4)!=array[i]){break;}
//		if(readImage(i%320,(i/320)+4)!=array1[i]){break;}
	//	if(i==arraySize-1){flash_test = 1;}
		
//}
	tft_clear();
	while(1)
	{
		
    if(get_ticks() != ticks)
		{
      ticks = get_ticks();
	//	if(ticks % 100 == 1){cameraTestTftDisplay();}
			if(!read_button(BUTTON1)){
				u8 x1[5]={1,2,3,4,5};
				u8 y1[5]={6,7,8,9,0};
				send_target(x1,y1,5);
				
				
				//storeImage();
				//but = 1;
				//time = get_ticks()-ticks;
			}
			if(!read_button(BUTTON2)){
				send_position(10000,20000);
				
			}
			if(!read_button(BUTTON3)){
				send_LED(0x0F);
			}
		//	pos = get_position();
			
			//tft_prints(0,0,"UART Testing");	
			//if( get_ready()){
				//tft_prints(0,1,"UART ReaDy");	
			//}
			//tft_prints(0,2,"LED: %d",get_LED());
			//tft_prints(0,3,"POS: %d %d",pos.n[0],pos.n[1]);

			for(int i=0;i<getSize();i++){
				tft_prints(0,i,"%d %d",get_target_x()[i],get_target_y()[i]);
			}
			tft_prints(0,6,"%d",get_LED());
			tft_prints(0,7,"%d",get_ready());
			tft_prints(0,8,"%d",get_nextmove());
			vec3 f = get_position();
			tft_prints(0,9,"%d %d",f.n[0],f.n[1]);
			//if(but){
			//tft_prints(0,8,"Done %d",time);} 
			tft_update();
		//	button_update();
		}
	}
}
