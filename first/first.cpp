// first.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/videoio.hpp>  // Video write
#include <map>

using namespace std;
using namespace cv;
using namespace cv::aruco;

int main()
{
    /*
    Mat markerImage;

    // Load the predefined dictionary
    Ptr<cv::aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

    // Generate the marker
    aruco::drawMarker(dictionary, 34, 200, markerImage, 1);
    Mat in_bmp = imread("1234.bmp");
    imwrite("marker34.png", markerImage);
    */
    //std::cout << "Hello World!\n";
    int pre = getTickCount();
    VideoCapture inputVideo("capture.avi");              // Open input
    if (!inputVideo.isOpened())
    {
        cout << "Could not open the input video: " << endl;
        return -1;
    }
    Size S = Size((int)inputVideo.get(CAP_PROP_FRAME_WIDTH),    // Acquire input size
        (int)inputVideo.get(CAP_PROP_FRAME_HEIGHT));
    int ex = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));     // Get Codec Type- Int form

    VideoWriter outputVideo;
    outputVideo.open("result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), inputVideo.get(CAP_PROP_FPS), S, true);
    if (!outputVideo.isOpened())
    {
        cout << "Could not open the output video for write: " << endl;
        return -1;
    }
    // Load the dictionary that was used to generate the markers.
    Ptr<Dictionary> dictionary = getPredefinedDictionary(DICT_6X6_250);

    // Initialize the detector parameters using default values.
    Ptr<DetectorParameters> parameters = DetectorParameters::create();

    // Declare the vectors that would contain the detected marker corners and the rejected marker candidates.
    vector<vector<Point2f> > markerCorners, rejectedCandidates;

    // The ids of the detected markers are stored in a vector
    vector<int> markerIds;

    // Warped image;
    Mat warpedImage, imageCopy;;

    // Load identify card image
    Mat im_src = imread("id_card.png");

    vector<Point> pts_src;
    pts_src.push_back(Point(0, 0));
    pts_src.push_back(Point(2273, 0));
    pts_src.push_back(Point(2273, 1441));
    pts_src.push_back(Point(0, 1441));

    Mat frame, h, mask, imOut; // = imread("frame.jpg");
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3)); // For eroding
    for (int t = 0 ; ; t++) //Show the image captured in the window and repeat
    {
        inputVideo >> frame;              // read
        if (frame.empty()) break;         // check if at end
        // Detect the markers in the image
        detectMarkers(frame, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
        
        vector<Point> pts_dst;
        for (int m = 0; m < 4; m++) 
            for (int i = 0; i < markerCorners.size(); i++)
                if (markerIds[i] == (31 + m)) pts_dst.push_back(markerCorners[i][0]);

        // Compute homography from source and destination points
        h = cv::findHomography(pts_src, pts_dst);

        // Wrap source image to destination based on homography
        warpPerspective(im_src, warpedImage, h, frame.size(), INTER_CUBIC);

        // Prepare a mask representing region to copy from the warped image into the original frame.
        mask = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
        fillConvexPoly(mask, pts_dst, Scalar(255, 255, 255));

        // Erode the mask to not copy the boundary effects from the warping
        erode(mask, mask, element);

        // Copy the masked warped image into the original frame in the mask region.
        warpedImage.copyTo(frame, mask);
        outputVideo << frame;
        if (t == 51) {
            cout << "Finished writing in " << getTickCount() - pre << endl;
        }
    }
    
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
