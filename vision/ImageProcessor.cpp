#include "ImageProcessor.hpp"

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

void ImageProcessor::start(VSSSBuffer<Mat> &view_buffer, VSSSBuffer<Mat> &processing_buffer,
                           VSSSBuffer<GameState> &game_buffer, int *waitkey_buf)
{
    this->processor_th = std::thread(&ImageProcessor::processor, this, &view_buffer, &processing_buffer,
                                     &game_buffer, waitkey_buf);
}

void ImageProcessor::processor(VSSSBuffer<Mat> *view_buffer, VSSSBuffer<Mat> *processing_buffer,
                               VSSSBuffer<GameState> *game_buffer, int *waitkey_buf)
{

    Mat frame(480, 854, CV_8UC3);

    while (*waitkey_buf != 27 /* ESC */)
    {
        Mat temp_frame;
        processing_buffer->get(temp_frame);
        cv::resize(temp_frame, frame, frame.size());
        view_buffer->update(frame);
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
    outputQuad[1] = Point2f(input.cols - 1, 0);
    outputQuad[2] = Point2f(input.cols - 1, input.rows - 1);
    outputQuad[3] = Point2f(0, input.rows - 1);

    // Get the Perspective Transform Matrix i.e. lambda
    lambda = getPerspectiveTransform(inputQuad, outputQuad);
}

void transform(const Mat &input, Mat &output)
{
    // Apply the Perspective Transform just found to the src image
    warpPerspective(input, output, lambda, output.size());
}

void find_color(const Mat &input, Mat &binary_image, Color color)
{

    inRange(input, Scalar(color.bmin, color.gmin, color.rmin), Scalar(color.bmax, color.gmax, color.rmax), binary_image);

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

    for (int i = 0; i < contours.size(); i++)
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
