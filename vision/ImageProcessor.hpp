
#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include "vision.h"
#include "../manager/program_settings.hpp"

struct Circle
{
	Point2f center;
	float radius;
	Circle() {}
	Circle(Point2f _center, float _radius) : center(_center), radius(_radius) {}

	bool operator<(Circle a) const { return radius > a.radius; }
};

class ImageProcessor
{
	GameState game;
	bool flip;
	std::string team_color;
	ProgramSettings settings;
	std::thread processor_th;

  public:
	ImageProcessor(bool _flip, std::string _team_color, ProgramSettings &settings);
	~ImageProcessor();

	void start(VSSSBuffer<Mat> &view_buffer, VSSSBuffer<Mat> &processing_buffer,
			   VSSSBuffer<GameState> &game_buffer, int *waitkey_buf);

	void processor(VSSSBuffer<Mat> *view_buffer, VSSSBuffer<Mat> *processing_buffer,
				   VSSSBuffer<GameState> *game_buffer, int *waitkey_buf);
};

// returns the transformed field
// the four corners are found from "input" and a new image is created
Mat transform(Mat input);
// returns a binary image created from "input" where a pixel is white if
// the input pixel belongs to the color_range given
void find_color(const Mat &input, Mat &binary_image, Color color);
// finds contours with the requested color
// and stores the minimum enclosing circle for each contour in the res vector
void find_circles(const Mat &imagem, Color color_sought, vector<Circle> &res);
// set manually the four point to transform the image
void set_border_manually(Mat input, Point2f p0, Point2f p1, Point2f p2, Point2f p3);

float point_distance(Point2f a, Point2f b);

#endif
