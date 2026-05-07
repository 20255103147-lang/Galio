#ifndef IRQ_H
#define IRQ_H

#include "cpu.h"

/* Initialize and remap PIC */
void irq_install(void);

/* Mask/unmask specific IRQ line (0–15) */
void irq_mask(u8 irq);
void irq_unmask(u8 irq);

#endif /* IRQ_H */
