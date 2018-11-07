#include "ImageProcessor.hpp"

#include <opencv2/opencv.hpp>

typedef std::chrono::duration<long long, std::ratio<1ll, 1000000000ll>> duration;

ImageProcessor::ImageProcessor(bool _flip, std::string _team_color, ProgramSettings &_settings) : flip(_flip), team_color(_team_color), settings(_settings)
{
}

ImageProcessor::~ImageProcessor()
{
	if ((this->processor_th).joinable())
	{
		(this->processor_th).join();
	}
}

void ImageProcessor::start(VSSSBuffer<vector<Mat>> &view_buffer, VSSSBuffer<Mat> &processing_buffer,
						   VSSSBuffer<GameState> &game_buffer, int *waitkey_buf)
{
	this->processor_th = std::thread(&ImageProcessor::processor, this, &view_buffer, &processing_buffer,
									 &game_buffer, waitkey_buf);
}

typedef std::chrono::duration<long long, std::ratio<1ll, 1000000000ll>> duration;

void ImageProcessor::processor(VSSSBuffer<vector<Mat>> *view_buffer, VSSSBuffer<Mat> *processing_buffer,
							   VSSSBuffer<GameState> *game_buffer, int *waitkey_buf)
{

	Mat frame;
	set_border_manually(frame, settings.borders[0], settings.borders[1], settings.borders[2], settings.borders[3]);
	Mat transformed_frame;

	int sw_key = 0;

	while (*waitkey_buf != ESC_CHAR)
	{
		this->game = GameState();

		// Get frame from FrameCapture
		// Mat temp_frame;
		// processing_buffer->get(temp_frame);
		// cv::resize(temp_frame, frame, frame.size());

		processing_buffer->get(frame);
		auto processing_start = chrono::high_resolution_clock::now();

		//Do border processing
		transform(frame, transformed_frame);

		cvtColor(transformed_frame, transformed_frame, COLOR_BGR2Lab);

		// Flip
		if (this->settings.flip)
		{
			Mat tmp;
			cv::flip(transformed_frame, tmp, -1);
			transformed_frame = tmp;
		}

		//Find objects on field
		std::vector<Circle> primary_circles, opponent_circles, secondary_circles[3], ball_circles;

		find_circles(transformed_frame, settings.colors["orange"], ball_circles);
		find_circles(transformed_frame, settings.team_color == "yellow" ? settings.colors["blue"] : settings.colors["yellow"], opponent_circles);
		find_circles(transformed_frame, settings.colors[settings.team_color], primary_circles);

		for (int i = 0; i < 3; i++)
			find_circles(transformed_frame, settings.colors[settings.secondary_colors[i]], secondary_circles[i]);

		double dWidth = transformed_frame.size().width;
		double dHeight = transformed_frame.size().height;

		Mat processed_frame = transformed_frame.clone();
		cvtColor(transformed_frame, transformed_frame, COLOR_Lab2BGR);

		for (int i = 0; i < (int)primary_circles.size(); i++) // searches all primary_color circles
		{

			int index = 0, type = 0;
			float min_dist = 1e8;
			for (int j = 0; j < 3; j++) // finds closest secondary color
				for (int k = 0; k < (int)secondary_circles[j].size(); k++)
					if (point_distance(primary_circles[i].center, secondary_circles[j][k].center) < min_dist)
					{
						min_dist = point_distance(primary_circles[i].center, secondary_circles[j][k].center);
						index = k, type = j;
					}

			if (primary_circles[i].radius * FIELD_WIDTH / dWidth < 1.5 || min_dist * FIELD_WIDTH / dWidth > 5)
				continue; // if the radius of the yellow circle is less than 8 or the secondary color is too far away (> 22), it is not a robot

			// transforms the field positions to the range (10, 160) for x and (0, 130) for y
			(this->game).robots[type].pos = {(FIELD_WIDTH * primary_circles[i].center.x / dWidth), (FIELD_HEIGHT * primary_circles[i].center.y / dHeight)};
			(this->game).robots[type].missing = false;
			// calculates direction as an unitary vector
			(this->game).robots[type].dir = {secondary_circles[type][index].center.x - primary_circles[i].center.x, secondary_circles[type][index].center.y - primary_circles[i].center.y};
			(this->game).robots[type].dir = normalise((this->game).robots[type].dir);

			circle(transformed_frame, primary_circles[i].center, 4, Scalar(255, 255, 255), -1);
			line(transformed_frame, primary_circles[i].center, primary_circles[i].center + 2 * (secondary_circles[type][index].center - primary_circles[i].center), Scalar(255, 255, 255), 2);
			printf("\nx_robo%d=%.1f, y_robo%d=%.1f\n", type, primary_circles[i].center.x, type, primary_circles[i].center.y);
			printf("robo%d_dir: %.1lf %.1lf\n", type, secondary_circles[type][index].center.x - primary_circles[i].center.x, secondary_circles[type][index].center.y - primary_circles[i].center.y);
			printf("robo%d_dir: %.1lf\n", type, (M_PI + atan2(secondary_circles[type][index].center.y - primary_circles[i].center.y, secondary_circles[type][index].center.x - primary_circles[i].center.x)) * (180 / M_PI));
		}

		sort(opponent_circles.begin(), opponent_circles.end());
		for (int i = 0; i < 3 && i < (int)opponent_circles.size(); i++)
		{
			(this->game).enemies[i].pos = {(FIELD_WIDTH * opponent_circles[i].center.x / dWidth), (FIELD_HEIGHT * opponent_circles[i].center.y / dHeight)};
			(this->game).enemies[i].dir = {0.0, 0.0};
			(this->game).enemies[i].missing = false;

			circle(transformed_frame, opponent_circles[i].center, 4, Scalar(255, 255, 255), -1);
			printf("\nx_opponent%d=%.1f, y_opponent%d=%.1f\n", i, opponent_circles[i].center.x, i, opponent_circles[i].center.y);
		}

		sort(ball_circles.begin(), ball_circles.end());
		if (ball_circles.size())
		{
			circle(transformed_frame, ball_circles[0].center, 4, Scalar(255, 255, 255), -1);
			(this->game).ball.pos = {(FIELD_WIDTH * ball_circles[0].center.x / dWidth), (FIELD_HEIGHT * ball_circles[0].center.y / dHeight)};
			(this->game).ball.missing = false;
			printf("\nx_ball=%.1f, y_ball=%.1f\n", ball_circles[0].center.x, ball_circles[0].center.y);
		}
		else
		{
			(this->game).ball.pos = {0.0, 0.0};
			(this->game).ball.missing = true;
		}

		auto processing_finish = chrono::high_resolution_clock::now();
		duration total_time = processing_finish - processing_start;
		cout << "Processing time (ms): " << total_time.count() / 1e6 << endl;

		game_buffer->update(this->game);

		vector<Mat> frames_vec;
		frames_vec.push_back(transformed_frame.clone());

		transformed_frame = processed_frame;

		Color clr;
		int temp_key = *waitkey_buf;
		if (isdigit(temp_key))
			sw_key = temp_key;

		switch (sw_key)
		{
		case '0':
			clr = settings.colors["orange"];
			break;
		case '1':
			clr = settings.colors["blue"];
			break;
		case '2':
			clr = settings.colors["yellow"];
			break;
		case '3':
			clr = settings.colors["pink"];
			break;
		case '4':
			clr = settings.colors["salmon"];
			break;
		case '5':
			clr = settings.colors["green"];
			break;
		case '6':
			clr = settings.colors["red"];
			break;
		case '7':
			clr = settings.colors["brown"];
			break;
		default:
			clr = Color(0, 255, 0, 255, 0, 255);
			break;
		}

		find_color(transformed_frame, transformed_frame, clr);
		frames_vec.push_back(transformed_frame);

		//Update buffers
		view_buffer->update(frames_vec);
	}
}

