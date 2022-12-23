#include "MKL25Z4.h"
#include <stdint.h>
#include "ControlLED.h"
#include "Uart.h"


#define RED_LED_PIN (18) // PORT B , PIN 18
#define GREEN_LED_PIN (19) // PORT B, PIN 19
#define BLUE_LED_PIN (1) // PORT D, PIN 1

void pwm_init(void)
{
	
	// Red LED 		(R)		- PTB18 - TPM2_CH0
	// Green LED 	(G)		- PTB19 - TPM2_CH1
	// Blue LED 	(B)		- PTD1 	- TPM0_CH1
	
	// Enable clock to PORTB 
	// atat pt led rosu, cat si pt led verde
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; // R+G
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; // B
	
	
	PORTB_PCR18 |= PORT_PCR_MUX(3); // R     PTB18: TPM2_CH0 (ALT3) ->Pinouts
	PORTB_PCR19 |= PORT_PCR_MUX(3); // G     PTB19: TPM2_CH1 (ALT3) ->Pinouts
	PORTD_PCR1 |= PORT_PCR_MUX(4);	// B		 PTB1 : TPM0_CH1 (ALT4) ->Pinouts
	
	
	// Select FLL as the Clock to TPM (PWM)
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	
	// Enable clock to TPM2 (PWM) Module 
	SIM->SCGC6 |= SIM_SCGC6_TPM2(1); // -> R+G
	SIM->SCGC6 |= SIM_SCGC6_TPM0(1); // -> B
	
	// Configure as edge aligned PWM
	TPM2->SC |= (TPM_SC_CPWMS(0)| TPM_SC_CMOD(1)); // -> R+G
	TPM0->SC |= (TPM_SC_CPWMS(0)| TPM_SC_CMOD(1)); // -> B
	
	// Clock prescaler is 7 (PWM clock devided by 128)
	// This makes PWM clock as 48000000/128 = 375000 Hz (375Khz)
	TPM2->SC |= TPM_SC_PS(7); // -> R+G
	TPM0->SC |= TPM_SC_PS(7);	// -> B
	
	// Configured as Edge Aligned PWM //table 31-34
	TPM2->CONTROLS[0].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK); // R
	TPM2->CONTROLS[1].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK); // G
	TPM0->CONTROLS[1].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK); // B
	
}

#define PWM_CLOCK      (48000000 / 128)    // 375 Khz
#define PWM_PERIOD			10                 // 50msec
#define PWM_PERIOD_VAL ((PWM_CLOCK * PWM_PERIOD) / 1000) // PWM_PERIOD_VAL

#define PWM_DUTY_CYCLE(x) (PWM_PERIOD_VAL - (PWM_PERIOD_VAL / x))

typedef struct
{
	uint16_t Period;
	uint16_t DutyCycle;
	char Brigtness[5];
}pwm_str;

static pwm_str LEDControl[] = {
	//   PERIOD         DUTY CYCLE     % DUTY CYCLE    HIGH (LED_OFF)  LOW (LED_ON)
	PWM_PERIOD_VAL, PWM_DUTY_CYCLE(10),   "10%",     //  90msec         10msec
	PWM_PERIOD_VAL, PWM_DUTY_CYCLE(20),   "20%",
	PWM_PERIOD_VAL, PWM_DUTY_CYCLE(30),   "30%",
	PWM_PERIOD_VAL, PWM_DUTY_CYCLE(40),   "40%",
	PWM_PERIOD_VAL, PWM_DUTY_CYCLE(50),   "50%",
	PWM_PERIOD_VAL, PWM_DUTY_CYCLE(60),   "60%",
	PWM_PERIOD_VAL, PWM_DUTY_CYCLE(70),   "70%",
	PWM_PERIOD_VAL, PWM_DUTY_CYCLE(80),   "80%",
	PWM_PERIOD_VAL, PWM_DUTY_CYCLE(90),   "90%",
	PWM_PERIOD_VAL, PWM_DUTY_CYCLE(100),  "100%",
};

#define TOTAL_BRIGHTNEES_LVLS (sizeof(LEDControl) / sizeof(LEDControl[0]))

void control_LED_brightness(void)
{	
	static int brightness_level = 0;
	int x;
	int i,j;
	int y;
	while(1)
  {    // Get the touch sensor input
      x = Touch_Scan_LH(); 
		
		  // 1400............. 100%
			// val_citita(x)..... y
			y=x*100/1400;//5535;//1700;//max 1780
			
			
		// scriere y - valoarea citita de la senzor	in procente
			int new_value[4]; 				
			char buf_temp[4]; 			 	 
			for (i = 0; i < 2; i++) 	{ 		
				new_value[i] = y % 10; 		
				y = y / 10; 		
				buf_temp[i] = new_value[i] + 48; 
			}			
			for (j = 1; j >=0; j--){
				UART0_Transmit(buf_temp[j]);
			}			
			UART0_Transmit('%');
			UART0_Transmit(' ');
			
			// afisez nivelul de intensitate asociat
			brightness_level = new_value[1];
			UART0_Transmit(brightness_level + 48);
			UART0_Transmit(' ');
			
			// afisez valoarea citita de senzor
			int new_value_x[4]; 			
			char buf_temp_x[4]; 			
			for (i = 0; i < 4; i++){ 		
				new_value_x[i] = x % 10; 		
				x = x / 10; 		
				buf_temp_x[i] = new_value_x[i] + 48; 
			}
			for (j = 3; j >=0; j--){
				UART0_Transmit(buf_temp_x[j]);
			}		
			
			UART0_Transmit(0x0D); //Carriage Return
			UART0_Transmit(0x0A); //Line Feed  
			
			
			TPM2_CNT = 0; // R+G
			TPM0_CNT = 0; // B
			
			TPM2_MOD = LEDControl[TOTAL_BRIGHTNEES_LVLS- brightness_level].Period; // R+G
			TPM0_MOD = LEDControl[TOTAL_BRIGHTNEES_LVLS - brightness_level].Period; // B
			
			TPM2_C0V = LEDControl[TOTAL_BRIGHTNEES_LVLS-brightness_level].DutyCycle; // R
			TPM2_C1V = LEDControl[TOTAL_BRIGHTNEES_LVLS-brightness_level].DutyCycle; // G
			TPM0_C1V = LEDControl[TOTAL_BRIGHTNEES_LVLS-brightness_level].DutyCycle; // B
					
			for ( i = 0; i < 290000; i++); // Delay */
		}

		 
}


