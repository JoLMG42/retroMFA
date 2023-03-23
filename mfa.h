#ifndef MFA_H
# define MFA_H

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <signal.h>
#include <fcntl.h>
# include "mlx.h"

# define COLOR_16 2
# define COLOR_24 3

# define SCR_X 720
# define SCR_Y 480
# define ON_DESTROY 17
# define ESC 65307

typedef struct	s_data {
	void			*img;
	char			*addr;
	unsigned char	*raw_data;
	int				bits_per_pixel;
	int				line_length;
	int				endian;
	int16_t			w;
	int16_t			h;
	char			type;
}				t_data;

typedef struct	s_mfa {
	void	*mlx;
	void	*mlx_win;
	t_data	list_img[1000];
	t_data	background;
	int		scroll;
	int		current_img;
}	t_mfa;


int		check_file(char *file);
int		create_trgb(int t, int r, int g, int b);
void	ft_pixel_put(t_data data, int x, int y, int color);
int		hook_destroy(t_mfa *param);
int		hook_mouse(int button, int x, int y, t_mfa *param);
int		hook_key(int keycode, t_mfa *param);
int		mfa_draw_images(t_mfa mfa, int current_img);
int		main(int argc, char *argv[]);
int		create_image(t_mfa *mfa, int w, int h, int current_img);
int		read_image(t_mfa *mfa, char *filename, int header_size, int16_t *header);
void	error_message(t_mfa mfa, char *c);
void	destroy_mlx(t_mfa mfa);


#endif /* MFA_H */