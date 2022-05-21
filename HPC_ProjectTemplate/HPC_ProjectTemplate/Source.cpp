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


// function to swap elements
void swap(int* a, int* b) {
	int t = *a;
	*a = *b;
	*b = t;
}

// function to rearrange array (find the partition point)
int partition(int array[], int low, int high) {

	// select the rightmost element as pivot
	int pivot = array[high];

	// pointer for greater element
	int i = (low - 1);

	// traverse each element of the array
	// compare them with the pivot
	for (int j = low; j < high; j++) {
		if (array[j] <= pivot) {

			// if element smaller than pivot is found
			// swap it with the greater element pointed by i
			i++;

			// swap element at i with element at j
			swap(&array[i], &array[j]);
		}
	}

	// swap pivot with the greater element at i
	swap(&array[i + 1], &array[high]);

	// return the partition point
	return (i + 1);
}

void quickSort(int array[], int low, int high) {
	if (low < high) {
		int pi = partition(array, low, high);
		quickSort(array, low, pi - 1);
		quickSort(array, pi + 1, high);
	}
}


int main()
{
	int ImageWidth = 4, ImageHeight = 4;
	int start_s, stop_s, TotalTime = 0;
	System::String^ imagePath;
	std::string img;
	img = "..//Data//Input//test.png";
	imagePath = marshal_as<System::String^>(img);
	int* imageData = inputImage(&ImageWidth, &ImageHeight, imagePath);
	int filtersize;
	MPI_Init(NULL, NULL);
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 0)
	{
		start_s = clock();
		cout << "Enter the filter size: ";
		cin >> filtersize;
		int* filter = new int[filtersize * filtersize];

		//set all the the cells in the filter with zero
		memset(filter, 0, filtersize * filtersize);

		int indexOfCenter = filtersize / 2;

		int startRowFilter, StartColumnFilter;


		cout << "start calculating\n";
		for (int i = 0; i < ImageHeight; i++)
		{
			for (int j = 0; j < ImageWidth; j++)
			{
				startRowFilter = indexOfCenter;
				StartColumnFilter = indexOfCenter + 1;
				filter[indexOfCenter] = imageData[i * ImageWidth + j];
				for (int k = (i * ImageWidth + j); k < filtersize && k < ImageHeight; k++)
				{
					for (int l = (i * ImageWidth + j) + 1; l < filtersize && k < ImageWidth; l++)
					{
						filter[startRowFilter * filtersize + StartColumnFilter] = imageData[k * filtersize + l];
						StartColumnFilter++;
					}
					StartColumnFilter = 0;
					startRowFilter++;
				}

				startRowFilter = indexOfCenter;
				StartColumnFilter = indexOfCenter - 1;

				for (int k = (i * ImageWidth + j); k > filtersize && k > ImageHeight; k--)
				{
					for (int l = (i * ImageWidth + j) - 1; l > filtersize && k > ImageWidth; l--)
					{
						filter[startRowFilter * filtersize + StartColumnFilter] = imageData[k * filtersize + l];
						StartColumnFilter--;
					}
					StartColumnFilter = 0;
					startRowFilter--;
				}
				quickSort(filter, 0, (filtersize * filtersize));
				imageData[i * ImageWidth + j] = filter[indexOfCenter];
				memset(filter, 0, filtersize * filtersize);

			}
		}
		createImage(imageData, ImageWidth, ImageHeight, 0);
		stop_s = clock();
		TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
		cout << "time: " << TotalTime << endl;
		free(imageData);
		return 0;
	}
	MPI_Finalize();


}




