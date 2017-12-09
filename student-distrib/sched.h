#ifndef SCHED_H
#define SCHED_H

#include "terminal.h"
#include "lib.h"
#include "system_call.h"
#include "pit.h"

#define TERMINAL_COUNT 3

/* task_switch
 *
 * Description: Do the task switching
 * Inputs: None
 * Outputs: None
 * Side Effects: Page table changed
 */
void task_switch();

#endif
