#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <unistd.h>
# include <stdlib.h>
# include <stdarg.h>

# define DEC_BASE "0123456789"
# define HEX_LOW "0123456789abcdef"
# define HEX_UP "0123456789ABCDEF"

int	ft_putchar(char c);
int	ft_putn(char const *s, char stop);
int	ft_putstr(char *s);
int	ft_putbase(unsigned long long n, char *base, unsigned int len, int neg);
int	ft_putnbr(int n);
int	ft_putptr(void *p);

int	ft_printf(char const *format, ...);

#endif