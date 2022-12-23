#include <stdint.h>
#include "TouchCapacitiv.h"
#include "ControlLED.h"
#include "Uart.h"

int main()
{
//	bsp_init();	
	UART0_Init(115200);
	Touch_Init();
		
	pwm_init();
	
	control_LED_brightness();
	
	
		return 0;
}
