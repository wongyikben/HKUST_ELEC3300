#include "camera.h"


#define TOP_LEFT 	0
#define TOP_RIGHT 1
#define BOT_LEFT 	2
#define BOT_RIGHT 3


#define ImageLength 240
#define ImageWidth 320

#define ZERO_8 0x00
#define ZERO_16 0x0000

ImageType imageType = RGBColour;
uint8_t camState = 0;
uint8_t threshold = 0;
//uint8_t image[ImageLength][ImageWidth];
u8 Image[arraySize]; // 4 Row 
u8 rough[ImageLength/4][ImageWidth/4];


u16 x_limit[2];
u16 y_limit[2];

vec3 point[3];
vec3 corner[4];

s16 x_length=0;
s16 y_length=0;
// Position is the position of the robot in the camera , NOT THE REAL COORDINATE
vec3 Position;

mat3 Upper_M;
mat3 Lower_M;
mat3 Upper_invM;
mat3 Lower_invM;

s32 slope = 0;
s32 offset = 0;

u16 x_portion = 0;
u16 y_portion = 0;


//corner
//     0      1 
//     2      3

//LED
//     1      2
//     3      4       LED =   1234


//register default
OV7725REG cameraConfig[] =
{
	{COM2,			0x03},
	{CLKRC,     0x00},
	{COM7,      0x46},
	//default value for QVGA
	{HSTART,    0x3f},
	{HSIZE,     0x50},
	{VSTRT,     0x03},
	{VSIZE,     0x78},
	{HREF,      0x00},
	{HOutSize,  ImageWidth>>2},
	{VOutSize,  ImageLength>>1},
	{EXHCH,     (ImageLength&0x01<<2)|(ImageWidth&0x03)},
	{TGT_B,     0x7f},
	{FixGain,   0x09},
	{AWB_Ctrl0, 0xe0},
	{DSP_Ctrl1, 0xbd},
	{DSP_Ctrl2, 0x0c},
	{DSP_Ctrl3,	0x00},
	{DSP_Ctrl4, 0x00},
	{DSPAuto,		0xff},
	{SCAL0,			0x0a},
	{COM8,		  0xf0},
	{COM4,		  0xc1},
	{COM6,		  0xc5},
	{COM9,		  0x21},
	{BDBase,	  0xFF},
	{BDMStep,	  0x01},
	{AEW,		    0x34},
	{AEB,		    0x3c},
	{VPT,		    0xa1},
	{EXHCL,		  0x00},
	{AWBCtrl3,  0xaa},
	{COM8,		  0xff},
	{AWBCtrl1,  0x5d},
	{EDGE1,		  0x0a},
	{DNSOff,	  0x01},
	{EDGE2,		  0x01},
	{EDGE3,		  0x01},
	{MTX1,		  0x5f},
	{MTX2,		  0x53},
	{MTX3,		  0x11},
	{MTX4,		  0x1a},
	{MTX5,		  0x3d},
	{MTX6,		  0x5a},
	{MTX_Ctrl,  0x1e},
	{BRIGHT,	  0x00},
	{CNST,		  0x25},
	{USAT,		  0x65},
	{VSAT,		  0x65},
	{UVADJ0,	  0x11},
	{UVADJ1,	  0x02},
	{SDE,		    0x06},
	{GAM1,		  0x0e},
	{GAM2,		  0x1a},
	{GAM3,		  0x31},
	{GAM4,		  0x5a},
	{GAM5,		  0x69},
	{GAM6,		  0x75},
	{GAM7,		  0x7e},
	{GAM8,		  0x88},
	{GAM9,		  0x8f},
	{GAM10,		  0x96},
	{GAM11,		  0xa3},
	{GAM12,		  0xaf},
	{GAM13,		  0xc5},
	{GAM14,		  0xd7},
	{GAM15,		  0xe8},
	{SLOP,		  0x20},
	{HUECOS,	  0x80},
	{HUESIN,	  0x80},
	{DSPAuto,	  0xff},
	{DM_LNL,	  0x00},
	{BDBase,	  0x99},
	{BDMStep,	  0x03},
	{LC_YC,			0x00},
	{LC_RADI,	  0x00},
	{LC_COEF,	  0x13},
	{LC_XC,		  0x08},
	{LC_COEFB,  0x14},
	{LC_COEFR,  0x17},
	{LC_CTR,	  0x05},
	{COM3,		  0xd0},
	{COM5,			0xf5},
	{SIGN,			0x06},
	{REG16,			0x00},
	{COM10,			0x00}
};

