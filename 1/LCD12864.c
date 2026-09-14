#include <intrins.h>
#include <sd.c>

#define uchar unsigned char
#define uint unsigned int

sbit REST = P2^0;			    											//Reset signal, active"L"
sbit C_D = P2^1;															//L:data   H:code
sbit C_E = P2^2;															//Chip enable signal, active"L"
sbit R_D = P2^3;															//read signal, active"L"
sbit W_R = P2^4;															//write signal, active"L"
#define width      				 15											//显示区宽度
#define Graphic          	     1                 
#define TXT           	    	 0
#define LcmLengthDots            128
#define LcmWidthDots             64                                                                                                                                                                                          
/**************************12864液晶的相关函数**********************/         								  
void delay_nms(uint i)
{
	while(i)
	i--;
}
void write_commond(uchar com)												//对液晶写一个指令
{	 									 	
	C_E = 0;	
	C_D = 1;
	R_D = 1;
	P0 = com;
	W_R = 0;																// write	  
	_nop_();					
	W_R = 1;																// disable write
	C_E = 1;						   	   		
	C_D = 0;
}
void write_date(uchar dat)													//对液晶写一个数据
{
	C_E = 0;
	C_D = 0;
	R_D = 1;
	P0 = dat;
	W_R = 0;
	_nop_();
	W_R = 1;
	C_E = 1;
	C_D = 1;
}
 void write_dc(uchar com,uchar dat)											//写一个指令和一个数据
{
	write_date(dat);
	write_commond(com);
}
//写一个指令和两个数据
void write_ddc(uchar com,uchar dat1,uchar dat2)
{
	write_date(dat1);
	write_date(dat2);
	write_commond(com);
}
//LCD初始化函数
void F12864_init(void)
{	
	REST = 0;
	delay_nms(2000);					
	REST = 1;
	write_ddc(0x40,0x00,0x00);			     						 		//设置文本显示区首地址
	write_ddc(0x41,128/8,0x00);  											//设置文本显示区宽度
	write_ddc(0x42,0x00,0x08);			      								//设置图形显示区首地址0x0800
	write_ddc(0x43,128/8,0x08);  											//设置图形显示区宽度
	write_commond(0xA0);                     							   	//设置光标形状 8x8方块
	write_commond(0x80);                     							   	//显示方式设置 文本and图形(异或)
	write_commond(0x92);                     							   	//设置光标
	write_commond(0x9F);                     							   	//显示开关设置 文本开,图形开,光标闪烁关							 	
}
void F12864_clear(void)
{
	unsigned int i;
	write_ddc(0x24,0x00,0x00); 												//置地址指针为从零开始
   	write_commond(0xb0); 													//自动写
	for(i = 0;i < 128 * 64 ;i++)write_date(0x00); 							//清一屏
	write_commond(0xb2); 													//自动写结束
	write_ddc(0x24,0x00,0x00); 												//重置地址指针 
}
//设定显示的地址
void goto_xy(uchar x,uchar y,uchar mode)
{
     uint  temp;
     temp = 128 / 8 * y + x;
     if(mode)                                 								//mode = 1为Graphic
     {                                        								//如果图形模式要加上图形区首地址0x0800
         temp = temp + 0x0100;
     }
     write_ddc(0x24,temp & 0xff,temp / 256);	  							//地址指针位置
}
void dprintf_hanzi_string_1(/*struct typFNT_GB16 *GB_16*/ unsigned char xdata * x,uint X_pos,uint Y_pos,uchar j,uchar k)
{  
	unsigned int address;
	unsigned char n;
	long int m = 0;
	
	while(k < j)
	{
		
		for(n = 0;n < 11;n++)                                  			  	//计数值16
    {
				address = LcmLengthDots / 8 * Y_pos + X_pos + 0x0800;
				write_ddc(0x24,(uchar)(address),(uchar)(address>>8));  			//设置显示存储器地址
				write_dc(0xc0,*x++);                    			//只用第一个地址   （其实浪费了一般的效率up注释可删）
				X_pos += 1;
		}
			X_pos += 5;
			k++;
	}
}         
/*void dprintf_hanzi_string_2(struct typFNT_GB16 *GB_16,uint X_pos,uint Y_pos,uchar j,uchar k)
{  
	unsigned int address;
	long int m = 0;
	address = LcmLengthDots / 8 * Y_pos + X_pos + 0x0800;
	while(k < j)
	{
				write_ddc(0x24,(uchar)(address),(uchar)(address>>8));  			//设置显示存储器地址
				write_dc(0xc0,GB_16[0].Mask[m++]);                    			//写入汉字字模左部
				write_dc(0xc0,GB_16[0].Mask[m++]);                 			    //写入汉字字模右部
			address = address + 128/8;
			k++;
		}
}*/
/*void dprintf_hanzi_string_3(struct typFNT_GB16 *GB_16,uint X_pos,uint Y_pos,uchar j,uchar k)
{  
	unsigned int address;
	unsigned char m,n;
	while(k < j)
	{
		m = 0;
		address = LcmLengthDots / 8 * Y_pos + X_pos + 0x0800;
		for(n = 0;n < 13;n++)                                  			  	//计数值16
    	{
		   write_ddc(0x24,(uchar)(address),(uchar)(address>>8));  			//设置显示存储器地址
		   write_dc(0xc0,GB_16[k].Mask[m++]);                    			//写入汉字字模左部
		   write_dc(0xc0,GB_16[k].Mask[m++]);                 			    //写入汉字字模右部
		   address = address + 128/8;                           			//修改显示存储器地址，显示下一列（共16列）
	    }
		X_pos += 2;
		k++;
	}
}
void dprintf_hanzi_string_4(struct typFNT_GB0 code *GB_0,uint X_pos,uint Y_pos,uchar j,uchar k)
{  
	unsigned int address;
	unsigned char m,n;
	m = 0;
	while(k < j)
	{
		
		for(n = 0;n < 6;n++)                                  			  	//计数值16
    	{
			 address = LcmLengthDots / 8 * Y_pos + X_pos + 0x0800;
		   write_ddc(0x24,(uchar)(address),(uchar)(address>>8));  			//设置显示存储器地址
		   write_dc(0xc0,GB_0[0].Mask[m++]);                    			//写入汉字字模左部
		   write_dc(0xc0,GB_0[0].Mask[m++]);                 			    //写入汉字字模右部
		   X_pos += 2;                                                //下一个八位像素
	    }
		X_pos += 4;
		k++;
	}
}*/