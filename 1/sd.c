#include <AT89X52.H>

#define F_OSC  12000000//晶振平率Hz  11059200 
#define F_BAUD 9600
#define RELOAD 256-F_OSC/12/32/F_BAUD
#define CR 0x0D        //回车

//定义SD卡需要的4根信号线
sbit SD_CLK = P1^4;
sbit SD_DI  = P1^6;
sbit SD_DO  = P1^5;
sbit SD_CS  = P1^7;

//===========================================================
//写一字节到SD卡,模拟SPI总线方式
void SdWrite(unsigned char n)
{

    unsigned char i;

    for(i=8;i;i--)
    {
        SD_CLK=0;
        SD_DI=(n&0x80);
        n<<=1;
        SD_CLK=1;
        }
        SD_DI=1; 
    } 
//===========================================================
//从SD卡读一字节,模拟SPI总线方式
unsigned char SdRead()
{
    unsigned char n,i;
    for(i=8;i;i--)
    {
        SD_CLK=0;
        SD_CLK=1;
        n<<=1;
        if(SD_DO) n|=1;

    }
    return n;
}
//============================================================
//检测SD卡的响应
unsigned char SdResponse()
{
    unsigned char i=0,response;

    while(i<=8)
    {
        response = SdRead();
        if(response==0x00)
        break;
        if(response==0x01)
        break;
        i++;
    }
    return response;
} 
//================================================================
//发命令到SD卡
void SdCommand(unsigned char command, unsigned long argument, unsigned char CRC)
{

    SdWrite(command|0x40);
		SdWrite(argument>>24);
		SdWrite(argument>>16);
		SdWrite(argument>>8);
		SdWrite(argument);
    SdWrite(CRC);
}
//================================================================
//初始化SD卡
unsigned char SdInit(void)
{
    int delay=0, trials=0;
    unsigned char i;
    unsigned char response=0x01;

    SD_CS=1;
    for(i=0;i<=9;i++)
    SdWrite(0xff);
    SD_CS=0;

    //Send Command 0 to put MMC in SPI mode
    SdCommand(0x00,0,0x95);


    response=SdResponse();

    if(response!=0x01)
    {
        return 0;
    } 

    while(response==0x01)
    {
        SD_CS=1;
        SdWrite(0xff);
        SD_CS=0;
        SdCommand(0x01,0x00ffc000,0xff);
        response=SdResponse();
    } 

    SD_CS=1;
    SdWrite(0xff);
    return 1; 
}
//================================================================
//往SD卡指定地址写数据,一次最多512字节
unsigned char SdWriteBlock(unsigned char *Block, unsigned long address,int len)
{
    unsigned int count;
    unsigned char dataResp;
    //Block size is 512 bytes exactly
    //First Lower SS

    SD_CS=0;
    //Then send write command
    SdCommand(0x18,address,0xff);

    if(SdResponse()==00)
    {
        SdWrite(0xff);
        SdWrite(0xff);
        SdWrite(0xff);
        //command was a success - now send data
        //start with DATA TOKEN = 0xFE
        SdWrite(0xfe);
        //now send data
        for(count=0;count<len;count++) SdWrite(*Block++);

        for(;count<512;count++) SdWrite(0);
        //data block sent - now send checksum
        SdWrite(0xff); //两字节CRC校验, 为0XFFFF 表示不考虑CRC
        SdWrite(0xff);
        //Now read in the DATA RESPONSE token
        dataResp=SdRead();
        //Following the DATA RESPONSE token
        //are a number of BUSY bytes
        //a zero byte indicates the MMC is busy

        while(SdRead()==0);

        dataResp=dataResp&0x0f; //mask the high byte of the DATA RESPONSE token
        SD_CS=1;
        SdWrite(0xff);
        if(dataResp==0x0b)
        {
        //printf("DATA WAS NOT ACCEPTED BY CARD -- CRC ERROR\n");
        return 0;
        }
        if(dataResp==0x05)
        return 1;

        //printf("Invalid data Response token.\n");
        return 0;
    }
    //printf("Command 0x18 (Write) was not received by the MMC.\n");
    return 0;
}

//=======================================================================
//从SD卡指定地址读取数据,一次最多512字节
unsigned char SdReadBlock(unsigned char *Block, unsigned long address,int len)
{
    unsigned int count;
    //Block size is 512 bytes exactly
    //First Lower SS

     //printf("MMC_read_block\n");

    SD_CS=0;
    //Then send read command
    SdCommand(0x11,address,0xff);

    if(SdResponse()==00)
    {
        //command was a success - now send data
        //start with DATA TOKEN = 0xFE
        while(SdRead()!=0xfe);

        for(count=0;count<len;count++) *Block++=SdRead(); 

        for(;count<512;count++) SdRead();

        //data block sent - now send checksum
        SdRead();
        SdRead();
        //Now read in the DATA RESPONSE token
        SD_CS=1;
				SdWrite(0xff);
        return 1;
    }
//printf("Command 0x11 (Read) was not received by the MMC.\n");
    return 0;
}

/*******************************************
         串口初始化
*******************************************/
void UART()                                        //用不着的函数
{
    SCON=0x40;//工作与方式1不允许接受
    TMOD=0x20;//定时器1工作与方式2自动重装模式
    TH1=RELOAD;
    TR1=1;
    TI=0;   
}

/********************************************************************
* 名称 : Com_Init()
* 功能 : 初始化串口程序，晶振11.0592, 波特率9600
* 输入 : 无
* 输出 : 无
**********************网上找的另一个串口初始化************************
***********************************************************************/
void Com_Init(void)                                //用不着的函数
{
TMOD = 0x20;
PCON = 0x00;
SCON = 0x50;
TH1 = 0xFd;
TL1 = 0xFd;
TR1 = 1;
}
//============================================================

//通过串口发送一个字符串
void Sen_String(unsigned char *string)               //用不着的函数
{
    while(*string!='\0')
    {
        if(*string=='\n')
        {
            SBUF=CR;
        }
        else
        {
            SBUF=*string;
        }
        while(TI==0);
        TI=0;
        string++;
    }
}