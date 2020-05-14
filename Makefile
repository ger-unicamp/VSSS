SRCDIR = vision strategy manager

CPPFILES = $(shell find $(SRCDIR) -name '*.cpp')
OBJFILES = $(patsubst %.cpp,%.o,$(CPPFILES))
CFLAGS = -g -std=c++11 -march=native -Ofast -Wall -Wextra -DNDEBUG -I./json_library/single_include
CFLAGS += $(shell pkg-config --cflags opencv4)

LDFLAGS = -larmadillo -pthread
LDFLAGS += $(shell pkg-config --libs opencv4)

EXEFILE = game.exe

all: $(EXEFILE)

$(EXEFILE): $(OBJFILES)
	g++ $(OBJFILES) -o $@ $(LDFLAGS)

%.o: %.cpp
	g++ -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJFILES) $(EXEFILE)
