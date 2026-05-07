#ifndef PIT_H
#define PIT_H

#include "common.h"
#include "cpu.h"

/* Programmable Interval Timer (PIT) driver */

typedef void (*timer_callback_t)(void);

/* Initialize PIT */
void pit_init(u32 frequency);

/* Get current tick count */
u32 pit_get_ticks(void);

/* Install user timer callback */
void pit_install_callback(timer_callback_t callback);

/* Enable/disable timer */
void pit_enable(void);
void pit_disable(void);

#endif /* PIT_H */
