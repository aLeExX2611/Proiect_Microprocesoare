#include "gpio.h"
#include "Uart.h"

#define SWITCH_PIN (12) // PORT A



void Switch_Init(void) {
	
	// Activarea semnalului de ceas pentru a putea folosi GPIO cu ajutorul pinului 1 de pe portul A
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	// Utilizarea GPIO impune selectarea variantei de multiplexare cu valorea 1
	PORTA->PCR[SWITCH_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[SWITCH_PIN] |= PORT_PCR_MUX(1);
	
	// Activare întreruperi pe rising edge
	PORTA->PCR[SWITCH_PIN] |= PORT_PCR_IRQC(0x09) | PORT_PCR_PE_MASK;
	
	
	// Activare întrerupere pentru a folosi switch-u
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	NVIC_SetPriority(PORTA_IRQn, 128);
	NVIC_EnableIRQ(PORTA_IRQn);
}


void PORTA_IRQHandler() {
		
	// Prin utilizarea variabilei state, putem realiza un FSM
	// si sa configuram fiecare tranzitie in parte prin 
	// stingerea ledului anterior si aprinderea ledului curent
	
	
	// OBS -> nu trebuie sa configuram PDDR. Valoarea lui default este 0, adica este pin de intrare
	// Configurare pin pe post de output
	//GPIOB_PDDR |= (1<<RED_LED_PIN);
	
	
	//UART0_Transmit(0x30);
	if(GPIOA->PDIR<<SWITCH_PIN)
			UART0_Transmit(0x31);
	
	//UART0_Transmit(count+0x30);
	//int i;
	//for( i = 0; i< 29000; i++){}
	
	//UART0_Transmit(0x0D); //Carriage Return
	//UART0_Transmit(0x0A); //Line Feed
	
	PORTA_ISFR = (1<<SWITCH_PIN);
}
