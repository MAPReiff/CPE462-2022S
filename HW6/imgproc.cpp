// imgproc.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Usage: imaproc in_file_name.jpg out_file_name.jpg
//

#include "CImg.h"

#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace cimg_library;

using namespace cv;
using namespace std;


int main(int argc, char *argv[]) {
	FILE *in, *out;
	int j, k, width, height;
	int **image_in, **image_out;
	float sum1, sum2;
	float new_T, old_T, delta_T;
	long count1, count2;
	// errno_t err;   // had to comment this out to compile on Linux

	float valueIn = 0.0;
	float mask[3][3] = {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}};
	float valueOut = 0.0;
	int m;
	int n;
	float sum;

	if (argc < 5) {
		cout << "FAILED" << endl;
		return 0;
	}

	width = atoi(argv[3]);
	height = atoi(argv[4]);

	image_in = (int **)calloc(height, sizeof(int *));
	if (!image_in) {
		cout << "FAILED memmory allocation" << endl;
		return 1;
	}

	image_out = (int **)calloc(height, sizeof(int *));
	if (!image_out) {
		cout << "FAILED memmory allocation" << endl;
		return 1;
	}

	for (int j = 0; j < height; j++) {
		image_in[j] = (int *)calloc(width, sizeof(int));
		if (!image_in[j]) {
			cout << "FAILED memmory allocation" << endl;
			return 1;
		}

		image_out[j] = (int *)calloc(width, sizeof(int));
		if (!image_out[j]) {
			cout << "FAILED memmory allocation" << endl;
			return 1;
		}
	}

	if ((in = fopen(argv[1], "rb")) == NULL) {
		cout << "FAILED to open file" << endl;
		return 1;
	}

	if ((out = fopen(argv[2], "wb")) == NULL) {
		cout << "FAILED to open file" << endl;
		return 1;
	}

	for (int j = 0; j < height; j++) {
		for (k = 0; k < width; k++) {
			if ((image_in[j][k] = fgetc(in)) == EOF) {
				cout << "FAILED to read file" << endl;
				return 1;
			}
		}
	}

	if (fclose(in) == EOF) {
		cout << "FAILED to close file" << endl;
		return 1;
	}

	CImg<int> image_disp(width, height, 1, 1, 0);

	for (int j = 0; j < height; j++) {
		for (k = 0; k < width; k++) {
			image_disp(k, j, 0, 0) = image_in[j][k];
		}
	}

	CImgDisplay disp_in(image_disp, "Input Image", 0);

	/****************************/
	/* Some OpenCV here */
	/********************/

	Mat M_in = imread(argv[1]);
	Mat_<uchar> M_in_g(M_in.rows, M_in.cols);
	cvtColor(M_in, M_in_g, COLOR_BGR2GRAY);
	//cout << "height" << M_in_g.rows << endl;

	String windowName1 = "Input Image"; //Name of the window

	namedWindow(windowName1); // Create a window

	imshow(windowName1, M_in_g); // Show our image inside the created window.

	waitKey(0); // Wait for any keystroke in the window

	destroyWindow(windowName1); //destroy the created window

	height = M_in_g.rows;
	width = M_in_g.cols;

	/********************************************************************/

	image_in = (int **)calloc(height, sizeof(int *));
	if (!image_in) {
		printf("Error: Can't allocate memmory!\n");
		return (1);
	}

	image_out = (int **)calloc(height, sizeof(int *));
	if (!image_out) {
		printf("Error: Can't allocate memmory!\n");
		return (1);
	}

	for (j = 0; j < height; j++) {
		image_in[j] = (int *)calloc(width, sizeof(int));
		if (!image_in[j]) {
			printf("Error: Can't allocate memmory!\n");
			return (1);
		}

		image_out[j] = (int *)calloc(width, sizeof(int));
		if (!image_out[j]) {
			printf("Error: Can't allocate memmory!\n");
			return (1);
		}
	}

	for (j = 0; j < height; j++)
		for (k = 0; k < width; k++)
			image_in[j][k] = M_in_g(j, k);


	/********************************************************************/
	/* Image Processing begins                                          */
	/********************************************************************/
	float histogram[256];
	float eqMap[256];
	float onePixel = 1.0 / (float(width) * height);

	for (int j = 0; j < 256; j++) {
		histogram[j] = 0.0;
		eqMap[j] = 0.0;
	}

	for (int j = 0; j < height; j++) {
		for (int k = 0; k < width; k++) {
			histogram[image_in[j][k]] += onePixel;
		}
	}

	for (int j = 0; j < 256; j++) {
		for (int k = 0; k < j + 1; k++) {
			eqMap[j] += histogram[k];
		}
		eqMap[j] = floor(eqMap[j] * 255);
	}

	for (int j = 0; j < height; j++) {
		for (int k = 0; k < width; k++) {
			image_out[j][k] = eqMap[image_in[j][k]];
			eqMap[image_in[j][k]];
		}
	}

	CImg<float> plot(256, 256, 1, 1, 255), vec_data(256, 1, 1, 1, 0);
	const unsigned char black[] = {0};
	for (int j = 0; j < 256; j++) {
		vec_data(j, 0, 0, 0) = histogram[j];
	}

	plot.draw_graph(vec_data, black, 1, 1, 0, 0, 0);

	CImgDisplay disp_histogram(plot, "Histogram", 0);

	for (int j = 0; j < 256; j++) {
		vec_data(j, 0, 0, 0) = eqMap[j];
	}

	plot.fill(255).draw_graph(vec_data, black, 1, 1, 0, 0, 0);
	CImgDisplay disp_eqMap(plot, "Equalization Map", 0);

	for (j = 0; j < height; j++) {
		for (k = 0; k < width; k++) {
			if (j == 0 || k == 0 || j == height - 1 || k == width - 1) {
				image_out[j][k] = image_in[j][k];
			} else {
				sum = 0.0;
				for (int m = 0; m < 3; m++) {
					for (int n = 0; n < 3; n++) {
						sum +=
							((float)image_in[j + m - 1][k + n - 1] *
							 mask[m][n] / 9.0);
					}
					valueOut = sum;
					valueOut = (valueOut > 255 ? 255 : valueOut);
					valueOut = (valueOut < 0 ? 0 : valueOut);
					image_out[j][k] = (int)valueOut;
				}
			}
		}
	}

	/********************************************************************/
	/* Image Processing ends                                          */
	/********************************************************************/


	for (j = 0; j < height; j++) {
		for (k = 0; k < width; k++) {
			image_disp(k, j, 0, 0) = image_out[j][k];
		}
	}
	CImgDisplay disp_out(image_disp, "Output Image", 0);

	for (int j = 0; j < height; j++) {
		for (int k = 0; k < width; k++) {
      if ((putc(image_out[j][k],out)) == EOF) {
        printf("Error: Can't write to file!\n");
        return (1);
      }
		}
	}

  if(fclose(out)==EOF) {
    cout << "Error: Can't close file!\n";
    return (1);
  }

  while (!disp_in.is_closed()) {
    disp_in.wait();
  }
  while (!disp_out.is_closed()) {
    disp_out.wait();
  }

  for (int j=0; j<height; j++) {
    free(image_in[j]);
    free(image_out[j]);
  }

  free(image_in);
  free(image_out);

  return 0;

	/****************************/
	/* Some OpenCV here */
	/********************/

	Mat_<uchar> M_out(height, width);
	for (int ii = 0; ii < height; ii++)
		for (int jj = 0; jj < width; jj++)
			M_out(ii, jj) = image_out[ii][jj];

	String windowName2 = "Output Image"; //Name of the window

	namedWindow(windowName2); // Create a window

	imshow(windowName2, M_out); // Show our image inside the created window.

	waitKey(0); // Wait for any keystroke in the window

	destroyWindow(windowName2); //destroy the created window


	bool isSuccess =
		imwrite(argv[2], M_out); //write the image to a file as JPEG
	//bool isSuccess = imwrite("MyOutputImage.png", M_out); //write the image to a file as PNG
	if (isSuccess == false) {
		cout << "Failed to save the image" << endl;
		//cin.get(); //wait for a key press
		return -1;
	}


	/***********************/

	for (j = 0; j < height; j++) {
		free(image_in[j]);
		free(image_out[j]);
	}
	free(image_in);
	free(image_out);

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
