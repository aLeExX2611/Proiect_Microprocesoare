#ifndef __TOUCH_SEN_H_
#define __TOUCH_SEN_H_
#include "MKL25Z4.h"

// Touch Sensor function prototypes
void Touch_Init(void);
int Touch_Scan_LH(void);
int Touch_Scan_HL(void);

// Macros
#define SCAN_OFFSET 685 // Offset for scan range
#define SCAN_DATA TSI0->DATA & 0xFFFF // Accessing the bits held in TSI0_DATA_TSICNT

#endif