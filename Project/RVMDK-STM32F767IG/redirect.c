#if 1
#include <stdio.h>
#pragma import(__use_no_semihosting)
//Support funcs that the stdlib needs
struct __FILE
{ 
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};
/* FILE is typedef’d in stdio.h. */
FILE __stdout;
/* Avoid semihosting in this way */
_sys_exit(int x)
{ 
    x = x;
}
/* Redirect the fputc */
int fputc(int ch, FILE *f)
{
//    while((USART1->ISR&0X40)==0); 
//    USART1->TDR = (u8) ch;
    return ch;
}

_ttywrch(int ch) 
{ 
ch = ch; 
}
#endif