static Mat lambda(2, 4, CV_32FC1);

void set_border_manually(const Mat &input, Point2f p0, Point2f p1, Point2f p2, Point2f p3)
{
	Point2f inputQuad[4];  // array containing the four corners of the field
	Point2f outputQuad[4]; // array containing the four corners of the image

	// The 4 points that select quadilateral on the input, from top-left in clockwise order
	// These four points are the sides of the rectangular box used as input
	inputQuad[0] = p0;
	inputQuad[1] = p1;
	inputQuad[2] = p2;
	inputQuad[3] = p3;

	// The 4 points where the mapping is to be done , from top-left in clockwise order
	outputQuad[0] = Point2f(0, 0);
	outputQuad[1] = Point2f(TRANSFORMED_FRAME_WIDTH - 1, 0);
	outputQuad[2] = Point2f(TRANSFORMED_FRAME_WIDTH - 1, TRANSFORMED_FRAME_HEIGHT - 1);
	outputQuad[3] = Point2f(0, TRANSFORMED_FRAME_HEIGHT - 1);

	// Get the Perspective Transform Matrix i.e. lambda
	lambda = getPerspectiveTransform(inputQuad, outputQuad);
}

void transform(const Mat &input, Mat &output)
{
	// Apply the Perspective Transform just found to the src image
	warpPerspective(input, output, lambda, Size(TRANSFORMED_FRAME_WIDTH, TRANSFORMED_FRAME_HEIGHT));
}

void find_color(const Mat &input, Mat &binary_image, Color color)
{
	double b_avg = (color.bmin + color.bmax) / 2;
	double b_diff = (color.bmax - color.bmin) / 2;

	inRange(input, Scalar(int(b_avg - 1.5 * b_diff), color.gmin, color.rmin), Scalar(int(b_avg + 1.5 * b_diff), color.gmax, color.rmax), binary_image);

	// Apply the erosion operation and then dilation. In theory, it removes small interferences from the image.
	// Increase erosion size to remove bigger "spots"
	int erosion_size = 1;
	erode(binary_image, binary_image, getStructuringElement(MORPH_ELLIPSE, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size)));
	int dilation_size = 2;
	dilate(binary_image, binary_image, getStructuringElement(MORPH_RECT, Size(2 * dilation_size + 1, 2 * dilation_size + 1), Point(dilation_size, dilation_size)));
}

void find_circles(const Mat &image, Color color_sought, vector<Circle> &res)
{
	// Creates binary image white the selected color
	Mat binary_image;
	find_color(image, binary_image, color_sought);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	// Finds countours and stores them in the "contours" vector
	findContours(binary_image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	for (int i = 0; i < (int)contours.size(); i++)
	{
		Point2f center(0, 0);
		float radius;
		// finds the smallest possible circle that contains the contour
		minEnclosingCircle(contours[i], center, radius);

		// stores it in the output vector
		res.push_back(Circle(center, radius));
	}
}

float point_distance(Point2f a, Point2f b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}
