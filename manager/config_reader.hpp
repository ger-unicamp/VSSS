#ifndef CONFIG_READER_HPP
#define CONFIG_READER_HPP

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <map>
#include "../vision/vision.h"

using namespace cv;

class ProgramSettings {
public:
	std::vector<Point2f> borders;
	std::map<std::string, Color> colors;

	ProgramSettings(std::string json_path);
};

#endif