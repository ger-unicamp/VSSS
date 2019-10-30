SRCDIR = vision strategy manager

CPPFILES = $(shell find $(SRCDIR) -name '*.cpp')
OBJFILES = $(patsubst %.cpp,%.o,$(CPPFILES))
CFLAGS = -g -std=c++14 -march=native -Ofast -Wall -Wextra -DNDEBUG -I/usr/local/include/opencv4 -I./json_library/single_include
CFLAGS += $(shell pkg-config --cflags opencv)

LDFLAGS = -larmadillo -pthread -L/usr/local/lib/ -lopencv_core -lopencv_imgproc
LDFLAGS += -L/usr/local/lib -lopencv_stitching -lopencv_objdetect -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core

EXEFILE = game.exe

all: $(EXEFILE)

$(EXEFILE): $(OBJFILES)
	g++ $(OBJFILES) -o $@ $(LDFLAGS)

%.o: %.cpp
	g++ -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJFILES) $(EXEFILE)
