#include <common.h>
#include <config.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clk.h>
#include <asm/arch/clock.h>
#include <asm/arch/uart.h>
#include "common_setup.h"
#include "exynos4412_setup.h"

void itop4412SetLedStatus(unsigned char bEnable)
{
#define GPX3CON (*(volatile unsigned int *)0x11000C60)
#define GPX3DAT (*(volatile unsigned int *)0x11000C64)
    GPX3CON = GPX3CON | (1 << 4);
    if (bEnable)
    {
        GPX3DAT = GPX3DAT | (1 << 1);
    }
    else
    {
        GPX3DAT = GPX3DAT & (~(1 << 1));
    }
    return;
}

void itop4412LedBlink(unsigned int u32BlinkTime)
{
	unsigned int u32DelayTime = 0x3000000;
	unsigned int u32Index;
	for (u32Index = 0; u32Index < u32BlinkTime; u32Index++)
	{
		itop4412SetLedStatus(1);
		sdelay(u32DelayTime);
		itop4412SetLedStatus(0);
		sdelay(u32DelayTime);
	}

	return;
}

void itop4412InitUart(void)
{
#define GPA1CON (*(volatile unsigned int *)(0x11400000 + 0x0020))

	struct exynos4x12_uart volatile *pUartBase = (struct exynos4x12_uart volatile *)samsung_get_base_uart();
	// itop4412 use uart2
	struct exynos4x12_uart volatile *pUart = pUartBase + 2;

	// set GPA1_0,GPA1_1 for uart2 mode
	GPA1CON = GPA1CON & (~0xff);
	GPA1CON = GPA1CON | (2 << 4) | (2 << 0);

	// set up uart2
	// normal mode, no parity, one stop bit, 8bit
	pUart->ulcon = (0 << 6) | (0 << 3) | (0 << 2) | (3 << 0);
	pUart->ucon = (1 << 0) | (1 << 2);
	pUart->ufcon = (0 << 0);
	pUart->ubrdiv = 53 << 0;
	pUart->ufracval = 3 << 0;
	return;
}

void itop4412PutChar(char ch)
{
	struct exynos4x12_uart volatile *pUartBase = (struct exynos4x12_uart volatile *)samsung_get_base_uart();
	// itop4412 use uart2
	struct exynos4x12_uart volatile *pUart = pUartBase + 2;

	while(!(pUart->utrstat & (1 << 1)));
	pUart->utxh = ch;
	if ('\n' == ch)
	{
		while(!(pUart->utrstat & (1 << 1)));
		pUart->utxh = '\r';
	}
}

void itop4412PutStr(char* p)
{
	while('\0' != *p)
	{
		itop4412PutChar(*p);
		if ('\n' == *p)
		{
			break;
		}
		p++;
	}
}

void itop4412PutNum(unsigned int num)
{
	char ch;
	signed char i;
	for (i = 28; i >=0; i-=4)
	{
		ch = ((num & (0xf << i)) >> i);
		if (ch <= 0x9)
		{
			ch += '0';
		}
		else
		{
			ch = ch - 0xa + 'A';
		}
	 	itop4412PutChar(ch);
	}	
}

void itop4412TestMemory(void)
{
#define DMC0_BASE (unsigned char *)0x40000000
#define DMC1_BASE (unsigned char *)0x80000000
#define DMC_CHIP_SIZE	0x40000000

	unsigned char *p;
	unsigned int index;
	itop4412LedBlink(5);
	itop4412InitUart();
	itop4412PutStr("Start test memory\n");
	p = DMC0_BASE;
	for (index = 0; index < DMC_CHIP_SIZE; index++)
	{
		*p = 0xff;
		p++;
	}
	itop4412PutStr("Write DMC0 done!!!\n");

	p = DMC1_BASE;
	for (index = 0; index < DMC_CHIP_SIZE; index++)
	{
		*p = 0xff;
		p++;
	}

	itop4412PutStr("Write DMC1 done!!!\n");

	p = DMC0_BASE;
	for (index = 0; index < DMC_CHIP_SIZE; index++)
	{
		if (0xff != *p)
		{
			itop4412PutStr("Check DMC0 error!!!\n");
			while(1);
		}
		p++;
	}

	itop4412PutStr("Check DMC0 done!!!\n");

	p = DMC1_BASE;
	for (index = 0; index < DMC_CHIP_SIZE; index++)
	{
		if (0xff != *p)
		{
			itop4412PutStr("Check DMC1 error!!!\n");
			while(1);
		}
		p++;
	}

	itop4412PutStr("Check DMC1 done!!!\n");
	
	itop4412LedBlink(5);
	return;
}

unsigned int itop4412GetPc(void)
{
	unsigned int pc_addr = 0;
	__asm__ __volatile__("mov %0, pc" :"=r"(pc_addr)::);
	return pc_addr;
}