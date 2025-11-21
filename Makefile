NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iinclude -g -O2

# Source directory and files
SRC_DIR = src
SRCS = main.cpp \
		Server.cpp \
		Client.cpp \
		Parser.cpp \
		Utils.cpp \
		clientGettersSetters.cpp	\
		serverGetterSetter.cpp		\
		Channel.cpp					\
		Commands.cpp				\
		simple_debug.cpp \
		ModeHandler.cpp

# Object directory and files
OBJ_DIR = obj
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

# Progress bar variables
TOTAL_SRCS = $(words $(SRCS))
CURRENT = 0

# Default target
all: $(NAME)

# Create object directory
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@$(eval CURRENT := $(shell expr $(shell find $(OBJ_DIR) -name "*.o" | wc -l) + 1))
	@$(eval PERCENT := $(shell expr $(CURRENT) \* 100 / $(TOTAL_SRCS)))
	@$(eval FILLED := $(shell expr $(PERCENT) / 5))
	@$(eval EMPTY := $(shell expr 20 - $(FILLED)))
	@echo -n "\r🔨 Compiling ["
	@for i in `seq 1 $(FILLED)`; do echo -n "█"; done
	@for i in `seq 1 $(EMPTY)`; do echo -n "░"; done
	@echo -n "] $(PERCENT)% 📁"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Link object files to create executable
$(NAME): $(OBJS)
	@echo ""
	@echo "🔗 Linking executable..."
	@$(CXX) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo ""
	@echo "✅ $(NAME) built successfully! 🎉"
	@echo ""

# Clean object files
clean:
	@echo "🧹 Cleaning object files..."
	@rm -rf $(OBJ_DIR)

# Clean everything
fclean: clean
	@echo "🗑️  Removing executable..."
	@rm -f $(NAME)
	@echo "✨ Clean complete! 🧽"

bonus:
	@$(MAKE) -C bonus_irc

bonus_clean:
	@$(MAKE) -C bonus_irc clean

bonus_fclean:
	@$(MAKE) -C bonus_irc fclean

bonus_re:
	@$(MAKE) -C bonus_irc re

# Rebuild everything
re: fclean all

.PHONY: all clean fclean re bonus bonus_clean  bonus_clean bonus_fclean bonus_re