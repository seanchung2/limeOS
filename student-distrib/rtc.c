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

/* changeFreq_RTC
 *
 * Description: change the rtc frequency
 * Inputs: freq: the frequency that we want to change into
 * Outputs: none
 * Side Effects: as description
 */
void changeFreq_RTC(uint32_t freq){

	uint8_t rate = 1;

	/* if freq is over 1024, it is an error */
	if(freq>MAX_FREQ)
		return;
	while( ((FREQ_FOR_RATE_CALC)>>(rate-1)) != freq )
	{
		rate++;
		/* if freq is not the power of 2 or over 2^15, it is an error */
		if(rate>15)
			return;
	}

	uint8_t regA = 0x00;
	outb(RTC_REG_A, RTC_PORT);
	regA = inb(COMS_PORT);
	outb(RTC_REG_A, RTC_PORT);
	regA = regA & RTC_FRQ_ACT_LOW;//0xF0
	regA = regA | rate;
	outb(regA, COMS_PORT);
}

/* open_RTC
 *
 * Description: basically do nothing, only set the freq to default value 2
 * Inputs: filename: not used for now
 * Outputs: 0, always success
 * Side Effects: as description
 */
int32_t open_RTC(const uint8_t* filename){
	changeFreq_RTC(2);
	return 0;//success
}

/* read_RTC
 *
 * Description: holding on until another rtc occur
 * Inputs: fd: not used for now
 *			buf: not used for now
 *			nbyte: not used for now
 * Outputs: 0, always success
 * Side Effects: as description
 */
int32_t read_RTC(int32_t fd, void* buf, int32_t nbytes){
	rtc_interrupt_occured = 1;
	while (rtc_interrupt_occured == 1){//like a spin lock
	}
	//rtc_interrupt_occured = 1;//reset it to 1... active low methodology followed everywhere
	return 0;
}

/* write_RTC
 *
 * Description: change the frequency
 * Inputs: fd: not used for now
 *			buf: the pointer which points to the
 *			nbyte: should be 4, or will not do anything
 * Outputs: 0, success; -1, fail
 * Side Effects: as description
 */
int32_t write_RTC(int32_t fd, const void* buf, int32_t nbytes){
	if(buf == NULL || nbytes != 4)
		return -1;
	else
		changeFreq_RTC(*((int32_t*)buf));//gives the frequency
	
	return nbytes;
}

/* close_RTC
 *
 * Description: basically do nothing, only set the freq to default value 2
 * Inputs: fd: not used for now
 * Outputs: 0, always success
 * Side Effects: as description
 */
int32_t close_RTC(int32_t fd){
	changeFreq_RTC(2);//as RTC interrupts should remain open all the times, therefore we set the rate back to 2.
	return 0;//success
}

/* RTC_handler
 *
 * Description: only change the rtc flag when rtc interrupt
 * Inputs: none
 * Outputs: none
 * Side Effects: as description
 */
void RTC_handler(){
	int8_t garbage;
	rtc_interrupt_occured = 0;
	outb(0x0C, 0x70);
    garbage = inb(0x71);
    send_eoi(8);
}
