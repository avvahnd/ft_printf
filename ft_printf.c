#include "ft_printf.h"

static int	ft_conv(char const *f, va_list *ap)
{
	char	c;

	c = f[1];
	if (c == 'c')
		return (ft_putchar(va_arg(*ap, int)));
	if (c == 's')
		return (ft_putstr(va_arg(*ap, char *)));
	if (c == 'p')
		return (ft_putptr(va_arg(*ap, void *)));
	if (c == 'd' || c == 'i')
		return (ft_putnbr(va_arg(*ap, int)));
	if (c == 'u')
		return (ft_putbase(va_arg(*ap, unsigned int), DEC_BASE, 10, 0));
	if (c == 'x')
		return (ft_putbase(va_arg(*ap, unsigned int), HEX_LOW, 16, 0));
	if (c == 'X')
		return (ft_putbase(va_arg(*ap, unsigned int), HEX_UP, 16, 0));
	if (c == '%')
		return (ft_putchar('%'));
	return (-1);
}

static int	ft_step(char const **f, va_list *ap)
{
	int	ret;

	if (**f != '%')
	{
		ret = ft_putn(*f, '%');
		if (ret > 0)
			*f += ret;
		return (ret);
	}
	ret = ft_conv(*f, ap);
	if (ret >= 0)
		*f += 2;
	return (ret);
}

int	ft_printf(char const *format, ...)
{
	va_list	ap;
	int		total;
	int		ret;

	if (!format)
		return (-1);
	total = 0;
	ret = 0;
	va_start(ap, format);
	while (ret >= 0 && *format)
	{
		ret = ft_step(&format, &ap);
		total += ret;
	}
	va_end(ap);
	if (ret < 0)
		return (-1);
	return (total);
}
