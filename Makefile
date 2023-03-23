NAME		=	retromfa

CC			=	clang

CFLAGS		=	-Wall -Wextra -Werror -I. -Iminilibx-linux -I/usr/include -g3 -O3
LFLAGS		=	-Lminilibx-linux -lmlx -L/usr/lib -Iminilibx-linux -lXext -lX11 -lm -lz
ifeq ($(DEBUG), 1)
	CFLAGS		+= -DDEBUG_LOG=1
endif

SRCS		=	main.c hooks.c utils.c

OBJS		=	$(SRCS:.c=.o)
DEPS		=	$(OBJS:.o=.d)

%.o : %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

all			:	$(NAME)

$(NAME)		:	$(OBJS)
				$(MAKE) -j -C minilibx-linux
				$(CC) $(CFLAGS) $(OBJS) ${LFLAGS} -o $(NAME)

clean		:
				rm -rf $(OBJS) $(DEPS)
				$(MAKE) -j -C minilibx-linux clean

fclean		:	clean
				rm -f $(NAME)

re			:	fclean
				$(MAKE)
				$(MAKE) -j -C minilibx-linux

debug		:	fclean
				$(MAKE) DEBUG=1

-include $(DEPS)

.PHONY: all clean fclean re -include debug
