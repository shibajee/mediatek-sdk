/* $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/linux-2.6.21.x/include/asm-sparc64/timer.h#1 $
 * timer.h: System timer definitions for sun5.
 *
 * Copyright (C) 1997 David S. Miller (davem@caip.rutgers.edu)
 */

#ifndef _SPARC64_TIMER_H
#define _SPARC64_TIMER_H

#include <linux/types.h>


struct sparc64_tick_ops {
	void (*init_tick)(unsigned long);
	unsigned long (*get_tick)(void);
	unsigned long (*get_compare)(void);
	unsigned long (*add_tick)(unsigned long, unsigned long);
	unsigned long (*add_compare)(unsigned long);
	unsigned long softint_mask;
};

extern struct sparc64_tick_ops *tick_ops;

#ifdef CONFIG_SMP
extern unsigned long timer_tick_offset;
struct pt_regs;
extern void timer_tick_interrupt(struct pt_regs *);
#endif

extern unsigned long sparc64_get_clock_tick(unsigned int cpu);

#endif /* _SPARC64_TIMER_H */