uint8_t cameraConfigLength = sizeof(cameraConfig)/sizeof(cameraConfig[0]);

//sccb
uint8_t cameraSccbInit(ImageType type)
{
	//sccb gpio config
	//	SCL - PA0
	//	SDA - PA1
	GPIO_InitTypeDef  GPIO_InitStructure; 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	//init config
	uint8_t ov7725ID = 0;
	if(!sccbWriteByte(COM7,0x86))
		return ERROR ;
	if(!sccbReadByte( &ov7725ID,1,PID))	
		return ERROR;
	if(ov7725ID != OV7725ID_PID)
		return ERROR;
	if(!sccbReadByte( &ov7725ID,1,VER))	
		return ERROR;
	if(ov7725ID != OV7725ID_VER)
		return ERROR;
	for(uint16_t i=0;i<cameraConfigLength;i++)
		if(!sccbWriteByte(cameraConfig[i].address,cameraConfig[i].data))
			return ERROR;
	if(type == GreyScale || type == BlackNWhite)
	{
		//8bit greyscale - capture with image, recommended
		//sccbWriteByte(COM7,0x40);
		//16bit greyscale - capture with colourImage
		//sccbWriteByte(SDE,0x26);
		//sccbWriteByte(UFix,0x80);
		//sccbWriteByte(VFix,0x80);
	}
	
	/* self define camera config here
	 * some useful config is also included
	 * you may also call the function
	 * 		sccbWriteByte(RegisterAddress,Value);
	 * in main.c to tune the value in the while loop
	 */
	if(type == SelfDefine)
	{
		// -image colour, resolution setting-
		//sccbWriteByte(COM7,);
		
		// -image size setting-
		//sccbWriteByte(HSTART,);
		//sccbWriteByte(HSIZE,);
		//sccbWriteByte(VSTRT,);
		//sccbWriteByte(VSIZE,);
		//sccbWriteByte(HREF,);
		//sccbWriteByte(HOutSize,);
		//sccbWriteByte(VOutSize,);
		//sccbWriteByte(EXHCH,);
		
		// -digital effect-
		//sccbWriteByte(SDE,);
		
		// -YUV setting-
		//sccbWriteByte(UFix,);
		//sccbWriteByte(VFix,);
		
		// -Brightness-
		//sccbWriteByte(BRIGHT,);
		
		// -Contrast-
		//sccbWriteByte(CNST,);
	}
	
	return SUCCESS;
}

