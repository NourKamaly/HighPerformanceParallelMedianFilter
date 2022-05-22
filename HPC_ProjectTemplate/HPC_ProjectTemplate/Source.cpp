#include <iostream>
#include <algorithm>
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
            //i * OriginalImageWidth + j
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


//
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
    int ImageWidth = 4, ImageHeight = 4;

    int start_s, stop_s, TotalTime = 0;

    System::String^ imagePath;
    std::string img;
    img = "..//Data//Input/test.png";

    imagePath = marshal_as<System::String^>(img);
    int* imageData = inputImage(&ImageWidth, &ImageHeight, imagePath);

    int filter_size;
    int* ImageOutput = new int[ImageHeight * ImageWidth];

    cout << "Enter the size of the filter: "; cin >> filter_size;

    Midean_Filter(ImageHeight, ImageWidth, filter_size, imageData, ImageOutput);

    start_s = clock();
    createImage(ImageOutput, ImageWidth, ImageHeight, 0);
    stop_s = clock();

    TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
    cout << "time: " << TotalTime << endl;

    free(imageData);
    system("pause");
    return 0;

}