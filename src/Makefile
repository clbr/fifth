NAME = fifth

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
HDR = $(wildcard *.h)

CXXFLAGS += -Wall -Wextra
LDFLAGS += -Wl,-O1

.PHONY: all clean

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(CXXFLAGS) $(LDFLAGS)

$(OBJ): $(HDR) Makefile

clean:
	rm -f $(OBJ) $(NAME)
