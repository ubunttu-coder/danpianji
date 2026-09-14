#include <REGX52.H>

/**
* @brief	???0???,?????11.0592MHz
  * @param	
  * @retval	
  */

void Timer0_Init(){
	TMOD &= 0xF0;
	TMOD |= 0x01;
	TH0 = 0xDC;
	TL0 = 0x00; //10ms
	TF0 = 0;
	TR0 = 1;
	ET0 = 1;	
	EA = 1; 	
	PT0 = 0;	
}
/* ?????????
void Timer0_Routine() interrupt 1{
	static unsigned int T0Count;
	TH0 = 0xDC;
	TL0 = 0x00;
	T0Count++;
	if(T0Count >= 100){
		T0Count = 0;

	}
}
*/
