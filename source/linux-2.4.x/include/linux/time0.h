/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright, Ralink Technology, Inc.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 ***************************************************************************
 */

#ifndef __TIMER0_H
#define __TIMER0_H

// extern static int timer0_aaa = 0;

int get_timer0_aaa();
struct timer0_data {
	unsigned long expires;
	unsigned long data;
	void (*tmr0_callback_function)(unsigned long);
	spinlock_t	tmr0_lock;
};

static void set_timer0_function(void (*function)(unsigned long));
#define PHYS_TO_K1(physaddr) KSEG1ADDR(physaddr)
#define sysRegRead(phys) (*(volatile unsigned int *)PHYS_TO_K1(phys))
#define sysRegWrite(phys, val)  ((*(volatile unsigned int *)PHYS_TO_K1(phys)) = (val))

#define TMRSTAT     0xA0300100  /* Timer1 Load Value */
#define TMR0LOAD    0xA0300110  /* Timer1 Load Value */
#define TMR0VAL     0xA0300114  /* Timer1 Counter Value */
#define TMR0CTL     0xA0300118  /* Timer 1 Control */

#define INTENA     0xA0300234  /* Timer 1 Control */


#define TMR0CTL_PRESCALE         0
#define TMRCTL_MODE             4
#define TMRCTL_EBL              7
#define TMRCTL_TEST             15

enum timer_mode {
    FREE_RUNNING,
    PERIODIC,
    TIMEOUT,
    WATCHDOG
};

enum timer_clock_freq {
    SYS_CLK,          /* System clock     */
    SYS_CLK_DIV4,     /* System clock /4  */
    SYS_CLK_DIV8,     /* System clock /8  */
    SYS_CLK_DIV16,    /* System clock /16 */
    SYS_CLK_DIV32,    /* System clock /32 */
    SYS_CLK_DIV64,    /* System clock /64 */
    SYS_CLK_DIV128,   /* System clock /128 */
    SYS_CLK_DIV256,   /* System clock /256 */
    SYS_CLK_DIV512,   /* System clock /512 */
    SYS_CLK_DIV1024,  /* System clock /1024 */
    SYS_CLK_DIV2048,  /* System clock /2048 */
    SYS_CLK_DIV4096,  /* System clock /4096 */
    SYS_CLK_DIV8192,  /* System clock /8192 */
    SYS_CLK_DIV16384, /* System clock /16384 */
    SYS_CLK_DIV32768, /* System clock /32768 */
    SYS_CLK_DIV65536  /* System clock /65536 */
};

static struct timer0_data tmr0;

static void start_refresh_timer_load(unsigned long timer_period)
{
    sysRegWrite(TMR0LOAD, timer_period);
}

static void set_timer_clock_prescale(enum timer_clock_freq prescale)
{
    unsigned int result;

    result=sysRegRead(TMR0CTL);
    result &= ~0xF;
    result=result | (prescale&0xF);
    sysRegWrite(TMR0CTL,result);
}

static void set_timer0_mode(enum timer_mode mode)
{
    unsigned int result;
    
    result=sysRegRead(TMR0CTL);
    result &= ~(0x3<<4);
    result=result | (mode << 4);
    sysRegWrite(TMR0CTL,result);

}

static void timer0_enable(int enable)
{
	unsigned int reg_val;

	reg_val = sysRegRead(TMR0CTL);
	if (enable == 1)
		reg_val |= (1<<TMRCTL_EBL);
	else
		reg_val &= ~(1<<TMRCTL_EBL);
	sysRegWrite(TMR0CTL, reg_val);
}

int request_tmr_service(int unit, void (*function)(unsigned long), unsigned long data);
int unregister_tmr_service();

#endif
