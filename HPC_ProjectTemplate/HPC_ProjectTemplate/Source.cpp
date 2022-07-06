#include <iostream>
#include <math.h>
#include <stdlib.h>
#include<string.h>
#include<msclr\marshal_cppstd.h>
#include <ctime>// include this header 
#include <mpi.h>
#include <algorithm>
#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
using namespace std;
using namespace msclr::interop;

int* inputImage(int* w, int* h, System::String^ imagePath) //put the size of image in w & h
{
	int* input;


	int Originalcolumns, Originalrows;

	//*********************************************************Read Image and save it to local arrayss*************************	
	//Read Image and save it to local arrayss

	System::Drawing::Bitmap BM(imagePath);

	Originalcolumns = BM.Width;
	Originalrows = BM.Height;
	*w = BM.Width;
	*h = BM.Height;
	int* Red = new int[BM.Height * BM.Width];
	int* Green = new int[BM.Height * BM.Width];
	int* Blue = new int[BM.Height * BM.Width];
	input = new int[BM.Height * BM.Width];
	for (int i = 0; i < BM.Height; i++)
	{
		for (int j = 0; j < BM.Width; j++)
		{
			System::Drawing::Color c = BM.GetPixel(j, i);

			Red[i * BM.Width + j] = c.R;
			Blue[i * BM.Width + j] = c.B;
			Green[i * BM.Width + j] = c.G;

			input[i * BM.Width + j] = ((c.R + c.B + c.G) / 3); //gray scale value equals the average of RGB values

		}

	}
	return input;
}


void createImage(int* image, int width, int height, int index)
{
	System::Drawing::Bitmap MyNewImage(width, height);


	for (int i = 0; i < MyNewImage.Height; i++)
	{
		for (int j = 0; j < MyNewImage.Width; j++)
		{
			//i * Originalcolumns + j
			if (image[i * width + j] < 0)
			{
				image[i * width + j] = 0;
			}
			if (image[i * width + j] > 255)
			{
				image[i * width + j] = 255;
			}
			System::Drawing::Color c = System::Drawing::Color::FromArgb(image[i * MyNewImage.Width + j], image[i * MyNewImage.Width + j], image[i * MyNewImage.Width + j]);
			MyNewImage.SetPixel(j, i, c);
		}
	}
	MyNewImage.Save("..//Data//Output//outputRes" + index + ".png");
	cout << "result Image Saved " << index << endl;
}

void Midean_Filter(int height, int width, int filter_size, int ImageData[], int ImageOutput[]) {

	cout << "Height: " << height << "  Width: " << width << endl;

	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{

			int* window = new int[filter_size * filter_size];

			for (int r = 0, i = row - floor(filter_size / 2); r < filter_size; i++, r++)
			{
				for (int c = 0, j = col - floor(filter_size / 2); c < filter_size; j++, c++)
				{
					if ((i >= 0 && i < height) && (j >= 0 && j < width))
					{
						window[r * filter_size + c] = ImageData[i * width + j];
					}
					else {
						window[r * filter_size + c] = 0;
					}
				}
			}

			sort(window, window + (filter_size * filter_size));

			ImageOutput[row * width + col] = window[(filter_size * filter_size) / 2];
		}
	}
}

int main()
{
	MPI_Init(NULL, NULL);
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int columns = 4, rows = 4;
	int* imageData = 0;
	int filterSize = 5;

	int start_s = 0, stop_s = 0, TotalTime = 0;
	if (rank == 0) {
		cout << "Enter the filter size: ";
		cin >> filterSize;
		System::String^ imagePath;
		std::string img;
		img = "..//Data//Input//girl_in_hat.png";
		imagePath = marshal_as<System::String^>(img);
		imageData = inputImage(&columns, &rows, imagePath);
		//write code here
		cout << endl << "Picture is stored in memory at location : " << imageData << endl;
		int* ImageOutput = new int[rows * columns];
		start_s = clock();
		Midean_Filter(rows, columns, filterSize, imageData, ImageOutput);
		stop_s = clock();
		createImage(ImageOutput, columns, rows, 0);
		TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
		cout << "Time of sequential code: " << TotalTime << endl;
		free(ImageOutput);
		start_s = clock();
	}
	MPI_Bcast(&filterSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&columns, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&start_s, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int elementsPerProcessor = (rows * columns) / size;
	int rowsPerProcessor = rows / size;
	int* filteredImagePerProcessor = new int[elementsPerProcessor] {};
	int* unfilteredImagePerProcessor = new int[elementsPerProcessor] {};

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Scatter(imageData, elementsPerProcessor, MPI_INT, unfilteredImagePerProcessor, elementsPerProcessor, MPI_INT, 0, MPI_COMM_WORLD);

	int* filteredImage = new int[rows * columns];
	int* medianFilter = new int[filterSize * filterSize];

	int ctr = 0;
	for (int row = 0; row < rowsPerProcessor; row++)
	{
		for (int col = 0; col < columns; col++)
		{
			for (int r = 0, i = row - floor(filterSize / 2); r < filterSize; i++, r++)
			{
				for (int c = 0, j = col - floor(filterSize / 2); c < filterSize; j++, c++)
				{
					if ((i >= 0 && i < rowsPerProcessor) && (j >= 0 && j < columns))
					{
						medianFilter[r * filterSize + c] = unfilteredImagePerProcessor[i * columns + j];
					}
					else {
						medianFilter[r * filterSize + c] = 127;
					}
				}
			}

			sort(medianFilter, medianFilter + (filterSize * filterSize));

			filteredImagePerProcessor[ctr] = medianFilter[(filterSize * filterSize) / 2];

			ctr++;
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gather(filteredImagePerProcessor, elementsPerProcessor, MPI_INT, filteredImage, elementsPerProcessor, MPI_INT, 2, MPI_COMM_WORLD);
	if (rank == 2) {
		stop_s = clock();
		createImage(filteredImage, columns, rows, 1);
		TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
		cout << "Time of parallel programming: " << TotalTime << endl;
	}
	if (rank == 0) {
		free(imageData);
	}
	if (rank == 2) {
		free(filteredImage);
	}
	free(filteredImagePerProcessor);
	free(unfilteredImagePerProcessor);
	free(medianFilter);
	MPI_Finalize();
	return 0;

}



