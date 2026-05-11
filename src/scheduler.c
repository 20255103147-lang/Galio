/* scheduler.c - Round-robin scheduler with time slices */

#include "process.h"
#include "pit.h"
#include "kprintf.h"

#define TIME_SLICE_TICKS 10  /* 100ms at 100Hz */

static u32 current_ticks = 0;

/* Scheduler tick handler - called by PIT */
void scheduler_tick(void) {
    process_t *current = process_current();
    if (!current) return;

    current->ticks++;

    /* Check if time slice expired */
    if (current->ticks >= TIME_SLICE_TICKS) {
        current->ticks = 0;
        process_yield();
    }
}

/* Initialize scheduler */
void scheduler_init(void) {
    pit_install_callback(scheduler_tick);
    kprintf("Scheduler initialized with %d tick time slices\n", TIME_SLICE_TICKS);
}