void config_limit(void){
	// lower limit of y
	if(corner[0].n[1]<corner[1].n[1]){y_limit[0] = corner[0].n[1];}
	else{y_limit[0] = corner[1].n[1];}
	
	if(corner[2].n[1]>corner[3].n[1]){y_limit[1] = corner[2].n[1];}
	else{y_limit[1] = corner[3].n[1];}
	
	if(corner[0].n[0]<corner[2].n[0]){x_limit[0] = corner[0].n[0];}
	else{x_limit[0] = corner[2].n[0];}
	
	if(corner[1].n[1]>corner[3].n[0]){x_limit[1] = corner[1].n[0];}
	else{x_limit[1] = corner[3].n[0];}
	
		/*
	The explanation of the transformation will be shown here just because I might forget and mess up what I am doing :o)
	v = M*c 
	v: the coordinate of the point of the white board
	M: Homogeneous Translation Matrix
	c: the pixel in the camera
	
	With the M , we are able to transform the pixel on the camera to the 2D-plane coordinate of the White Board,
	[ v1 v2 v3 ] = M [c1 c2 c3] 
	[ v1 v2 v3 ][c1 c2 c3]^(-1)  = M 
	it will only work on the half plane, this matrix take 3 input to contruct
	
	However, it will never reach at the point on the whiteboard, since it is discrete on the camera.
	In order to fix this problem in saving stage of the we need to reverse the process by multiplying the inverse of the matrix.
	M^(-1)*v = c, we are able to get that pixel for the image from whiteboard 
	M^(-1) = c*v^(-1)

	The followibg code will only compute the M and inverse of M for both half plane.
		*/
	
	mat3 V;
	mat3 C;
	assign_col(&C,corner[TOP_LEFT],0);
	assign_col(&C,corner[BOT_LEFT],1);
	assign_col(&C,corner[TOP_RIGHT],2);
	
	vec3 temp;
	temp.n[0] = 0;
	temp.n[1] = 0;
	temp.n[2] = 1;
	
	assign_col(&V,temp,0);
	temp.n[1] = y_length;
	assign_col(&V,temp,1);
	temp.n[0] = x_length;
	temp.n[1] = 0;
	assign_col(&V,temp,2);
	
	mat3 inv_C;
	mat3 inv_V;
	
	inv_C = inverse(C); // scaled by 128
	
	//for(int i=0;i<3;i++){
	//	tft_prints(0,i,"%d %d %d",V.n[i*3+0],V.n[i*3+1],V.n[i*3+2]);
	//}
	//	tft_update();
	//	while(1);
	
	inv_V = inverse(V); // scaled by 128
	
	Upper_M = mat_prod(V,inv_C);
	Upper_M.n[8]=4096;
	
	

	//for(int i=0;i<3;i++){
	//	tft_prints(0,i,"%d %d %d",Upper_M.n[i*3+0]/1024,Upper_M.n[i*3+1]/1024,Upper_M.n[i*3+2]/1024);
	//}
	//tft_update();
	//while(1);
	
	Upper_invM = mat_prod(C,inv_V);
	Upper_invM.n[8]=4096;
	
	assign_col(&C,corner[BOT_RIGHT],0);
	temp.n[1] = y_length;
	//temp.n[0] = x_length;
	assign_col(&V,temp,0);
	
	

	inv_C = inverse(C); // scaled by 128
	inv_V = inverse(V); // scaled by 128
	
	Lower_M = mat_prod(V,inv_C);
	Lower_M.n[8]=4096;
	Lower_invM = mat_prod(C,inv_V);
	Lower_invM.n[8]=4096;
	
	//for(int i=0;i<3;i++){
	//tft_prints(0,i,"%d %d %d",Lower_M.n[i*3+0]/4096,Lower_M.n[i*3+1]/4096,Lower_M.n[i*3+2]/4096);
	//}
	//	tft_update();
	//while(1);
	
	slope = ((corner[2].n[1]-corner[1].n[1])*4096)/(corner[2].n[0]-corner[1].n[0]);
	offset = corner[2].n[1]*4096 - slope*corner[2].n[0];
	
	x_portion = 2*x_length/320;
	y_portion = 2*y_length/280;
	
	//tft_clear();
	//tft_prints(0,8,"%d",x_portion);
	//tft_prints(0,9,"%d",y_portion);
	//tft_update();
	//while(1);
	
	
}

void TestFunction(void){
	corner[0].n[0]= 58;
	corner[0].n[1]= 30;
	corner[0].n[2]= 1;
	
	corner[1].n[0]= 260;
	corner[1].n[1]= 33;
	corner[1].n[2]= 1;
	
	corner[2].n[0]= 48;
	corner[2].n[1]= 150;
	corner[2].n[2]= 1;
	
	corner[3].n[0]= 283;
	corner[3].n[1]= 152;
	corner[3].n[2]= 1;
	
	x_length = 1450;
	y_length = 870;

	config_limit();
	
}


static void sccbDelay(void)
{	
   uint16_t i = 400; 
   while(i) i--;
}

static uint8_t sccbStart(void)
{
	SDA_H;
	SCL_H;
	sccbDelay();
	if(!SDA_read)
	return DISABLE;
	SDA_L;
	sccbDelay();
	if(SDA_read) 
	return DISABLE;
	SDA_L;
	sccbDelay();
	return ENABLE;
}

static void sccbStop(void)
{
	SCL_L;
	sccbDelay();
	SDA_L;
	sccbDelay();
	SCL_H;
	sccbDelay();
	SDA_H;
	sccbDelay();
}

static void sccbAck(void)
{	
	SCL_L;
	sccbDelay();
	SDA_L;
	sccbDelay();
	SCL_H;
	sccbDelay();
	SCL_L;
	sccbDelay();
}

static void sccbNoAck(void)
{	
	SCL_L;
	sccbDelay();
	SDA_H;
	sccbDelay();
	SCL_H;
	sccbDelay();
	SCL_L;
	sccbDelay();
}

static uint8_t sccbWaitAck(void) 	
{
	SCL_L;
	sccbDelay();
	SDA_H;			
	sccbDelay();
	SCL_H;
	sccbDelay();
	if(SDA_read)
	{
      SCL_L;
      return DISABLE;
	}
	SCL_L;
	return ENABLE;
}

