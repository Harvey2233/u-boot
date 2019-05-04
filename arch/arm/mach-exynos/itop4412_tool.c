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
