#include <iostream>
#include <vector>
#include <assert.h>
#include <cmath>
#include <chrono>
#include <png++/png.hpp>

using namespace std;

typedef vector<double> Array;
typedef vector<Array> Matrix;
typedef vector<Matrix> Image;

Matrix getGaussian(int height, int width, double sigma)
{
    Matrix kernel(height, Array(width));
    double sum=0.0;
    int i,j;

    for (i=0 ; i<height ; i++) {
        for (j=0 ; j<width ; j++) {
            kernel[i][j] = exp(-(i*i+j*j)/(2*sigma*sigma))/(2*M_PI*sigma*sigma);
            sum += kernel[i][j];
        }
    }

    for (i=0 ; i<height ; i++) {
        for (j=0 ; j<width ; j++) {
            kernel[i][j] /= sum;
        }
    }

    return kernel;
}


void saveImage(Image &image, const char *filename)
{
    assert(image.size()==3);

    int height = image[0].size();
    int width = image[0][0].size();
    int x,y;

    png::image<png::rgb_pixel> imageFile(width, height);

    for (y=0 ; y<height ; y++) {
        for (x=0 ; x<width ; x++) {
            imageFile[y][x].red = image[0][y][x];
            imageFile[y][x].green = image[1][y][x];
            imageFile[y][x].blue = image[2][y][x];
        }
    }
    imageFile.write(filename);
}


// has problem
Image& loadImage(const char *filename)
{
    png::image<png::rgb_pixel> image(filename);
    Image imageMatrix(3, Matrix(image.get_height(), Array(image.get_width())));

    int h,w;
    for (h=0 ; h<image.get_height() ; h++) {
        for (w=0 ; w<image.get_width() ; w++) {
            imageMatrix[0][h][w] = image[h][w].red;
            imageMatrix[1][h][w] = image[h][w].green;
            imageMatrix[2][h][w] = image[h][w].blue;
        }
    }

    return imageMatrix;
}


// has another problem
Image* loadImage_v2(const char *filename)
{
    png::image<png::rgb_pixel> image(filename);
    Image* imageMatrix = new Image(3, Matrix(image.get_height(), Array(image.get_width())));

    int h,w;
    for (h=0 ; h<image.get_height() ; h++) {
        for (w=0 ; w<image.get_width() ; w++) {
            (*imageMatrix)[0][h][w] = image[h][w].red;
            (*imageMatrix)[1][h][w] = image[h][w].green;
            (*imageMatrix)[2][h][w] = image[h][w].blue;
        }
    }

    return imageMatrix;
}


Image applyFilter(Image &image, Matrix &filter){
    assert(image.size()==3 && filter.size()!=0);

    int height = image[0].size();
    int width = image[0][0].size();
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();
    int newImageHeight = height - filterHeight + 1;
    int newImageWidth = width - filterWidth + 1;
    int d,i,j,h,w;

    Image newImage(3, Matrix(newImageHeight, Array(newImageWidth)));

    for (d = 0 ; d < 3 ; d++) {
        for (i = 0 ; i < newImageWidth ; i++) {
            for (j=0 ; j< newImageHeight; j++) {
                for (h=i ; h<i+filterHeight ; h++) {
                    for (w=j ; w<j+filterWidth ; w++) {
                        newImage[d][i][j] += filter[h-i][w-j]*image[d][h][w];
                    }
                }
            }
        }
    }

    return newImage;
}

// This is corerct, but slow
Image applyFilter_v2(Image &image, Matrix &filter){
    assert(image.size()==3 && filter.size()!=0);

    int height = image[0].size();
    int width = image[0][0].size();
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();
    int newImageHeight = height - filterHeight + 1;
    int newImageWidth = width - filterWidth + 1;
    int d,i,j,h,w;

    Image newImage(3, Matrix(newImageHeight, Array(newImageWidth)));

    for (j=0 ; j<newImageWidth ; j++) {
    for (i=0 ; i<newImageHeight ; i++) {
    for (w=j ; w<j+filterWidth ; w++) {
    for (h=i ; h<i+filterHeight ; h++) {
    for (d=0 ; d<3 ; d++) {
    		        newImage[d][i][j] += filter[h-i][w-j]*image[d][h][w];
                    }
                }
            }
        }
    }

    return newImage;
}

// this is correct and fast
Image applyFilter_v3(Image &image, Matrix &filter){
    assert(image.size()==3 && filter.size()!=0);

    int height = image[0].size();
    int width = image[0][0].size();
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();
    int newImageHeight = height - filterHeight + 1;
    int newImageWidth = width - filterWidth + 1;
    int d,i,j,h,w;

    Image newImage(3, Matrix(newImageHeight, Array(newImageWidth)));

    for (d=0 ; d<3 ; d++) {
        for (i=0 ; i<newImageHeight ; i++) {
            for (j=0 ; j<newImageWidth ; j++) {
                for (h=i ; h<i+filterHeight ; h++) {
                    for (w=j ; w<j+filterWidth ; w++) {
                        newImage[d][i][j] += filter[h-i][w-j]*image[d][h][w];
                    }
                }
            }
        }
    }

    return newImage;
}

Image applyFilter(Image &image, Matrix &filter, int times)
{
    Image newImage = image;
    for(int i=0 ; i<times ; i++) {
        newImage = applyFilter(newImage, filter);
    }
    return newImage;
}

int main()
{
    Matrix filter = getGaussian(5, 5, 10.0);

    cout << "Loading image..." << endl;
    Image* image = loadImage_v2("image.png");

    cout << "Applying filter..." << endl;
    auto start = std::chrono::steady_clock::now();

    Image newImage = applyFilter_v3(*image, filter);

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;

    cout << "Saving image..." << endl;
    saveImage(newImage, "newImage.png");

    // delete image;
    cout << "Done! ApplyFilter Took " << elapsed_seconds.count() * 1000 << " ms " << endl;

}
