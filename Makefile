SRCDIR = vision strategy manager

CPPFILES = $(shell find $(SRCDIR) -name '*.cpp')
OBJFILES = $(patsubst %.cpp,%.o,$(CPPFILES))

CFLAGS = -std=c++14 -march=native -Ofast -pedantic -Wall -Wextra -DNDEBUG
CFLAGS += $(shell pkg-config --cflags opencv)

LDFLAGS = -larmadillo $(shell pkg-config --libs --static opencv)

EXEFILE = game.exe

all: $(EXEFILE)

$(EXEFILE): $(OBJFILES)
	g++ $< -o $@ $(LDFLAGS)

%.o: %.cpp
	g++ -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJFILES) $(EXEFILE)
