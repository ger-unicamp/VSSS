#ifndef CONFIG_READER_HPP
#define CONFIG_READER_HPP

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <map>
#include "../vision/vision.hpp"

using namespace cv;

class ProgramSettings
{
  public:
	std::vector<Point2f> borders;
	std::map<std::string, Color> colors;
	std::string camera_parameters;
	bool flip;
	int camera_number;
	std::string team_color;
	std::string secondary_colors[3];

	ProgramSettings(std::string json_path, int argc, char *argv[]);

  private:
	void parse_command_line(int argc, char *argv[]);

	void print_usage(std::string name);
};

#endif