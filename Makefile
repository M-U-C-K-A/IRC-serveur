# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/16 02:31:44 by hdelacou          #+#    #+#              #
#    Updated: 2025/12/16 04:10:23 by hdelacou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================================================================ #
#                                  SETTINGS                                    #
# ============================================================================ #

NAME        := ircserv

# Compiler and Flags
CXX         := c++
CXXFLAGS    := -Wall -Wextra -Werror -std=c++98 -g3
DEPFLAGS    := -MMD -MP

# Directories
SRCDIR      := srcs
OBJDIR      := objs
INCDIR      := includes

# ============================================================================ #
#                                   SOURCES                                    #
# ============================================================================ #

# Root sources (srcs/)
SRCS_ROOT   := main.cpp \
               User.cpp \
               Channel.cpp \
               Utils.cpp \
               IrcReplies.cpp

# Combine sources with paths
SRCS        := $(addprefix $(SRCDIR)/, $(SRCS_ROOT))

# Objects and Dependencies
OBJS        := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
DEPS        := $(OBJS:.o=.d)

# ============================================================================ #
#                                  COLORS                                      #
# ============================================================================ #

RESET       := \033[0m
BOLD        := \033[1m
DIM         := \033[2m

RED         := \033[91m
GREEN       := \033[92m
YELLOW      := \033[93m
BLUE        := \033[94m
MAGENTA     := \033[95m
CYAN        := \033[96m

# Specific colors for output
PREFIX      := $(BOLD)$(CYAN)[IRC]$(RESET)
SUCCESS     := $(BOLD)$(GREEN)✓$(RESET)
FAILURE     := $(BOLD)$(RED)✗$(RESET)

# ============================================================================ #
#                                   RULES                                      #
# ============================================================================ #

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(PREFIX) Linking objects..."
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(PREFIX) $(SUCCESS) Build complete! Run with ./$(NAME) <port> <password>"

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@printf "$(PREFIX) $(CYAN)[%2d/%2d]$(RESET) Compiling $(BOLD)$(YELLOW)%-30s$(RESET) " \
		$$(expr $$(find $(OBJDIR) -name '*.o' 2>/dev/null | wc -l) + 1) \
		$$(echo $(SRCS) | wc -w) "$(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(DEPFLAGS) -I$(INCDIR) -c $< -o $@ \
		&& echo "$(SUCCESS)" || (echo "$(FAILURE)" && exit 1)

# Include dependencies
-include $(DEPS)

# Clean objects
clean:
	@echo "$(PREFIX) Cleaning object files..."
	@rm -rf $(OBJDIR)
	@echo "$(PREFIX) $(SUCCESS) Objects cleaned."

# Full clean
fclean: clean
	@echo "$(PREFIX) Removing executable..."
	@rm -f $(NAME)
	@echo "$(PREFIX) $(SUCCESS) Executable removed."

# Rebuild
re: fclean all

# Bonus (same as all for now)
bonus: all

# ============================================================================ #
#                                   UTILS                                      #
# ============================================================================ #

# Debugging helper
info:
	@echo "$(PREFIX) Source files: $(words $(SRCS))"
	@echo "$(PREFIX) Object dir:   $(OBJDIR)"
	@echo "$(PREFIX) Compiler:     $(CXX) $(CXXFLAGS)"

help:
	@echo "$(PREFIX) Available targets:"
	@echo "$(PREFIX) $(YELLOW)all$(RESET) - Build the project"
	@echo "$(PREFIX) $(YELLOW)clean$(RESET) - Clean object files"
	@echo "$(PREFIX) $(YELLOW)fclean$(RESET) - Full clean (remove executable and object files)"
	@echo "$(PREFIX) $(YELLOW)re$(RESET) - Rebuild the project"
	@echo "$(PREFIX) $(YELLOW)bonus$(RESET) - Build the bonus"
	@echo "$(PREFIX) $(YELLOW)info$(RESET) - Display build information"
	@echo "$(PREFIX) $(YELLOW)help$(RESET) - Display this help message"

.PHONY: all clean fclean re bonus info
