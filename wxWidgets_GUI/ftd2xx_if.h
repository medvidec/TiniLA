#ifndef _WINDOWS
#define Sleep(ms)  usleep(ms*1000)
#endif

int d2xx_writereg ( int addr, unsigned char data);
int d2xx_readreg ( int addr, unsigned char *data);
int d2xx_readreg_multi ( int addr, unsigned char *data, unsigned int length);
int d2xx_open( void);
int d2xx_close( void);
int d2xx_detect_tinila(void);