static void sccbSendByte(uint8_t SendByte) 
{
	uint8_t i = 8;
	while(i--)
	{
		SCL_L;
		sccbDelay();
		if(SendByte & 0x80)
			SDA_H;  
		else 
			SDA_L;   
		SendByte <<= 1;
		sccbDelay();
		SCL_H;
		sccbDelay();
	}
	SCL_L;
}

static uint8_t sccbReceiveByte(void)  
{ 
    uint8_t i = 8;
    uint8_t receiveByte = 0;
    SDA_H;				
    while(i--)
    {
      receiveByte <<= 1;      
      SCL_L;
      sccbDelay();
			SCL_H;
      sccbDelay();	
      if(SDA_read)
        receiveByte |= 0x01;
    }
    SCL_L;
    return receiveByte;
}

uint8_t sccbWriteByte(uint16_t address,uint8_t data)
{
	if(!sccbStart())
		return DISABLE;
	sccbSendByte(OV7725ADR);
	if(!sccbWaitAck())
	{
		sccbStop(); 
		return DISABLE;
	}
	sccbSendByte((uint8_t)(address & 0x00FF));     
	sccbWaitAck();	
	sccbSendByte(data);
	sccbWaitAck();   
	sccbStop(); 
	return ENABLE;
}

uint8_t sccbReadByte(uint8_t* buffer, uint16_t length, uint8_t address)
{
	if(!sccbStart())
		return DISABLE;
	sccbSendByte(OV7725ADR);
	if(!sccbWaitAck())
	{
		sccbStop(); 
		return DISABLE;
	}
	sccbSendByte(address);
	sccbWaitAck();
	sccbStop();
	if(!sccbStart())
		return DISABLE;
	sccbSendByte( OV7725ADR + 1 );
	if(!sccbWaitAck())
	{
		sccbStop(); 
		return DISABLE;
	}
	while(length)
	{
		*buffer = sccbReceiveByte();
		if(length == 1)
			sccbNoAck();
		else
			sccbAck(); 
		buffer++;
		length--;
	}
	sccbStop();
	return ENABLE;
}

//ov7725
uint8_t cameraInit(ImageType type)
{

	
	
	//check image type
	if(type != RGBColour && type != GreyScale && type != BlackNWhite)
		return ERROR;
	//sccb init
	if(!cameraSccbInit(type))
		return ERROR;
	//fifo init
	//	GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//		OE - PA15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//		RRST - PA12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//		RCLK - PB1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//		WRST - PC5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//		WEN - PC4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//		Data - PB8 to PB15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//		VSYNC - PB0
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//	NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//	EXTI
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling ; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_GenerateSWInterrupt(EXTI_Line0);
	//camera gpio init
	FIFO_OE_L();
	FIFO_WEN_H();
	//camera status init
	camState = 0;
	//set camera image type
	imageType = type;
	return SUCCESS;
}


u8 inside(u16 x,u16 y){
	vec3 v;
	v.n[0] = x;
	v.n[1] = y;
	v.n[2] = 1;
	if(in_triangle(corner[0],corner[1],corner[2],v)){
		return 1;
	}
	if(in_triangle(corner[3],corner[1],corner[2],v)){
		return 1;
	}
	return 0;
}

//corner
//     0      1 
//     2      3

u8 in_limit(s16 x,s16 y){
	if(x>=x_limit[0]&&x<=x_limit[1]){
		if(y>=y_limit[0]&&y<=y_limit[1]){
			//if(x>(Position.n[0]-20)&&x<(Position.n[0]+20)){
				//if(y>(Position.n[1]-20)&&y<(Position.n[1]+20)){
					return 1;
				}
			}
			
//		}
//	}
	return 0;	
}



inline uint16_t flipRB(uint16_t input)
{
	return (input>>11)|(input<<11)|(input&0x07E0);
}

inline uint16_t greyScale8bitTo16bit(uint8_t input)
{
	return ((input << 8) & 0xF800) | ((input << 3) & 0x07E0) | (input >> 3);
}

vec3 CameraToWhiteBoard(vec3 v){
	vec3 result;
	if(in_upper(v.n[0],v.n[1])){ // inside the top plane 
			result = mat_vec_prod(Upper_M,v);
		tft_prints(0,9,"Upper");
	}
	else { // inside the bottom plane
			result = mat_vec_prod(Lower_M,v);
		tft_prints(0,9,"Lower");
	}
	
	//tft_clear();
	//for(int i=0;i<3;i++){
//		tft_prints(0,i,"%d %d %d",Upper_M.n[i*3+0]/1024,Upper_M.n[i*3+1]/1024,Upper_M.n[i*3+2]/1024);
//	}
	
//	tft_prints(0,4,"%d %d %d",v.n[0],v.n[1],v.n[2]);
//	tft_prints(0,5,"%d %d %d",result.n[0]/1024,result.n[1]/1024,result.n[2]/1024);
//	tft_update();
//	while(1);
	
	sca_vec_div(4096,&result);
	//sca_vec_div(result.n[2],&result);
	return result;
}





