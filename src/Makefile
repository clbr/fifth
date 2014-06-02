NAME = fifth

SRC = $(wildcard *.cpp) $(wildcard *.c)
TMP = $(SRC:.cpp=.o)
OBJ = $(TMP:.c=.o)
HDR = $(wildcard *.h)

CXXFLAGS += -Wall -Wextra
CFLAGS += -Wall -Wextra
CXXFLAGS += -DDATADIR=\"/usr/share/fifth\"
LDFLAGS += -Wl,-O1
LDFLAGS += -lphysfs -pthread

.PHONY: all clean

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(CXXFLAGS) $(LDFLAGS)

$(OBJ): $(HDR) Makefile version.h

version.h: ../.git
	./getver.sh

clean:
	rm -f $(OBJ) $(NAME)
