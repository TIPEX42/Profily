RAYLIB_SOURCES	:=	raylib/sources/raylib/src

all:
	@echo "Making raylib..."
	@$(MAKE) -C $(RAYLIB_SOURCES)
	@echo "Finished making raylib"

clean:
	@$(MAKE) -C $(RAYLIB_SOURCES) clean

fclean:	clean
	@$(MAKE) -C $(RAYLIB_SOURCES) clean

re:	fclean all

.PHONY: all clean fclean re