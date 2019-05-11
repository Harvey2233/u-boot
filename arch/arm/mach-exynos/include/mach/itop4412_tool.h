#ifndef _ITOP4412_TOOL_H_
#define _ITOP4412_TOOL_H_
void itop4412SetLedStatus(unsigned char bEnable);
void itop4412LedBlink(unsigned int u32BlinkTime);
void itop4412InitUart(void);
void itop4412PutChar(char ch);
void itop4412PutStr(char* p);
void itop4412PutNum32(unsigned int num);
void itop4412TestMemory(void);
unsigned int itop4412GetPc(void);
void itop4412ReadMem(unsigned char *start, unsigned int size);
void itop4412PutNum8(unsigned char num);
#endif
