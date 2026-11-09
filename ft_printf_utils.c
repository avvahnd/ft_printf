#include "ft_printf.h"

int	ft_putchar(char c)
{
	return (write(1, &c, 1));
}

int	ft_putn(char const *s, char stop)
{
	size_t	len;

	len = 0;
	while (s[len] && s[len] != stop)
		len++;
	if (len == 0)
		return (0);
	return (write(1, s, len));
}

int	ft_putstr(char *s)
{
	if (!s)
		return (write(1, "(null)", 6));
	return (ft_putn(s, '\0'));
}
