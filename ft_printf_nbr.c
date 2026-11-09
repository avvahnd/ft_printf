#include "ft_printf.h"

int	ft_putbase(unsigned long long n, char *base, unsigned int len, int neg)
{
	char	buf[24];
	int		i;

	i = 24;
	if (n == 0)
		buf[--i] = base[0];
	while (n > 0)
	{
		buf[--i] = base[n % len];
		n /= len;
	}
	if (neg)
		buf[--i] = '-';
	return (write(1, buf + i, 24 - i));
}

int	ft_putnbr(int n)
{
	if (n < 0)
		return (ft_putbase(-(long long)n, DEC_BASE, 10, 1));
	return (ft_putbase(n, DEC_BASE, 10, 0));
}

int	ft_putptr(void *p)
{
	int	head;
	int	body;

	if (!p)
		return (write(1, "(nil)", 5));
	head = (write(1, "0x", 2));
	if (head < 0)
		return (-1);
	body = ft_putbase((unsigned long long)(size_t)p, HEX_LOW, 16, 0);
	if (head < 0 || body < 0)
		return (-1);
	return (head + body);
}
