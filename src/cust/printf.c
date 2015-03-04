#include <stdarg.h>    /* va_list, va_arg() */
#include <stdio.h>
#include "types.h"

#include "do_printf.h"

#ifdef HOST_CONSOLE
extern void host_console_output(unsigned c);
#endif /* HOST_CONSOLE */

extern bool  PRINT_FLAG;

/*****************************************************************************
 * PRINTF You must write your own putchar()
 *****************************************************************************/
static int vprintf_help(unsigned c, void **ptr)
{
    ptr = ptr; /* to avoid unused varible warning */
    putchar(c);
#ifdef HOST_CONSOLE
    host_console_output(c);
#endif /* HOST_CONSOLE */
    return 0;
}

static int vsprintf_help(unsigned int c, void **ptr)
{
    char *dst = *ptr;
    *dst++ = c;
    *ptr = dst;

    return 0 ;
}

int vsprintf(char *buffer, const char *fmt, va_list args)
{
    int ret_val = do_printf(fmt, args, vsprintf_help, (void *)buffer);
    buffer[ret_val] = '\0';

    return ret_val;
}

int sprintf(char *buffer, const char *fmt, ...)
{
    va_list args;
    int ret_val;

    va_start(args, fmt);
    ret_val = vsprintf(buffer, fmt, args);
    va_end(args);

    return ret_val;
}

int vprintf(const char *fmt, va_list args)
{
    return do_printf(fmt, args, vprintf_help, (void *)0);
}

/*Only use this function when you must output message in somewhere*/
/*For debug usage, please use  DBGPRINT(Level, Fmt) */
#if ((UART_SUPPORT == 1) && (DBG_MSG == 1))
void printf(const char *fmt, ...)
{
#if (MTK_DEBUG_MSG_PRINT!=1) && (MT7650_DRIVER_TEST!=1)
    va_list args;
    int ret_val;

    va_start(args, fmt);
    ret_val = vprintf(fmt, args);
    va_end(args);

    //return ret_val;
#endif
}
#endif

#if (DBG_MSG_HIGH == 1)
void printf_high(const char *fmt, ...)
{
    if (PRINT_FLAG !=TRUE)
        return;

    va_list args;
    int ret_val;

    va_start(args, fmt);
    ret_val = vprintf(fmt, args);
    va_end(args);
}
#endif