vec3 WhiteBoardToCamera(vec3 v){
	vec3 bottom_left;
	vec3 top_right;
	vec3 top_left;
	vec3 result;
	
	bottom_left.n[0] = 0;
	bottom_left.n[1] = 0;
	bottom_left.n[2] = 1;
	
	top_left.n[0] = 0;
	top_left.n[1] = y_length;
	top_left.n[2] = 1;
	
	top_right.n[0] = x_length;
	top_right.n[1] = y_length;
	top_right.n[2] = 1;

	if(in_triangle(bottom_left,top_left,top_right,v)){
		result = mat_vec_prod(Upper_invM,v);
	}else{
		result = mat_vec_prod(Lower_invM,v);
	}
	sca_vec_div(128,&result);
	return result;
}




void cameraTestTftDisplay(void)
{
	static uint8_t init=0;
	sccbWriteByte(COM7, 0x46);
	if(!init)
	{
		while(!cameraInit(RGBColour));
		//while(!cameraInit(GreyScale));
		init = 1;
	}
	if(camState == 2)
	{
	//	uart_tx(UART3,"Load in to the Fucntion");
		FIFO_READY;
	//	tft_clear_line(9);
		
		tft_write_command(0x2b);		// Row addr set
		tft_write_data(0x00);
		tft_write_data(0x19);				// Y START
		tft_write_data(0x00);
		tft_write_data(0x19+ImageLength/3+1);	// Y END
		tft_write_command(0x2a);		// Column addr set
		tft_write_data(0x00);
		tft_write_data(0x01); 				// X START
		tft_write_data(0x00);
		tft_write_data(0x00+ImageWidth/3+1); 			// X END

		tft_write_command(0x2c); 		// write to RAM*/
		u16 temp = 0;
//		char r = 0;
//		char g = 0;
//		char b = 0;
		uint32_t t=get_ticks();
		for(uint16_t j=0;j<ImageLength;j++)
			for(uint16_t i=0;i<ImageWidth;i++)
			{
		/*	if(i==160&&j==120){
					i= 320;
					j=240;
					break;
				} */
				
			//	if(i%4==0&&j%4==0&&in_limit(i,j)){
				if(i%3==0&&j%3==0){
				//	READ_FIFO_GREYSCALE(temp);
					READ_FIFO_COLOUR(temp);
					tft_write_data(((temp<<3)&0xF8)|((temp>>8)&0x07));
					tft_write_data(((temp>>11))|(temp&0xE0));	
				
				}	
				else{FAKE_READ();}

			}
	//	tft_prints(0,9,"%dms per frame",get_ticks() - t);
		tft_update();
		camState = 0;
	}
}

void cameraHighResolution(void)
{
	sccbWriteByte(COM7, 0x40);
	static uint8_t init=0;
	if(!init)
	{
		while(!cameraInit(RGBColour));
		//while(!cameraInit(GreyScale));
		init = 1;
	}
	//sccbWriteByte(HOutSize,  320>>2);
	//sccbWriteByte(VOutSize,  240>>1);
	//sccbWriteByte(EXHCH, (240&0x01<<2)|(320&0x03));
	
	camState = 0;
	while(camState != 2);
	FIFO_READY;
	u8 temp = 0;
	for(uint16_t j=0;j<240;j++)
		for(uint16_t i=0;i<320;i++)
		{
			READ_FIFO_GREYSCALE(temp);

			uart_tx(UART1,"%c%c",((temp&0xF0)>>4)+64,((temp&0x0F)>>0)+64);
		}
	//sccbWriteByte(HOutSize,  ImageWidth>>2);
	//sccbWriteByte(VOutSize,  ImageLength>>1);
 //sccbWriteByte(EXHCH, (ImageLength&0x01<<2)|(ImageWidth&0x03));
	camState = 0;

}



/*
	roughImage() will store the image with resolution/16 to the 2D array rough[][],
	which can be access freely in the file.

*/

