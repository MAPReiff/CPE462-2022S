// I plege my honor that I have abided by the Stevens Honor System.
// Based on https://docs.opencv.org/4.x/db/deb/tutorial_display_image.html
// and https://www.opencv-srf.com/2017/11/load-and-display-image.html

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char **argv) {

   Mat image = imread("./assets/vivy10.png");

   if (image.empty()) {
	  cout << "Could not open or find the image" << endl;
	  return -1;
   }

   String windowName = "Image Demo";

   namedWindow(windowName);

   imshow(windowName, image);

   waitKey(0);
   destroyWindow(windowName);

   return 0;
}
