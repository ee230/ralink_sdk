/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * Putting things on the screen/serial line.
 */
#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/serial_reg.h>
#include <linux/spinlock.h>
#include <asm/io.h>

#include <asm/rt2880/surfboard.h>

static unsigned int uart_base;

static inline unsigned int serial_in(int offset)
{
//	return inb(uart_base + offset);
//kaiker
	return inl(uart_base + offset);
}

static inline void serial_out(int offset, int value)
{
//kaiker
	//outb(value, uart_base + offset);
	outl(value, uart_base + offset);
}

int putPromChar(char c)
{
	while ((serial_in(UART_LSR) & UART_LSR_THRE) == 0)
		;

	serial_out(UART_TX, c);

	return 1;
}

char getPromChar(void)
{
	while (!(serial_in(UART_LSR) & 1))
		;

	return serial_in(UART_RX);
}

void __init prom_setup_printf(int tty_no)
{
	if (tty_no == 1)
		uart_base = SURFBOARD_UART1_REGS_BASE;
	else	/* Default = ttys0 */
		uart_base = SURFBOARD_UART_REGS_BASE;
}

static spinlock_t con_lock = SPIN_LOCK_UNLOCKED;

static char buf[1024];

/* NOTE:  must call prom_setup_printf before using this function */
void __init prom_printf(char *fmt, ...)
{
	va_list args;
	int l;
	char *p, *buf_end;
	long flags;

	int putPromChar(char);

	spin_lock_irqsave(con_lock, flags);
	va_start(args, fmt);
	l = vsprintf(buf, fmt, args); /* hopefully i < sizeof(buf) */
	va_end(args);

	buf_end = buf + l;

	for (p = buf; p < buf_end; p++) {
		/* Crude cr/nl handling is better than none */
		if (*p == '\n')
			putPromChar('\r');
		putPromChar(*p);
	}
	spin_unlock_irqrestore(con_lock, flags);
}
