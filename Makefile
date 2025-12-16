# Compiler and flags
NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

# Include directories
INCLUDES = -I./includes -I./src/config_parser -I./src/config 

# Source files
SRCS = src/webserv.cpp \
       src/config_parser/Tokenizer.cpp \
       src/config_parser/ConfigValidator.cpp \
       src/config_parser/ConfigParser.cpp \
       src/config/ConfigStructures.cpp

OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(NAME)

# Build
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean object files
clean:
	rm -f $(OBJS)

# Full clean: remove objects and binary
fclean: clean
	rm -f $(NAME)

# Rebuild everything
re: fclean all

# Phony targets
.PHONY: all clean fclean re
