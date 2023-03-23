#include "mfa.h"

int	check_file(char *file)
{
	int fd;

	fd = open(file, O_RDONLY);
	if (fd == -1)
	{
		printf("Error\n");
		return (0);
	}
	char buffer[5] = {0};
	read(fd, buffer, 4);
	if (strcmp(buffer, "MMF2") != 0)
	{
		close(fd);
		return (0);
	}
	close(fd);
	if (strlen(file) > 4 && strcmp(file + strlen(file) - 4, ".mfa"))
	{
		printf("Error\n");
		return (0);
	}
	return (1);
}

int	create_trgb(int t, int r, int g, int b)
{
	return (t << 24 | r * 8 << 16 | g * 8 << 8 | b * 8);
}

void	ft_pixel_put(t_data data, int x, int y, int color)
{
	char	*dst;

	if ((x >= 0 && x < SCR_X) && (y >= 0 && y < SCR_Y))
	{
		dst = data.addr + (y * data.line_length + x * \
		(data.bits_per_pixel / 8));
		*(unsigned int *)dst = color;
	}
}

void	destroy_mlx(t_mfa mfa)
{
	mlx_destroy_image(mfa.mlx, mfa.background.img);
	mlx_destroy_window(mfa.mlx, mfa.mlx_win);
	mlx_destroy_display(mfa.mlx);
	free(mfa.mlx);
}

void	error_message(t_mfa mfa, char *c)
{
	mlx_set_font(mfa.mlx, mfa.mlx_win, "8x13");
	mlx_string_put(mfa.mlx, mfa.mlx_win, SCR_X/2-(strlen(c)*4), SCR_Y/2-25, 0xD30000, c);

	mlx_hook(mfa.mlx_win, 2, 1L<<0, hook_key, &mfa);
	mlx_hook(mfa.mlx_win, ON_DESTROY, 0, hook_destroy, &mfa);

	mlx_loop(mfa.mlx);
	destroy_mlx(mfa);
}