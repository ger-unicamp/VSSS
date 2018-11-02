SRCDIR = vision strategy manager

CPPFILES = $(shell find $(SRCDIR) -name '*.cpp')
OBJFILES = $(patsubst %.cpp,%.o,$(CPPFILES))

CFLAGS = -std=c++11 -march=native -Ofast -Wall -Wextra -DNDEBUG
CFLAGS += $(shell pkg-config --cflags opencv)

LDFLAGS = -larmadillo -pthread -L/usr/local/lib/ -lopencv_core -lopencv_imgproc
LDFLAGS += -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d
LDFLAGS += -lopencv_calib3d -lopencv_objdetect -lopencv_stitching -lopencv_imgcodecs -lopencv_videoio

EXEFILE = game.exe

all: $(EXEFILE)

$(EXEFILE): $(OBJFILES)
	g++ $(OBJFILES) -o $@ $(LDFLAGS)

%.o: %.cpp
	g++ -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJFILES) $(EXEFILE)
