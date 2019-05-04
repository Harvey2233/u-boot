/*
 * Memory setup for board based on EXYNOS4210
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

#include <config.h>
#include <asm/arch/dmc.h>
#include "common_setup.h"
#include "exynos4412_setup.h"
#include <asm/arch/itop4412_tool.h>

struct mem_timings mem = {
	.direct_cmd_msr = {
		DIRECT_CMD1, DIRECT_CMD2, DIRECT_CMD3, DIRECT_CMD4, DIRECT_CMD5,
	},
	.timingref = TIMINGREF_VAL,
	.timingrow = TIMINGROW_VAL,
	.timingdata = TIMINGDATA_VAL,
	.timingpower = TIMINGPOWER_VAL,
	.zqcontrol = ZQ_CONTROL_VAL,
	.control0 = CONTROL0_VAL,
	.control1 = CONTROL1_VAL,
	.control2 = CONTROL2_VAL,
	.concontrol = CONCONTROL_VAL,
	.prechconfig = PRECHCONFIG,
	.memcontrol = MEMCONTROL_VAL,
	.memconfig0 = MEMCONFIG0_VAL,
	.memconfig1 = MEMCONFIG1_VAL,
	.dll_resync = FORCE_DLL_RESYNC,
	.dll_on = DLL_CONTROL_ON,
};
static void phy_control_reset(int ctrl_no, struct exynos4_dmc *dmc)
{
	if (ctrl_no) {
		writel((mem.control1 | (1 << mem.dll_resync)),
		       &dmc->phycontrol1);
		writel((mem.control1 | (0 << mem.dll_resync)),
		       &dmc->phycontrol1);
	} else {
		writel((mem.control0 | (0 << mem.dll_on)),
		       &dmc->phycontrol0);
		writel((mem.control0 | (1 << mem.dll_on)),
		       &dmc->phycontrol0);
	}
}

static void dmc_config_mrs(struct exynos4_dmc *dmc, int chip)
{
	int i;
	unsigned long mask = 0;

	if (chip)
		mask = DIRECT_CMD_CHIP1_SHIFT;

	for (i = 0; i < MEM_TIMINGS_MSR_COUNT; i++) {
		writel(mem.direct_cmd_msr[i] | mask,
		       &dmc->directcmd);
	}
}

static void dmc_init(struct exynos4_dmc *dmc)
{
	/**
	 * 步骤归类
	 * 1-6,14,15,16:初始化PHY DLL
	 * 7-11,28,29:初始化DMC
	 * 19-27:初始化DDR
	 */

	/**
	 * Setup 2:
	 *	PhyControl1.term_write_en = 1
	 *	PhyControl1.term_read_en = 1
	 */
	writel(mem.control1, &dmc->phycontrol1);

	/**
	 * Stup 3:
	 *	PhyZQControl.ctrl_zq_mode_noterm = 1
	 *	PhyZQControl.ctrl_zq_start = 1
	 */
	writel(mem.zqcontrol, &dmc->phyzqcontrol);


	/**
	 * Setup 4:
	 *	PhyControl0.ctrl_start_point = 0x10
	 *	PhyControl0.ctrl_inc = 0x10
	 *	PhyControl0.ctrl_dll_on = 1
	 */
	phy_control_reset(0, dmc);

	/**
	 * Setup 5:
	 *	PhyControl1.ctr_shiftc = 0x6
	 *	PhyControl1.ctrl_offsetc = 0x0
	 */
	writel(mem.control1, &dmc->phycontrol1);

	/**
	 * Setup 6:
	 *	PhyControl0.ctrl_start = 1
	 */
	writel((mem.control0 | CTRL_DLL_ON_EN | CTRL_START_EN), &dmc->phycontrol0);


	/**
	 * Setup 7:
	 *	ConControl.aref_en = 0
	 */
	writel(mem.concontrol, &dmc->concontrol);


	/**
	 * Setup 8:
	 *	MemControl.dpwrdn_en = 0
	 *	MemControl.pzq_en = 0
	 *	chip是rank
	 */
	writel(mem.memcontrol, &dmc->memcontrol);


	/**
	 * Setup 9:
	 *	set Memory info
	 */
	writel(mem.memconfig0, &dmc->memconfig0);


	/**
	 * Setup 10:
	 *	set PrechConfig
	 */
	writel(mem.prechconfig, &dmc->prechconfig);


	/**
	 * Setup 11:
	 *	set TimingAref, TimingRow, TimingData and TimingPower
	 */
	writel(mem.timingref, &dmc->timingref);
	writel(mem.timingrow, &dmc->timingrow);
	writel(mem.timingdata, &dmc->timingdata);
	writel(mem.timingpower, &dmc->timingpower);


	/**
	 * Setup 13, 14:
	 *	wait PhyStatus0.ctrl_clock and PhyStatus0.ctrl_flock to 1
	 */
	while((dmc->phystatus & 0x7) != 0x7);
	writel((dmc->phycontrol0 | ((dmc->phystatus & 0x3FC)<<18))
			, &dmc->phycontrol0);

	/**
	 * Setup 15,16:
	 *	set PhyContron1.fp_resync to 1
	 */
	phy_control_reset(1, dmc);

	/**
	 * Setup 19:
	 *	NOP command
	 *	hold CKE to logic high level
	 *	chip 0
	 */
	writel(DIRECT_CMD_NOP, &dmc->directcmd);

	/**
	 * Setup 21:
	 *	send EMRS2 command
	 *	send EMRS3 command
	 *	send EMRS command
	 *	send MRS command
	 *	chip 0
	 */
	dmc_config_mrs(dmc, 0);

	/**
	 *Setup 26:
	 send ZQINIT command
	 chip 0
	 */
	writel(DIRECT_CMD_ZQ, &dmc->directcmd);


	/**
	 * Setup 28:
	 *	set ConControl auto refresh
	 */
	writel((mem.concontrol | AREF_ON), &dmc->concontrol);

	/**
	 * Setup 29:
	 *	set MemControl
	 */
	writel((mem.memcontrol | MEMCONTROL_END), &dmc->memcontrol);

}

void mem_ctrl_init(int reset)
{
	struct exynos4_dmc *dmc0;
	struct exynos4_dmc *dmc1;

	dmc0 = (struct exynos4_dmc *)samsung_get_base_dmc_ctrl();
	dmc1 = (struct exynos4_dmc *)(samsung_get_base_dmc_ctrl()
					+ DMC_OFFSET);
	
	writel(APB_SFR_INTERLEAVE_CONF_VAL, &dmc0->ivcontrol);
	writel(APB_SFR_INTERLEAVE_CONF_VAL, &dmc1->ivcontrol);

	dmc_init(dmc0);
	dmc_init(dmc1);
#if 0
	// test memory
	itop4412TestMemory();
#endif
}