void roughImage(){
	sccbWriteByte(COM7, 0x40);
	camState = 0;
	while(camState != 2);
	FIFO_READY;
	u8 temp = 0;
	for(uint16_t j=0;j<240;j++){
		for(uint16_t i=0;i<320;i++)
		{
			if(i%4==0&&j%4==0){
			READ_FIFO_GREYSCALE(temp);
			rough[j/4][i/4]=temp;
			}else{
				FAKE_READ();
			}
		}
	}
	//sccbWriteByte(COM7, 0x46);
	camState = 0;
}

/* number : number of LED need to be detected, only provide 1 and 3 as option
   LED    : HEX number, a input for send_LED(LED) 
   cor	  : indicate which corner is searching, only when number == 1 matter
	
   When the function terminate, it will reset the LED of the robot to all off
	before return

*/

// not yet tested
void flashDetection(u8 number,u8 LED,u8 cor){
	sccbWriteByte(COM7, 0x40);
	u32 time = 0;
	send_LED(0x00);
	time = get_ticks();
	//tft_prints(0,6,"SEND_LED(0x00)");
	//tft_update();
	while(get_ticks()<time+30){}
	
	roughImage();
	
	send_LED(LED);
	//tft_prints(0,6,"SEND_LED(0x01)");
	//tft_update();
	time = get_ticks();
	while(get_ticks()<time+30){}
	
	camState = 0;
	while(camState != 2);
	FIFO_READY;
	u8 temp =0 ;
	
	if(number == 1){
	//tft_prints(0,6,"second image");
	//tft_update();
		for(uint16_t j=0;j<240;j++){
			for(uint16_t i=0;i<320;i++){
				if(i%4==0&&j%4==0){
					READ_FIFO_GREYSCALE(temp);
					if(temp - rough[j/4][i/4]>140){
						send_LED(0x00);
						if(cor==5){
							Position.n[0] = i;
							Position.n[1] = j;
							Position.n[2] = 1;
							return ;
						}
						corner[cor].n[0] = i;
						corner[cor].n[1] = j;
						corner[cor].n[2] = 1;
						time = get_ticks();
						//while(get_ticks()<time+30){}
						
						return ;
					}
				}else{FAKE_READ();}
			}
		}
			
	}
	else if(number == 3){
	u8 count =0;
	// calculate the LED position
		
	for(uint16_t j=0;j<240;j++){
		for(uint16_t i=0;i<320;i++)
		{
			if(i%4==0&&j%4==0){
			READ_FIFO_GREYSCALE(temp);
			if(temp - rough[j/4][i/4]>140){ // compare the value between the old image and the new image
				if(count==0){
					point[0].n[0]=i;
					point[0].n[1]=j;
					point[0].n[2]=1;
					count++;
				}
				for(int k=0;k<count;k++){
					vec3 curr;
					curr.n[0]=i;
					curr.n[1]=j;
					curr.n[2]=1;
					vec3 temp = vec_sub(point[k],curr);
					if(vec_length2(temp)<100){ // compare the distance bewteen the existing point and current point
						break;
					}
					if(k==count-1){
						point[count].n[0]=i;
						point[count].n[1]=j;
						point[count].n[2]=1;
						count++;
						break;
					}
				}
				
			}
			}else{FAKE_READ();}
			if(count==3){
				
				
				//point 
				//   0
				//   1      2
				vec3 ttemp;
				send_LED(0x00);
				if(point[1].n[0]>point[2].n[0]){
					ttemp = point[1];
					point[1]= point[2];
					point[2]=ttemp;
				}
				Position = point[0];
				ttemp = vec_sub(point[0],point[1]);
				y_length = vec_length(ttemp);// scaled by 128
				ttemp = vec_sub(point[1],point[2]);
				x_length = vec_length(ttemp);// scaled by 128
				corner[2] = point[1];
				while(get_ticks()<time+30){}
			/*tft_clear();
				
				tft_prints(0,0,"x: %d , y: %d",point[0].n[0],point[0].n[1]);
				tft_prints(0,1,"x: %d , y: %d",point[1].n[0],point[1].n[1]);
				tft_prints(0,2,"x: %d , y: %d",point[2].n[0],point[2].n[1]);
				tft_prints(0,3,"x: %d , y: %d",x_length/128,y_length/128 );
				tft_update();
				while(1);*/
				return ;
			}
		}
	}
	}
}


// Position Initialization allow camera know the corner and size of the whiteboard

