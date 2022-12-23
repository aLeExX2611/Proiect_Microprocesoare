#include "gpio.h"
#include "Uart.h"

int main()
{
	UART0_Init(115200);
	UART0_Transmit(0x0D); //Carriage Return
	UART0_Transmit(0x0A); //Line Feed
	
	Switch_Init(); // initializare PTA12	
	
	int i;
	for(;;) {
		
		/*if((UART0_S1 & UART0_S1_IDLE_MASK) ==1){
			UART0_Transmit(0X32);
		}*/
		
		if((UART0_S1 & UART0_S1_TC_MASK) !=0 ){
			UART0_Transmit(0x0D); //Carriage Return
			UART0_Transmit(0x0A); //Line Feed
			UART0_Transmit(0X30);
			UART0_Transmit(0x0D); //Carriage Return
			UART0_Transmit(0x0A); //Line Feed
			for(i=0;i<1000000;i++){}
		}
		
		
	}
}

