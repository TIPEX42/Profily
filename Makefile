PROJECT_DIR		:=		$(PWD)

TEST_CFLAGS		:=		-g -finstrument-functions
TEST_LDFLAGS	:=
TEST_NAME		:=		test

TEST_SRCS		:=		test.c
TEST_OBJS		:=		$(TEST_SRCS:.c=.o)

PROFILY_INCLUDES	:=	-I$(PROJECT_DIR)/Dependencies/raylib/include
PROFILY_INCLUDES	+=	-I$(PROJECT_DIR)/Dependencies/raylib/sources/raylib/src
PROFILY_LIBS		:=	-L$(PROJECT_DIR)/Dependencies/raylib/sources/raylib/src -lraylib

PROFILY_CFLAGS		:= $(PROFILY_INCLUDES)
PROFILY_CFLAGS		+= -Wall -Wextra -O2 -std=c++17

PROFILY_LDFLAGS		:= $(PROFILY_LIBS)

PROFILY_NAME		:=	profily

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	PROFILY_LDFLAGS += -l GL -l m -l pthread -l dl -l rt -l X11
endif
ifeq ($(UNAME), Darwin)
	PROFILY_LDFLAGS += -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
endif

PROFILY_SRCS_DIR	:=	src
PROFILY_OBJS_DIR	:=	objs
PROFILY_FILES		:=	main.cpp Application.cpp
PROFILY_SRCS		:=	$(addprefix $(PROFILY_SRCS_DIR)/, $(PROFILY_FILES))
PROFILY_OBJS		:=	$(addprefix $(PROFILY_OBJS_DIR)/, $(PROFILY_FILES:.cpp=.o))

RAYLIB				:=	$(PROJECT_DIR)/Dependencies/raylib/raylib.a

.PHONY: all
all:	$(TEST_NAME) $(PROFILY_NAME)

%.o:	%.c
	gcc $(TEST_CFLAGS) -c $< -o $@

$(PROFILY_OBJS_DIR)/%.o:	$(PROFILY_SRCS_DIR)/%.cpp | $(PROFILY_OBJS_DIR)
	g++ $(PROFILY_CFLAGS) -c $< -o $@

$(TEST_NAME):	$(TEST_OBJS)
	gcc $(TEST_LDFLAGS) $(TEST_OBJS) -o $(TEST_NAME)

$(PROFILY_NAME):	$(PROFILY_OBJS) $(RAYLIB)
	g++ $(PROFILY_LDFLAGS) $(PROFILY_OBJS) -o $(PROFILY_NAME)

$(RAYLIB):
	$(MAKE) -j4 -C $(PROJECT_DIR)/Dependencies

.PHONY: clean
clean:
	rm -f $(TEST_OBJS)
	rm -f $(PROFILY_OBJS)
	$(MAKE) -j4 -C $(PROJECT_DIR)/Dependencies clean

.PHONY: fclean
fclean:	clean
	rm -f $(TEST_NAME)
	rm -f $(PROFILY_NAME)
	$(MAKE) -j4 -C $(PROJECT_DIR)/Dependencies fclean

.PHONY: re
re: fclean $(TEST_NAME) $(PROFILY_NAME)

$(PROFILY_OBJS_DIR):
	@mkdir $(PROFILY_OBJS_DIR)