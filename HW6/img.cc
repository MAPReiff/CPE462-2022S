#include "CImg.h"

#include <iostream>
#include <math.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace cimg_library;
using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {
	FILE *in, *out;
	int width, height, **image_in, **image_out, size;
	float sum1, sum2, new_T, old_T, delta_T;
	long count1, count2;

	size = 3;

	int mask[3][3] = {{-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1}};

	Mat M_in = imread(argv[1]);
	Mat_<uchar> M_in_g(M_in.rows, M_in.cols);


	cvtColor(M_in, M_in_g, COLOR_BGR2GRAY);

	String windowIn = "Input";
	namedWindow(windowIn);
	imshow(windowIn, M_in_g);
	waitKey(0);
	destroyWindow(windowIn);

	height = M_in_g.rows;
	width = M_in_g.cols;

	image_in = (int **)calloc(height, sizeof(int *));
	if (!image_in) {
		cout << "Error allocating memory" << endl;
		return (1);
	}

	image_out = (int **)calloc(height, sizeof(int *));
	if (!image_out) {
		cout << "Error allocating memory" << endl;
		return (1);
	}

	for (int j = 0; j < height; j++) {
		image_in[j] = (int *)calloc(width, sizeof(int));
		if (!image_in[j]) {
			cout << "Error allocating memory" << endl;
			return (1);
		}

		image_out[j] = (int *)calloc(width, sizeof(int));
		if (!image_out[j]) {
			cout << "Error allocating memory" << endl;
			return (1);
		}
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			image_in[i][j] = M_in_g(i, j);
		}
	}

	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			for (int k = -1; k <= 1; k++) {
				for (int l = -1; l <= 1; l++) {
					image_out[i][j] +=
						image_in[i + k][j + l] * mask[k + 1][l + 1];
				}
			}
			int value = image_out[i][j];
			if (value < 0) {
				image_out[i][j] = 0;
			} else if (value > 255) {
				image_out[i][j] = 255;
			}
		}
	}

	Mat_<uchar> M_out(height, width);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			M_out[i][j] = image_out[i][j];
		}
	}

	String windowOut = "Output";
	namedWindow(windowOut);
	imshow(windowOut, M_out);
	waitKey(0);
	destroyWindow(windowOut);

	bool isSuccess = imwrite(argv[2], M_out);
	if (!isSuccess) {
		cout << "Failed to write out file";
		return -1;
	}

	for (int j = 0; j < height; j++) {
		free(image_in[j]);
		free(image_out[j]);
	}
  
	free(image_in);
	free(image_out);

	return 0;
}