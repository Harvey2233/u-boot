/*
 * Clock Initialization for board based on EXYNOS4412
 *
 * Copyright (C) 2013 Samsung Electronics
 * Rajeshwari Shinde <rajeshwari.s@samsung.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <config.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clk.h>
#include <asm/arch/clock.h>
#include "common_setup.h"
#include "exynos4412_setup.h"
#include <asm/arch/itop4412_tool.h>

/*
 * system_clock_init: Initialize core clock and bus clock.
 * void system_clock_init(void)
 */
void system_clock_init(void)
{
	struct exynos4x12_clock volatile *clk =
			(struct exynos4x12_clock volatile *)samsung_get_base_clock();

#if 0
	// for debug system clock
	itop4412LedBlink(5);
#endif

	// set up PLL_LOCKTIME
	// set max APLL lock time(100usec) for every PLL, bit[31:16] is reserved
	writel(APLL_LOCKTIME & 0x0000FFFF, &clk->apll_lock);
	writel(MPLL_LOCKTIME & 0x0000FFFF, &clk->mpll_lock);
	writel(EPLL_LOCKTIME & 0x0000FFFF, &clk->epll_lock);
	writel(VPLL_LOCKTIME & 0x0000FFFF, &clk->vpll_lock);
	
    // set up PLL control register
	// enable APLL to 1GHz
	writel(APLL_CON0_VAL, &clk->apll_con0);
	writel(APLL_CON1_VAL, &clk->apll_con1);
    // enable MPLL to 800MHz
	writel(MPLL_CON0_VAL, &clk->mpll_con0);
    writel(MPLL_CON1_VAL, &clk->mpll_con1);
	// enable EPLL to 400MHz
	writel(EPLL_CON0_VAL, &clk->epll_con0);
    writel(EPLL_CON1_VAL, &clk->epll_con1);
	// enable VPLL to 100MHz
	writel(VPLL_CON0_VAL, &clk->vpll_con0);
    writel(VPLL_CON1_VAL, &clk->vpll_con1);

	// set up DIV register
	writel(CLK_DIV_CPU0_VAL, &clk->div_cpu0);
	writel(CLK_DIV_CPU1_VAL, &clk->div_cpu1);
	writel(CLK_DIV_DMC0_VAL, &clk->div_dmc0);
	writel(CLK_DIV_DMC1_VAL, &clk->div_dmc1);
	writel(CLK_DIV_LEFTBUS_VAL, &clk->div_leftbus);
	writel(CLK_DIV_RIGHTBUS_VAL, &clk->div_rightbus);
	writel(CLK_DIV_TOP_VAL, &clk->div_top);
	writel(CLK_DIV_FSYS1_VAL, &clk->div_fsys1);
	writel(CLK_DIV_FSYS2_VAL, &clk->div_fsys2);
	writel(CLK_DIV_FSYS3_VAL, &clk->div_fsys3);
	writel(CLK_DIV_PERIL0_VAL, &clk->div_peril0);
	writel(CLK_DIV_CAM_VAL, &clk->div_cam);
	writel(CLK_DIV_MFC_VAL, &clk->div_mfc);
	writel(CLK_DIV_G3D_VAL, &clk->div_g3d);
	writel(CLK_DIV_LCD0_VAL, &clk->div_lcd);

	// wait for PLL ready
	while(!(clk->apll_con0 & (1 << 29)));
    while(!(clk->mpll_con0 & (1 << 29)));
    while(!(clk->epll_con0 & (1 << 29)));
    while(!(clk->vpll_con0 & (1 << 29)));

	// set up mux
	writel(CLK_SRC_CPU_VAL, &clk->src_cpu);
    writel(CLK_SRC_DMC_VAL, &clk->src_dmc);
    writel(CLK_SRC_TOP0_VAL, &clk->src_top0);
    writel(CLK_SRC_TOP1_VAL, &clk->src_top1);
    writel(CLK_SRC_LEFTBUS_VAL, &clk->src_leftbus);
    writel(CLK_SRC_RIGHTBUS_VAL, &clk->src_rightbus);
    writel(CLK_SRC_FSYS_VAL, &clk->src_fsys);
    writel(CLK_SRC_PERIL0_VAL, &clk->src_peril0);
    writel(CLK_SRC_CAM_VAL, &clk->src_cam);
    writel(CLK_SRC_MFC_VAL, &clk->src_mfc);
    writel(CLK_SRC_G3D_VAL, &clk->src_g3d);
    writel(CLK_SRC_LCD0_VAL, &clk->src_lcd);

#if 0
		// for debug system clock
		itop4412LedBlink(5);
#endif
}
