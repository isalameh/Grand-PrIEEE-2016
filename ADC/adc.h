/**
 * @file am335x.h
 * @author Ethan Hayon
 *
 * This file contains important config for TI am335x ARM Cortex-A8 SoC
 * 
 * PLEASE DO NOT MODIFY
 *
 * Licensed under the MIT License (MIT)
 * See MIT-LICENSE file for more information
 */

#ifndef _AM335X_H_
#define _AM335X_H_

#define MMAP_OFFSET (0x44C00000)
#define MMAP_SIZE   (0x481AEFFF-MMAP_OFFSET)

/* Clock Module Memory Registers */
#define CM_WKUP (0x44E00400)
#define CM_WKUP_ADC_TSC_CLKCTRL (CM_WKUP+0xBC)
#define CM_WKUP_MODULEMODE_ENABLE (0x02)
#define CM_WKUP_IDLEST_DISABLED (0x03<<16)

#define CM_PER (0x44E00000)
#define CM_PER_EPWMSS1_CLKCTRL (CM_PER+0xCC)
#define CM_PER_EPWMSS0_CLKCTRL (CM_PER+0xD4)
#define CM_PER_EPWMSS2_CLKCTRL (CM_PER+0xD8)


/* GPIO Memory Registers */
#define GPIO_REGISTER_SIZE (4)

#define GPIO0 	(0x44E07000)
#define GPIO1		(0x4804C000)
#define GPIO2		(0x481AC000)
#define GPIO3		(0x481AE000)

#define GPIO_CLEARDATAOUT (0x190)
#define GPIO_SETDATAOUT   (0x194)
#define GPIO_OE			      (0x134)
#define GPIO_DATAOUT      (0x13C)
#define GPIO_DATAIN       (0x138)

/* Analog Digital Converter Memory Registers */
#define ADC_TSC (0x44E0D000)

#define ADC_CTRL (ADC_TSC+0x40)
#define ADC_STEPCONFIG_WRITE_PROTECT_OFF (0x01<<2)
#define ADC_STEPENABLE (ADC_TSC+0x54)

#define ADCSTEPCONFIG1 (ADC_TSC+0x64)
#define ADCSTEPDELAY1  (ADC_TSC+0x68)
#define ADCSTEPCONFIG2 (ADC_TSC+0x6C)
#define ADCSTEPDELAY2  (ADC_TSC+0x70)
#define ADCSTEPCONFIG3 (ADC_TSC+0x74)
#define ADCSTEPDELAY3  (ADC_TSC+0x78)
#define ADCSTEPCONFIG4 (ADC_TSC+0x7C)
#define ADCSTEPDELAY4  (ADC_TSC+0x80)
#define ADCSTEPCONFIG5 (ADC_TSC+0x84)
#define ADCSTEPDELAY5  (ADC_TSC+0x88)
#define ADCSTEPCONFIG6 (ADC_TSC+0x8C)
#define ADCSTEPDELAY6  (ADC_TSC+0x90)
#define ADCSTEPCONFIG7 (ADC_TSC+0x94)
#define ADCSTEPDELAY7  (ADC_TSC+0x98)
#define ADCSTEPCONFIG8 (ADC_TSC+0x9C)
#define ADCSTEPDELAY8  (ADC_TSC+0xA0)

#define ADC_AVG1 (0b000 << 2)
#define ADC_AVG2 (0b001 << 2)
#define ADC_AVG4 (0b010 << 2)
#define ADC_AVG8 (0b011 << 2)
#define ADC_AVG16 (0b100 << 2)

#define ADC_SW_ONESHOT 0b00
#define FIFO0COUNT (ADC_TSC+0xE4)
#define FIFO_COUNT_MASK 0b01111111

#define ADC_FIFO0DATA (ADC_TSC+0x100)
#define ADC_FIFO_MASK (0xFFF)

#endif