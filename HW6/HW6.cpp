// imgproc.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Usage: imaproc in_file_name.jpg out_file_name.jpg
//

#include "CImg.h"

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
	//errno_t err;

	float value_in = 0.0;
	float mask[3][3] = {
		{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}}; //any mask (filter)
	float value_out = 0.0;
	int m, n;
	float sum;


	/********************/
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


	if (argc < 5) {
		printf("Error: Insufficient parameters \n");
		return (1);
	}

	width = atoi(argv[3]);
	height = atoi(argv[4]);

	image_in = (int **)calloc(height, sizeof(int *));
	if (!image_in) {
		printf("Error: Can't allocate memmory \n");
		return (1);
	}

	image_out = (int **)calloc(height, sizeof(int *));
	if (!image_out) {
		printf("Error: Can't allocate memmory \n");
		return (1);
	}

	for (j = 0; j < height; j++) {
		image_in[j] = (int *)calloc(width, sizeof(int));
		if (!image_in[j]) {
			printf("Error: Can't allocate memmory \n");
			return (1);
		}

		image_out[j] = (int *)calloc(width, sizeof(int));
		if (!image_out[j]) {
			printf("Error: Can't allocate memmory \n");
			return (1);
		}
	}

	if ((in = fopen(argv[1], "rb")) == NULL) {
		printf("Error: Can't open in_file \n");
		return (1);
	}

	if ((out = fopen(argv[2], "wb")) == NULL) {
		printf("Error: Can't open out_file \n");
		return (1);
	}

	for (j = 0; j < height; j++)
		for (k = 0; k < width; k++) {
			if ((image_in[j][k] = getc(in)) == EOF) {
				printf("Error: Can't read from in_file \n");
				return (1);
			}
		}

	if (fclose(in) == EOF) {
		printf("Error: Can't close in_file \n");
		return (1);
	}

	//diplay image_in
	CImg<int> image_disp(
		width, height, 1, 1,
		0); //CImg<type> image_name(width, height, temporal_frame_number, color_plane_number, initial_value)

	for (j = 0; j < height; j++)
		for (k = 0; k < width; k++) {
			image_disp(k, j, 0, 0) = image_in[j][k];
		}
	CImgDisplay disp_in(
		image_disp, "Image_In",
		0); //CImgDisplay display_name(image_displayed, "window title", normalization_factor)


	/********************************************************************/
	/* Image Processing begins                                          */
	/********************************************************************/

	float histogram[256];
	float eq_map[256];
	float one_pixel = 1.0 / ((float)width * height);
	for (j = 0; j < 256; j++)
		histogram[j] = 0.0;
	for (j = 0; j < 256; j++)
		eq_map[j] = 0.0;
	for (j = 0; j < height; j++)
		for (k = 0; k < width; k++) {
			histogram[image_in[j][k]] += one_pixel;
		}
	for (j = 0; j < 256; j++) {
		for (k = 0; k < width; k++)
			eq_map[j] += histogram[k];
		eq_map[j] = floor(eq_map[j] * 255.0);
	}
	for (j = 0; j < height; j++)
		for (k = 0; k < width; k++) {
			image_out[j][k] = (int)eq_map[image_in[j][k]];
		}
	CImg<float> plot(256, 256, 1, 1, 255), vec_data(256, 1, 1, 1, 0);
	const unsigned char black[] = {0};
	for (j = 0; j < 256; j++)
		vec_data(j, 0, 0, 0) = histogram[j];
	plot.draw_graph(vec_data, black, 1, 1, 0, 0, 0);
	CImgDisplay disp_histogram(plot, "Histogram", 0);
	for (j = 0; j < 256; j++)
		vec_data(j, 0, 0, 0) = eq_map[j];
	plot.fill(255).draw_graph(vec_data, black, 1, 1, 0, 0, 0);
	CImgDisplay disp_eqmap(plot, "Equalization Mapping Function", 0);
	for (j = 0; j < height; j++)
		for (k = 0; k < width; k++) {
			if (j == 0 || k == 0 || j == height - 1 || k == width - 1)
				image_out[j][k] = image_in[j][k];

			else {
				sum = 0.0;
				for (m = 0; m < 3; m++)
					for (n = 0; n < 3; n++) {
						sum += ((float)image_in[j + m - 1][k + n - 1]) *
							   mask[m][n] /
							   9.0; //the 9.0 is part of this filter
					}
				value_out = sum;
				value_out = (value_out > 255 ? 255 : value_out);
				value_out = (value_out < 0 ? 0 : value_out);
				image_out[j][k] = (int)value_out;
			}
		}


	/********************************************************************/
	/* Image Processing ends                                            */
	/********************************************************************/

	//display image_out
	for (j = 0; j < height; j++)
		for (k = 0; k < width; k++) {
			image_disp(k, j, 0, 0) = image_out[j][k];
		}
	CImgDisplay disp_out(image_disp, "Image_Out", 0);

	//save image_out into out_file in RAW format
	for (j = 0; j < height; j++)
		for (k = 0; k < width; k++) {
			if ((putc(image_out[j][k], out)) == EOF) {
				printf("Error: Can't write to out_file\n");
				return (1);
			}
		}

	if (fclose(out) == EOF) {
		printf("Error: Can't close out_file\n");
		return (1);
	}

	//closing
	while (!disp_in.is_closed())
		disp_in.wait();
	while (!disp_out.is_closed())
		disp_out.wait();

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