void positionInit(){
	sccbWriteByte(COM7, 0x40);
	send_init();
	vec3 ttemp;
	// take a full image 
	// detect the lower left corner and the length of the robot in camera
	flashDetection(3,0x0B,0);
	tft_clear();
	tft_prints(0,9,"State 1 Done");
	tft_update();
	
	send_next(); // go to the lower right corner 
	while(!get_ready());
	reset_ready();
	
	flashDetection(1,0x01,3);
	tft_clear();
	tft_prints(0,9,"State 2 Done");
	tft_update();
	
	send_next();// go to the upper right corner
	
	ttemp = vec_sub(corner[2],corner[3]);//
	x_length = vec_length(ttemp)*357/x_length;

	while(!get_ready());
	reset_ready();
	
	flashDetection(1,0x04,1);
	tft_clear();
	tft_prints(0,9,"State 3 Done");
	tft_update();
	
	send_next(); // go to the upper left corner
	while(!get_ready());
	reset_ready();

	flashDetection(1,0x08,0);
	tft_clear();
	tft_prints(0,9,"State 4 Done");
	tft_update();
	
	send_next(); // go back to origin
	
	ttemp = vec_sub(corner[0],corner[2]);
	y_length = vec_length(ttemp)*254/y_length;
	
	while(!get_ready());
	reset_ready();
	sccbWriteByte(COM7, 0x46);
	camState = 0;
	config_limit();
	
}


void storeImage(void){
	sccbWriteByte(COM7, 0x40);
	camState = 0;
	
	for(u8 k = 0;k<3;k++){
	
	while(camState != 2);
	FIFO_READY;
	u8 temp = 0;
	for(uint16_t j=0;j<240;j++){
		for(uint16_t i=0;i<320;i++)
		{
			READ_FIFO_GREYSCALE(temp);
			if(k==0){
			Image[i+(j%colSize)*320] = temp/3;}else{
			Image[i+(j%colSize)*320] = temp/3 + readImage(i,j);
			}

		}
		if(j%colSize==(colSize-1)){writeRow(Image,j-colSize+1);}
	}
	camState = 0;
	}
	sccbWriteByte(COM7, 0x46);
}


void storeImage2(void){
	sccbWriteByte(COM7, 0x40);
	camState = 0;
	
	for(u8 k = 0;k<3;k++){
	
	while(camState != 2);
	FIFO_READY;
	u8 temp = 0;
	for(uint16_t j=0;j<240;j++){
		for(uint16_t i=0;i<320;i++)
		{
			READ_FIFO_GREYSCALE(temp);
			if(k==0){
			Image[i+(j%colSize)*320] = temp/3;}else{
			Image[i+(j%colSize)*320] = temp/3 + readImage2(i,j);
			}

		}
		if(j%colSize==(colSize-1)){writeRow2(Image,j-colSize+1);}
	}
	camState = 0;
	}
	sccbWriteByte(COM7, 0x46);
}





void uartImage(void){
	u8 temp = 0;
	for(u16 j=0;j<240;j++){
		for(u16 i=0;i<320;i++){
			temp = readImage(i,j);
			uart_tx(UART1,"%c%c",((temp&0xF0)>>4)+64,((temp&0x0F)>>0)+64);
			
		}
	}
}

void uartImage2(void){
	u8 temp = 0;
	for(u16 j=0;j<240;j++){
		for(u16 i=0;i<320;i++){
			temp = readImage2(i,j);
			uart_tx(UART1,"%c%c",((temp&0xF0)>>4)+64,((temp&0x0F)>>0)+64);
			
		}
	}

}

void ComputeCoordinate(u16 x,u16 y){
	vec3 WhiteBoard;
	vec3 Camera;
	Camera.n[0]=x;
	Camera.n[1]=y;
	Camera.n[2]=1;
	// Compute
	tft_clear();
	WhiteBoard = CameraToWhiteBoard(Camera);

	Position.n[0]=x;
	Position.n[1]=y;
	
	WhiteBoard.n[1] = -1*WhiteBoard.n[1]+y_length;
	send_position(WhiteBoard.n[0],WhiteBoard.n[1]);
	
	tft_prints(0,7,"x:%d",WhiteBoard.n[0]);
//	tft_prints(6,7,"x:%d",Position.n[0]);
	tft_prints(0,8,"y:%d",WhiteBoard.n[1]);
//	tft_prints(6,8,"y:%d",Position.n[1]);
	tft_update();
	return ;
}


