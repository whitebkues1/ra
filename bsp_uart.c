#include "hal_data.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <errno.h>

volatile bool uart_send_complete_flag = false;
volatile bool uart_receive_complete_flag = false;

int __io_putchar(int ch);
int __io_getchar(void);
int _write(int fd,char *pBuffer,int size);
int _read(int fd, char *pBuffer, int size);
int _isatty(int file);
int _close(int file);
int _lseek(int file, int ptr, int dir);
int _fstat(int file, struct stat *st);


void user_uart_callback (uart_callback_args_t * p_args)
{
    if(p_args->event == UART_EVENT_TX_COMPLETE)
    {
        uart_send_complete_flag = true;
    }

    if(UART_EVENT_RX_COMPLETE == p_args->event)
    {
        uart_receive_complete_flag = true;
    }
}

#ifdef __GNUC__
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
    #define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    R_SCI_B_UART_Write(&g_uart1_ctrl, (uint8_t *)&ch, 1);
    while(uart_send_complete_flag == false){}
    uart_send_complete_flag = false;
    return ch;
}

GETCHAR_PROTOTYPE
{
    uint8_t ch = 0;
    R_SCI_B_UART_Read(&g_uart1_ctrl, (uint8_t *)&ch, 1);
    while(uart_receive_complete_flag == false){}
    uart_receive_complete_flag = false;
    return ch;
}

int _write(int fd,char *pBuffer,int size)
{
    (void)fd;
    for(int i=0;i<size;i++)
    {
        __io_putchar(*pBuffer++);
    }
    return size;
}

int _read(int fd, char *pBuffer, int size)
{
    (void)fd;
    (void)size;
    *pBuffer = (char)__io_getchar();
    return 1;
}

int _isatty(int file)
{
    (void) file;
    return 1;
}

int _close(int file)
{
    (void) file;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    (void) file;
    (void) ptr;
    (void) dir;
    return 0;
}

int _fstat(int file, struct stat *st)
{
    (void) file;
    st->st_mode = S_IFCHR;
    return 0;
}
