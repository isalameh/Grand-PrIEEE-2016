/*
Copyright (c) 2014, James Strawson
All rights reserved.
*/

// these pin definitions are specific to SD-101C Robotics Cape
#define CH1BIT r30.t5		// SI pin P9.27
#define CH2BIT r30.t3		// CLOCK pin P9.28


#define CONST_PRUCFG         C4
#define CONST_PRUSHAREDRAM   C28
 
#define PRU0_CTRL            0x22000
#define PRU1_CTRL            0x24000

#define CTPPR0               0x28
 
#define OWN_RAM              0x000
#define OTHER_RAM            0x020
#define SHARED_RAM           0x100

#define HALF_PULSE 2000000			//2000000 is visible
#define QUARTER_PULSE 1000000		//100000 is visible
#define NUM_OF_PULSES 130

.origin 0
.entrypoint START

START:
	LBCO	r0, CONST_PRUCFG, 4, 4		// Enable OCP master port
	CLR 	r0, r0, 4					// Clear SYSCFG[STANDBY_INIT] to enable OCP master port
	SBCO	r0, CONST_PRUCFG, 4, 4
	MOV		r0, 0x00000120				// Configure the programmable pointer register for PRU0 by setting c28_pointer[15:0]
	MOV     r0, SHARED_RAM              // Set C28 to point to shared RAM
	MOV     r1, PRU1_CTRL + CTPPR0		// Note we use beginning of shared ram unlike example which
	SBBO    r0, r1, 0, 4				// has arbitrary 2048 offset
	MOV		r9, 0x00000000				// erase r9 to use to use later
	
	MOV 	r0, QUARTER_PULSE			// r0 counter for SI HIGH/LOW
	MOV 	r1, QUARTER_PULSE 			// r1 counter for 1st CLOCK pulse HIGH/LOW
	MOV 	r2, HALF_PULSE				// r2 counter for regular clock pulses LOW
	MOV		r3, HALF_PULSE 				// r3 counter for regular clock pulses HIGH
 	MOV 	r4, NUM_OF_PULSES

	MOV 	r30, 0x00000000				// turn off GPIO outputs
	
//INTEGRATION_CYCLE:
		
// Turn on SI pulse
SI_HIGH:		
		
		MOV		r1,	QUARTER_PULSE 				// load counter value in r1 for clock high
		QBEQ	FIRST_CLOCK_HIGH, r0, 0 		// jump to clock high when counter expires
		SET 	CH1BIT							// Turn on CH1 "SI"
		SUB 	r0, r0, 1 						// decrement counter by 1
		QBA 	SI_HIGH							// stay in SI high till counter expires

	
// Turn on clock
FIRST_CLOCK_HIGH:	
		MOV		r0, QUARTER_PULSE				// Reset timer for "SI off" loop
		QBEQ	SI_LOW, r1, 0					// If timer is 0, jump to SI Low
		SET		CH2BIT							// If non-zero turn on the corresponding channel
		SUB		r1, r1, 1						// Subtract one from HALF_PULSEr
		// MOV		r1, 0x00000000				// waste a cycle for timing
		// SBCO	r1, CONST_PRUSHAREDRAM, 0, 4	// write 0 to shared memory
		QBA		FIRST_CLOCK_HIGH				// return to beginning of loop

// Turn off SI		
SI_LOW:
		MOV 	r2, HALF_PULSE 					// Reset timer for CLOCK_LOW loop
		MOV		r4, NUM_OF_PULSES				// Set the counter for clock cycles loop
		QBEQ	CLOCK_LOW, r0, 0
		CLR		CH1BIT							// turn off the corresponding channel
		SUB 	r0, r0, 1 						// decrement timer by 1
		QBA		SI_LOW							// go back to loop

// run clock for 128 cycles (NUM_OF_PULSES loaded in r4)
//CLOCK_LOOP:
		
CLOCK_LOW:
		MOV		r3, HALF_PULSE 					// set timer for clock high loop
		QBEQ 	LOOP_COUNT, r2, 0 				// after timer expires, go check how many pulses generated (out of 128)
		CLR 	CH2BIT							// Turn clock OFF
		SUB 	r2, r2, 1
		QBA 	CLOCK_LOW

LOOP_COUNT:
		MOV		r0, QUARTER_PULSE				// load counter value in r0 for SI high
		QBEQ 	SI_HIGH, r4, 0 					// when clock counter expires, go to SI high to start over again		
		SUB 	r4, r4, 1 						// decrement 128 ck loop by 1
		QBA 	CLOCK_HIGH						

CLOCK_HIGH:
		MOV		r2, HALF_PULSE
		QBEQ 	CLOCK_LOW, r3, 0
		SET 	CH2BIT
		SUB 	r3, r3, 1
		QBA 	CLOCK_HIGH
	
	HALT	// we should never actually get here