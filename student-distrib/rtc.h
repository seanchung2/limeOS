/* i8259.h - Defines used in interactions with the RTC
 * vim:ts=4 noexpandtab
 */

#ifndef _RTC_H
#define _RTC_H

/*Definitions for RTC*/
#define	RTC_REG_B 			0x8B
#define RTC_REG_C 			0x0C
#define RTC_PORT 			0x70
#define COMS_PORT 			0x71

/*Activve Low Implementation Constant*/
#define ACT_LOW				0xFF
#define RTC_FRQ_ACT_LOW 	0xF0 //The low 4 bits are important for frequency as it is between 2 and 15

/* initialize_RTC
 *
 * Description: Initializes the RTC
 * Inputs: none
 * Outputs: none
 * Side Effects:
 */
void initialize_RTC();

int32_t open_RTC(const uint8_t* filename);

void changeFreq_RTC(uint8_t rate);

int32_t read_RTC(int32_t fd, void* buf, int32_t nbytes);

int32_t write_RTC(int32_t fd, const void* buf, int32_t nbytes);

int32_t close_RTC(int32_t fd);


#endif /* _RTC_H */