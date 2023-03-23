#include "mfa.h"

int mfa_draw_images(t_mfa mfa, int current_img)
{
	int row = 0;
	int col = 0;
	int max_row = 0;
	for (int i = 0; i < current_img && mfa.list_img[i].img != NULL; i++)
	{
		if (row + mfa.list_img[i].w > SCR_X)
		{
			row = 0;
			col += max_row;
		}
		if (max_row == 0 || max_row < mfa.list_img[i].h)
			max_row = mfa.list_img[i].h;
		// printf("%d %d \n", col-mfa.scroll < SCR_Y,  col-mfa.scroll + mfa.list_img[i].h > 0);
		if (col-mfa.scroll < SCR_Y && col-mfa.scroll + mfa.list_img[i].h > 0)
			mlx_put_image_to_window(mfa.mlx, mfa.mlx_win, mfa.list_img[i].img, row, col-mfa.scroll);
		else if (col-mfa.scroll > SCR_Y)
			break;
		row += mfa.list_img[i].w + 5;
	}

	return (0);
}

int main(int argc, char *argv[])
{
	int16_t header[] = {0x06, 0x10, 0x0, 0x0};
	int16_t header24[] = {0x04, 0x10, 0x0, 0x0};

	if (argc < 2)
		return (1);

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	t_mfa	mfa;
	mfa.scroll = 0;
	mfa.mlx = mlx_init();
	if (!mfa.mlx)
		return 0;
	mfa.mlx_win = mlx_new_window(mfa.mlx, SCR_X, SCR_Y, argv[1]);
	if (!mfa.mlx_win)
		return 0;

	mfa.background.img = mlx_new_image(mfa.mlx, SCR_X, SCR_Y);

	if (check_file(argv[1]) == 0)
	{
		error_message(mfa, "Error files");
		return (0);
	}

	mfa.current_img = read_image(&mfa, argv[1], sizeof(header) / sizeof(int16_t), header);
	if (mfa.current_img == 0)
		mfa.current_img += read_image(&mfa, argv[1], sizeof(header24) / sizeof(int16_t), header24);
	if (mfa.current_img == 0)
	{
		error_message(mfa, "no image found");
		return (0);
	}


	mfa_draw_images(mfa, mfa.current_img);

	mlx_hook(mfa.mlx_win,2, 1L<<0, hook_key, &mfa);
	mlx_mouse_hook(mfa.mlx_win, hook_mouse, &mfa);
	mlx_hook(mfa.mlx_win, ON_DESTROY, 0, hook_destroy, &mfa);

	mlx_loop(mfa.mlx);

	for (int i = 0; i < mfa.current_img && mfa.list_img[i].img != NULL; i++)
		mlx_destroy_image(mfa.mlx, mfa.list_img[i].img);
	destroy_mlx(mfa);
}

int	create_image(t_mfa *mfa, int w, int h, int current_img)
{
	t_data *img = &mfa->list_img[current_img];
	img->w = w;
	img->h = h;
	img->img = mlx_new_image(mfa->mlx, img->w, img->h);
	img->addr = mlx_get_data_addr(img->img, &img->bits_per_pixel, &img->line_length,
								&img->endian);
	
	int color_k = 0;
	for (int i = 0; i < img->h; i++)
	{
		for (int j = 0; j < img->w; j++)
		{
			if (img->type == COLOR_16)
			{
				int color16 = (img->raw_data[color_k]) + (img->raw_data[color_k + 1] << 8);
				int r = color16 >> 10 & 0b11111;
				int g = color16 >> 5 & 0b11111;
				int b = color16 >> 0 & 0b11111;
				ft_pixel_put(*img, j, i, create_trgb(0, r, g, b));
			}
			if (img->type == COLOR_24)
			{
				int color24 = (0 << 24) + (img->raw_data[color_k]) + (img->raw_data[color_k + 1] << 8) + (img->raw_data[color_k + 2] << 16);
				ft_pixel_put(*img, j, i, color24);
			}
			color_k+=img->type;
		}
		if (img->type == COLOR_24)
			color_k+=(w%2)*img->type;
	}
	return (1);
}

int read_image(t_mfa *mfa, char *filename, int header_size, int16_t *header) {

    FILE* in_file = fopen(filename, "rb");
    if (!in_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

	int i = 0;
	int j = 0;
	int k = 0;
	int found = 0;
	int offset = 0;
	static int current_img = 0;
	unsigned char *buffer = malloc(sizeof(char) * 1024);
	if (!buffer) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	int offset_g = 0;
	while (!feof(in_file)) {
		int readed = fread(buffer, sizeof(char), 1024, in_file);
		for (i = 0; i < readed; i++) {
			if (buffer[i] == header[j])
			{
				found = 1;
				offset = i;
				for (k = 0; k < header_size; k++)
				{
					if (i+k >= 1024 || buffer[i + k] != header[k])
					{
						found = 0;
						break;
					}
				}
				if (found)
				{
					fseek(in_file, offset_g+offset-4, SEEK_SET);
					fread(buffer, sizeof(char), 4, in_file);
					int16_t w = (buffer[1] << 8) + buffer[0];
					int16_t h = (buffer[3] << 8) + buffer[2];
					// printf("0x%x: %d %d\n", offset_g+offset, w, h);
					if (w > 0 && h > 0)
					// Read w*h*3 bytes of data or * 2 if it's a 16 bit image
					{
						fseek(in_file, offset_g+offset+16, SEEK_SET);

						mfa->list_img[current_img].type = header[0] == 0x06 ? COLOR_16 : COLOR_24;
						int size_image = w * h;
						if (mfa->list_img[current_img].type == COLOR_24)
							size_image = (w+w%2) * h;

						mfa->list_img[current_img].raw_data = malloc(sizeof(char) * size_image * mfa->list_img[current_img].type);
						if (!mfa->list_img[current_img].raw_data) {
							perror("malloc");
							exit(EXIT_FAILURE);
						}
						fread(mfa->list_img[current_img].raw_data, sizeof(char), size_image * mfa->list_img[current_img].type, in_file);
						
						create_image(mfa, w, h, current_img);

						free(mfa->list_img[current_img].raw_data);
						current_img++;
					}
					fseek(in_file, offset_g+1024, SEEK_SET);
				}
			}
		}
		j = 0;
		found = 0;
		offset_g += 1024;
	}
	fclose(in_file);
	free(buffer);
	return (current_img);
}
