PROJECT_DIR		:=		$(PWD)

TEST_CFLAGS		:=		-g -finstrument-functions -DDEBUG_PROFILE
TEST_NAME		:=		example

TEST_SRCS		:=		example.c profiling.c
TEST_OBJS		:=		$(TEST_SRCS:.c=.o)

PROFILY_CFLAGS		:= -Wall -Wextra -O2 -std=c++17

PROFILY_LDFLAGS		:=

PROFILY_NAME		:=	profily

PROFILY_SRCS_DIR	:=	src
PROFILY_OBJS_DIR	:=	objs
PROFILY_FILES		:=	main.cpp Application.cpp
PROFILY_SRCS		:=	$(addprefix $(PROFILY_SRCS_DIR)/, $(PROFILY_FILES))
PROFILY_OBJS		:=	$(addprefix $(PROFILY_OBJS_DIR)/, $(PROFILY_FILES:.cpp=.o))

.PHONY: all
all:	$(TEST_NAME) $(PROFILY_NAME)

%.o:	%.c
	gcc $(TEST_CFLAGS) -c $< -o $@

$(PROFILY_OBJS_DIR)/%.o:	$(PROFILY_SRCS_DIR)/%.cpp | $(PROFILY_OBJS_DIR)
	g++ $(PROFILY_CFLAGS) -c $< -o $@

$(TEST_NAME):	$(TEST_OBJS)
	gcc $(TEST_LDFLAGS) $(TEST_OBJS) -o $(TEST_NAME)

$(PROFILY_NAME):	$(PROFILY_OBJS)
	g++ $(PROFILY_LDFLAGS) $(PROFILY_OBJS) -o $(PROFILY_NAME)

.PHONY: clean
clean:
	rm -f $(TEST_OBJS)
	rm -f $(PROFILY_OBJS)

.PHONY: fclean
fclean:	clean
	rm -f $(TEST_NAME)
	rm -f $(PROFILY_NAME)

.PHONY: re
re: fclean $(TEST_NAME) $(PROFILY_NAME)

$(PROFILY_OBJS_DIR):
	@mkdir $(PROFILY_OBJS_DIR)