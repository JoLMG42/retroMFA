#include "mfa.h"

int hook_destroy(t_mfa *param)
{
	mlx_loop_end(param->mlx);
	return (0);
}

int	hook_mouse(int button, int x, int y, t_mfa *param)
{
	(void) x;
	(void) y;

	if (button == 5)
		param->scroll += 20;
	if (button == 4)
		param->scroll -= 20;
	if (button == 5 || button == 4)
	{
		// mlx_clear_window(param->mlx, param->mlx_win);
		mlx_put_image_to_window(param->mlx, param->mlx_win, param->background.img, 0, 0);
		mfa_draw_images(*param, param->current_img);
	}
	return (0);
}


int	hook_key(int keycode, t_mfa *param)
{
	if (keycode == ESC)
		mlx_loop_end(param->mlx);
	return (0);
}