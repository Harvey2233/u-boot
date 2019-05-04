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