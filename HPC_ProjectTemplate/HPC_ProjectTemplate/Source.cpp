#include <iostream>
#include <math.h>
#include <stdlib.h>
#include<string.h>
#include<msclr\marshal_cppstd.h>
#include <ctime>// include this header 
#include <mpi.h>
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


	int OriginalImageWidth, OriginalImageHeight;

	//*********************************************************Read Image and save it to local arrayss*************************	
	//Read Image and save it to local arrayss

	System::Drawing::Bitmap BM(imagePath);

	OriginalImageWidth = BM.Width;
	OriginalImageHeight = BM.Height;
	*w = BM.Width;
	*h = BM.Height;
	int *Red = new int[BM.Height * BM.Width];
	int *Green = new int[BM.Height * BM.Width];
	int *Blue = new int[BM.Height * BM.Width];
	input = new int[BM.Height*BM.Width];
	for (int i = 0; i < BM.Height; i++)
	{
		for (int j = 0; j < BM.Width; j++)
		{
			System::Drawing::Color c = BM.GetPixel(j, i);

			Red[i * BM.Width + j] = c.R;
			Blue[i * BM.Width + j] = c.B;
			Green[i * BM.Width + j] = c.G;

			input[i*BM.Width + j] = ((c.R + c.B + c.G) / 3); //gray scale value equals the average of RGB values

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
			//i * OriginalImageWidth + j
			if (image[i*width + j] < 0)
			{
				image[i*width + j] = 0;
			}
			if (image[i*width + j] > 255)
			{
				image[i*width + j] = 255;
			}
			System::Drawing::Color c = System::Drawing::Color::FromArgb(image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j]);
			MyNewImage.SetPixel(j, i, c);
		}
	}
	MyNewImage.Save("..//Data//Output//outputRes" + index + ".png");
	cout << "result Image Saved " << index << endl;
}


int main()
{
	MPI_Init(NULL, NULL);
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	cout << rank << endl;

	int ImageWidth = 4, ImageHeight = 4;
	int* imageData = NULL;
	int filterSize;
	cin >> filterSize;
	int start_s, stop_s, TotalTime = 0;
	if (rank == 0) {
		System::String^ imagePath;
		std::string img;
		img = "..//Data//Input//test.png";
		imagePath = marshal_as<System::String^>(img);
		imageData = inputImage(&ImageWidth, &ImageHeight, imagePath);

		//write code here
		cout << endl << imageData << endl;
		cout << ImageHeight << endl << ImageWidth << endl;

		start_s = clock();
		createImage(imageData, ImageWidth, ImageHeight, 11);
		stop_s = clock();
		TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
		cout << "time: " << TotalTime << endl;
	}
	MPI_Bcast(&ImageHeight, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&ImageWidth, 1, MPI_INT, 0, MPI_COMM_WORLD);
	int numOfBlocks = ImageHeight / size, extraRowsForBoundanarySubArray = int(filterSize/2);
	int extraRowsForMiddleSubArrays = filterSize - 1;
	MPI_Datatype boundarySubArray;
	// height is the row, width is the column
	int imageSize[2] = {ImageHeight,ImageWidth};
	int boundarySubsize[2] = {numOfBlocks+extraRowsForBoundanarySubArray,ImageWidth};
	int boundaryStart[2];
	boundaryStart[1] = 0;
	if (rank == 0) {
		boundaryStart[0] = 0;
	}
	else if (rank == size - 1) {
		boundaryStart[0] = numOfBlocks * rank - extraRowsForBoundanarySubArray;
	}
	PMPI_Type_create_subarray(2, imageSize, boundarySubsize, boundaryStart, MPI_ORDER_C,MPI_INT, & boundarySubArray);
	MPI_Type_commit(&boundarySubArray);

	MPI_Datatype middleSubArray;
	
	if (rank == 0) {
		free(imageData);
	}
	MPI_Finalize();
	return 0;

}



