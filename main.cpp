#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono> // for timing
/*
Average preprocessing time: 0.000819749 seconds / frame
Average contour detection time: 0.000259412 seconds / frame
Correct contour detection rate: 95.6757%
*/
using namespace cv;
using namespace std;

// Function to perform preprocessing steps
// This function takes a frame of the video as input, performs preprocessing steps, and returns a binary mask of the region of interest (black -> white, other colors -> black)
void preprocessFrame(const Mat &inputFrame, Mat &outputFrame)
{
    // convert input frame to HSV color space
    Mat frame_hsv;
    cvtColor(inputFrame, frame_hsv, COLOR_BGR2HSV);

    // Smooth the image to reduce noise
    int blurSize = 7; // Adjust the size of the blur kernel
    GaussianBlur(frame_hsv, frame_hsv, Size(blurSize, blurSize), 0);

    // Filter out the colors that are not in the range of HSV values of interest
    Mat rangeFilteredMask;

    // method 1: HSV inrange filtering
    inRange(frame_hsv, Scalar(0, 0, 0), Scalar(180, 132, 107), rangeFilteredMask);

    // method 2: split HSV channels and filter value channel
    // vector<Mat> hsv_channels;
    // split(frame_hsv, hsv_channels);
    // threshold(hsv_channels[2], rangeFilteredMask, 100, 255, THRESH_BINARY_INV);

    // method 3: directly convert input frame to grayscale and threshold
    // Mat grayFrame;
    // cvtColor(inputFrame, grayFrame, COLOR_BGR2GRAY);
    // threshold(grayFrame, rangeFilteredMask, 100, 255, THRESH_BINARY);

    // Apply morphological operations to remove small noise and fill in gaps in the mask
    int dilationSize = 12; // Adjust the size of the dilation kernel
    Mat kernel = getStructuringElement(MORPH_RECT, Size(dilationSize, dilationSize));
    dilate(rangeFilteredMask, rangeFilteredMask, kernel, Point(-1, -1), 1);
    erode(rangeFilteredMask, outputFrame, kernel, Point(-1, -1), 1);
}

// Function to detect contours
// returns true if contours are found, false otherwise
// stores the largest contour in largestContour
bool detectContours(const Mat &inputFrame, vector<Point> &largestContour)
{
    vector<vector<Point>> contours;
    findContours(inputFrame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (contours.empty())
    {
        // No contours found, return false
        return false;
    }

    int largest_contour_index = 0;
    double max_area = 0.0;
    for (size_t i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > max_area)
        {
            max_area = area;
            largest_contour_index = i;
        }
    }

    largestContour = contours[largest_contour_index];
    return true; // Contours found
}

int main()
{
    VideoCapture cap("/home/hat082/coursework/video.mp4");

    if (!cap.isOpened())
    {
        cout << "Error opening video file" << endl;
        return -1;
    }

    VideoWriter out;
    int frame_width = cap.get(CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    out.open("/home/hat082/coursework/output_video.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, Size(frame_width, frame_height), true);

    // Declaration for timing
    chrono::duration<double> total_preprocessing_time = chrono::duration<double>::zero();
    chrono::duration<double> total_contour_detection_time = chrono::duration<double>::zero();

    int frame_count = 0;   // to count the number of frames processed
    int correct_count = 0; // to count the number of frames with correct contour detection

    int split = 1;

    while (true)
    {
        Mat frame;
        cap >> frame;
        if (frame.empty())
        {
            cout << "End of video" << endl;
            break;
        }

        // define the region of interest
        // draw a rectangle on the frame to show the region of interest

        int x = frame.cols * 0.02;
        int y = frame.rows * 0.5;
        int width = frame.cols * 0.96;
        int height = frame.rows * 0.3 / split;
        Mat resultFrame;

        resultFrame = frame.clone(); // Copy the original frame to the output
                                     // #pragma omp parallel for
        for (int i = 0; i < split; i++)
        {
            Rect roi(x, (int)y + i * height, width, height);

            // Call the functions to perform operations
            Mat preprocessedFrame;
            vector<Point> largestContour;

            // Start timing for preprocessing
            auto start_preprocessing = chrono::high_resolution_clock::now();
            preprocessFrame(frame(roi), preprocessedFrame);
            total_preprocessing_time += chrono::high_resolution_clock::now() - start_preprocessing;

            // start timing for contour detection
            auto start_contour_detection = chrono::high_resolution_clock::now();

            rectangle(resultFrame, roi, Scalar(0, 255, 0), 2);
            // If a contour is detected, perform further operations on it
            if (detectContours(preprocessedFrame, largestContour))
            {
                // find the center of the largest contour and puttext on the frame
                Moments m = moments(largestContour, false);
                Point2f center(m.m10 / m.m00, m.m01 / m.m00);

                // end time for contour detection
                total_contour_detection_time += chrono::high_resolution_clock::now() - start_contour_detection;

                // Draw the largest contour and center of the largest contour on the frame
                drawContours(resultFrame(roi), vector<vector<Point>>{largestContour}, -1, Scalar(255, 255, 255), 2);
                circle(resultFrame(roi), center, 5, Scalar(0, 255, 255), -1);

                // draw the distance from the center of the frame to the center of the largest contour
                double distance = center.x - roi.width / 2;

                line(resultFrame(roi), center, Point(roi.width / 2, center.y), Scalar(0, 255, 255), 1);

                // puttext on the center of the line
                putText(resultFrame(roi), to_string(distance), Point(roi.width / 2 - 5, center.y - 5), FONT_HERSHEY_SIMPLEX, 0.2, Scalar(255, 255, 255), 1, LINE_AA);

                ostringstream text;
                text << "(" << center.x << "," << center.y << ")";

                // Check if the center of the largest contour is within the black line
                // cout << "Value at center: " << (int)preprocessedFrame.at<uchar>(center.y, center.x) << endl;
                if (preprocessedFrame.at<uchar>(center.y, center.x) == 255)
                {
                    correct_count++;
                    putText(resultFrame(roi), text.str(), Point(20, roi.height / 2), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1, LINE_AA);
                }
                else
                {
                    putText(resultFrame(roi), text.str(), Point(20, roi.height / 2), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1, LINE_AA);
                }
            }

            else
            {
                correct_count++;
                // end time for contour detection
                total_contour_detection_time += chrono::high_resolution_clock::now() - start_contour_detection;

                // puttext on the center of the frame
                ostringstream text;
                text << "Out of View";
                putText(resultFrame(roi), text.str(), Point(resultFrame(roi).cols / 2 - 5, resultFrame(roi).rows / 2), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1, LINE_AA);
            }
        }

        // Write the processed frame into the output video
        out.write(resultFrame);

        // imshow("resultFrame", resultFrame);
        // if (waitKey(1) == 27)
        // {
        //     break;
        // }

        frame_count++;
    }

    cap.release();
    out.release();

    // Calculate average time for each section
    double avg_preprocessing_time = total_preprocessing_time.count() / frame_count;
    double avg_contour_detection_time = total_contour_detection_time.count() / frame_count;

    cout << "Total frames processed: " << frame_count << endl;
    cout << "Total preprocessing time: " << total_preprocessing_time.count() << " seconds" << endl;
    cout << "Total contour detection time: " << total_contour_detection_time.count() << " seconds" << endl;
    cout << "Average preprocessing time: " << avg_preprocessing_time << " seconds / frame" << endl;
    cout << "Average contour detection time: " << avg_contour_detection_time << " seconds / frame" << endl;
    cout << "Correct contour detection rate: " << (double)correct_count / (frame_count * split) * 100 << "%" << endl;

    return 0;
}
