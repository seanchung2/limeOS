/* i8259.h - Defines constants and functions to be used in rtc.c
 * vim:ts=4 noexpandtab
 */

#ifndef _RTC_H
#define _RTC_H

#include "types.h"

/*Definitions for RTC*/
#define RTC_REG_A			0x8A
#define	RTC_REG_B 			0x8B
#define RTC_REG_C 			0x0C
#define RTC_PORT 			0x70
#define COMS_PORT 			0x71
#define RTC_EOI				8
#define RTC_DEFAULT_FREQ 	2
#define NBYTE_DEFAULT_VAL 	4

/*Active Low Implementation Constant*/
#define ACT_LOW				0xFF
#define RTC_FRQ_ACT_LOW 	0xF0 //The low 4 bits are important for frequency as it is between 2 and 15

/*Constants for frequency calculation*/
#define MAX_FREQ			1024
#define FREQ_FOR_RATE_CALC	32768

/* initialize_RTC
 *
 * Description: Initialize the RTC
 * Inputs: None
 * Outputs: None
 * Side Effects: As description
 */
void initialize_RTC();

/* open_RTC
 *
 * Description: basically do nothing, only set the freq to default value 2
 * Inputs: filename: not used for now
 * Outputs: 0, always success
 * Side Effects: as description
 */
int32_t open_RTC(const uint8_t* filename);

/* changeFreq_RTC
 *
 * Description: Change the rtc frequency
 * Inputs: freq: The frequency that we want to change into
 * Outputs: None
 * Side Effects: As description
 */
void changeFreq_RTC(uint32_t freq);

/* read_RTC
 *
 * Description: Holding on until another RTC interrupt occur
 * Inputs: fd: Not used for now
 *		   buf: Not used for now
 *		   nbyte: Not used for now
 * Outputs: 0, Always success
 * Side Effects: As description
 */
int32_t read_RTC(int32_t fd, void* buf, int32_t nbytes);

/* write_RTC
 *
 * Description: Change the frequency
 * Inputs: fd: Not used for now
 *		   buf: A pointer
 *		   nbyte: Should be 4 (NBYTE_DEFAULT_VAL), or will not do anything
 * Outputs: 0, Success; -1, Fail
 * Side Effects: As description
 */

int32_t write_RTC(int32_t fd, const void* buf, int32_t nbytes);

/* close_RTC
 *
 * Description: Set the freq back to default value 2
 * Inputs: fd: Not used for now
 * Outputs: 0, Always success
 * Side Effects: As description
 */
int32_t close_RTC(int32_t fd);

/* RTC_handler
 *
 * Description: Only changes the RTC flag when RTC interrupt occurs
 * Inputs: None
 * Outputs: None
 * Side Effects: As description
 */

void RTC_handler();

#endif /* _RTC_H */