void PositionUpdate(void){
	// slow Approach 
	
/*	send_LED(0x02);
	while(camState != 2){}

		FIFO_READY;
		u8 temp = 0;
		for(uint16_t j=0;j<ImageLength;j++){
			for(uint16_t i=0;i<ImageWidth;i++)
			{
				if(i%3==0&&j%3==0&&in_limit(i,j)){
					READ_FIFO_GREYSCALE(temp);
					if(temp>=230){
						ComputeCoordinate(i,j); //will compute the coordinate on the whiteboard and send it 
						camState = 0;
						return;
					}
				}	
				else{FAKE_READ();}

			}
	}*/
	
	flashDetection(1,2,5);
	ComputeCoordinate(Position.n[0],Position.n[1]);
	
	
}





void EXTI0_IRQHandler(void)
{
	if ( EXTI_GetITStatus(EXTI_Line0) != RESET )
	{
		if(camState == 0)
		{
			FIFO_WRST_L();
			FIFO_WEN_L();
			camState = 1;	   	
			FIFO_WEN_H();
			FIFO_WRST_H();
		}
		else if(camState == 1)
		{
			FIFO_WEN_L();
			camState = 2;
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
  
vec3 getCorner(u8 cor){
	return corner[cor];
}

u16 get_x_length(void){
	return x_length;
}

u16 get_y_length(void){
	return y_length;
}

mat3 getUpper_M(void){return Upper_M;}

u8 in_upper(u32 x,u32 y){
	if(y*4096<=slope*x+offset+10000	){return 1;}
	return 0;
}

void computeDirty(void){
{
	storeImage2();
	u16 Dirty[20][25];
	tft_clear();
	vec3 dis = vec_sub(corner[0],corner[2]);
	u16 y_space = vec_length(dis)/(128*y_portion);
	//tft_prints(0,1,"%d",y_space);
	dis = vec_sub(corner[0],corner[1]);
	u16 x_space = vec_length(dis)/(128*x_portion);
	//tft_prints(0,2,"%d",x_space);
	//tft_update();
	//while(1);
	
	u8 temp = 0;
	vec3 in_vector;
	u8 result = 0;
	for(uint16_t j=0;j<240;j++){
		for(uint16_t i=0;i<320;i++)
		{
			in_vector.n[0] = i;
			in_vector.n[1] = j;
			in_vector.n[2] = 1;
			if(in_rectangle(in_vector,corner)){
				result = readImage(i,j);
				if(result-readImage2(i,j)<0){result=0;}
				else{result -= readImage2(i,j);}
				
				
				if(result>=5){
						Dirty[(j-corner[0].n[1])/y_space][(i-corner[0].n[0])/x_space]++;
				}	
			}
			else{
				result = 0;
			}
			
			//uart_tx(UART1,"%c%c",((result&0xF0)>>4)+64,((result&0x0F)>>0)+64);
			
		}
	}
	u16 size = 0;

	u8 last = y_portion-1;
	
	for(u8 i=0;i<x_portion;i++){
		u8 min = 255;
		u8 max = 255;
		for(u8 j=0;j<y_portion;j++){
				if(Dirty[j][i]>15){
					if(!(~min)){
						min = j;
					}else{
						max=j;
					}
				}
		}
		if(~min){ // if there is min 
			if(!(~max)){ // if there is no max 
				max = min;
			}
				if((max+min)/2>last){
					if(min>0){min--;}
				   if(max<y_portion-1){max++;}
					get_target_x()[size] = i;
					get_target_y()[size] = min;
					get_target_x()[size+1] = i;
					get_target_y()[size+1] = max;
					last = max;

				}else{
					if(min>0){min--;}
				  if(max<y_portion-1){max++;}
					get_target_x()[size] = i;
					get_target_y()[size] = max;
					get_target_x()[size+1] = i;
					get_target_y()[size+1] = min;
					last = min;
				}
			
			size+=2;
		}
		
	}
	
	
	
	
	
	/*
	for(u8 j=0;j<y_portion;j++){
		for(u8 i=0;i<x_portion;i++){
			if(Dirty[j][i]>15  ){
					Dirty[j][i] = 1;
					get_target_x()[size] = i;
					get_target_y()[size] = j;
					size++;
			}else{
				Dirty[j][i]=0;
			}
			//tft_prints(i,j,"%d",Dirty[j][i]);
		}
	}
	
	
	*/
	//tft_update();
	send_target(get_target_x(),get_target_y(),size);
	camState = 0;

}



}
