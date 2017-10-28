/*
* rtc.c - Has open, read, write, and close functions for the real-time clock (RTC) and demonstrate.
* Has the ability to change the clock frequency
*/

#include "rtc.h"
#include "lib.h"

volatile int rtc_interrupt_occured = 1;

/* initialize_RTC
 *
 * Description: Initializes the RTC
 * Inputs: none
 * Outputs: none
 * Side Effects: as description
 */
void initialize_RTC(){
	//cli();

	uint8_t regB = 0x00;//Register B of RTC
	outb(RTC_REG_B, RTC_PORT);
	regB = inb(COMS_PORT);
	regB = regB | 0x40;//This turns on bit 6
	outb(RTC_REG_B, RTC_PORT);
	outb(regB, COMS_PORT);
	//sti();

	enable_irq(2);
	enable_irq(8);
	outb(RTC_REG_C, RTC_PORT);
	inb(COMS_PORT);
}

void changeFreq_RTC(uint8_t rate){
	//Need to make sure that rate is between 2 and 15.
	if(rate > 2 && rate <=15){//checking for this as RTC has problems with rate of 1 or 2. It doesn't generate the expected interrupt
		cli();
		uint8_t regA = 0x00;
		outb(RTC_REG_A, RTC_PORT);
		regA = inb(COMS_PORT);
		outb(RTC_REG_A, RTC_PORT);
		regA = regA & RTC_FRQ_ACT_LOW;
		regA = regA | rate;
		outb(regA, COMS_PORT);
		sti();
	}
	return;
}

int32_t open_RTC(const uint8_t* filename){
	changeFreq_RTC(2);
	return 0;//success
}

int32_t read_RTC(int32_t fd, void* buf, int32_t nbytes){
	while (rtc_interrupt_occured == 1){//like a spin lock
	}
	rtc_interrupt_occured = 1;//reset it to 1... active low methodology followed everywhere
	return 0;
}

int32_t write_RTC(int32_t fd, const void* buf, int32_t nbytes){
	if(buf == NULL || nbytes != 4)
		return -1;
	else
		changeFreq_RTC(*((int32_t*)buf));
	
	return nbytes;
}

int32_t close_RTC(int32_t fd){
	changeFreq_RTC(2);//as RTC interrupts should remain open all the times, therefore we set the rate back to 2.
	return 0;//success
}
