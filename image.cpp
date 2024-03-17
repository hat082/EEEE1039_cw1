#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
  // 读取图像
  Mat image = imread("/home/hat082/coursework/sample_line.png");
  if (image.empty())
  {
    cout << "无法读取图像" << endl;
    return -1;
  }
  imshow("original image", image);

  // Convert the image to HSV
  Mat hsvImage;
  cvtColor(image, hsvImage, COLOR_BGR2HSV);

  // extract the value channel

  vector<Mat> hsvChannels;
  split(hsvImage, hsvChannels);

  // The value channel is at index 2
  cv::Mat valChannel = hsvChannels[2]; //

  Mat lowValMask;
  inRange(valChannel, 0, 50, lowValMask);

  // imshow("Low Value Binary Image", lowValMask);

  // Create a bitmask with the same size as the valChannel
  Mat bitmask = Mat::ones(valChannel.size(), CV_8U);

  // Specify the thickness of the lines
  int lineThickness = 3;

  // Draw black lines at every valChannel.rows / 5 interval with specified thickness
  for (int i = 0; i < bitmask.rows; i += (int)bitmask.rows / 8)
  {
    line(bitmask, Point(0, i), Point(bitmask.cols, i), Scalar(0), lineThickness);
  }
  // imshow("Bitmask", bitmask);

  // Apply the bitmask to the valChannel
  bitwise_and(lowValMask, bitmask, lowValMask);
  // imshow("Low Value Binary Image with Lines", lowValMask);

  // find the contours of the image
  vector<vector<Point>> contours;
  findContours(lowValMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

  // split the contours into separate blocks
  // draw the contours
  Mat contour_image = image.clone();

  // use approxPolyDP to approximate the contours
  vector<vector<Point>> approxContours;
  for (int i = 0; i < contours.size(); i++)
  {
    vector<Point> approx;
    approxPolyDP(contours[i], approx, 0.01 * arcLength(contours[i], true), true);
    approxContours.push_back(approx);
  }

  // Draw the approximated parallelogram contours and display the moments of the contours
  for (size_t i = 0; i < approxContours.size(); i++)
  {
    drawContours(contour_image, approxContours, i, Scalar(0, 255, 0), 2);

    Moments m = moments(approxContours[i], false);
    Point2f center(m.m10 / m.m00, m.m01 / m.m00);
    circle(contour_image, center, 5, Scalar(255, 0, 0), -1);
  }
  imshow("contour image with center of mass", contour_image);

  waitKey(0);

  return 0;
}
