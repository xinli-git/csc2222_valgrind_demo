#include <iostream>
#include <vector>
#include <assert.h>
#include <cmath>
#include <chrono>
#include <png++/png.hpp>

using namespace std;

typedef vector<double> Array;
typedef vector<Array> Matrix; typedef vector<Matrix> Image;

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


// returning reference to local variable imageMatrix -> segfault
Image& loadImage_segfault(const char *filename)
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


// 
Image* loadImage_correct(const char *filename)
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


Image applyFilter_invalid_access(Image &image, Matrix &filter){
    assert(image.size()==3 && filter.size()!=0);

    int height = image[0].size();
    int width = image[0][0].size();
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();
    int newImageHeight = height - filterHeight + 1;
    int newImageWidth = width - filterWidth + 1;
    int d,i,j,h,w;

    Image newImage(3, Matrix(newImageHeight, Array(newImageWidth)));
    //Image* newImage = new Image(3, Matrix(newImageHeight, Array(newImageWidth)));
    //Image* imageMatrix = new Image(3, Matrix(image.get_height(), Array(image.get_width())));

    for (d = 0 ; d < 3 ; d++) {
        for (i = 0 ; i < newImageWidth ; i++) {
            for (j=0 ; j< newImageHeight; j++) {
        //for (i = 0 ; i < newImageHeight; i++) {
        //    for (j=0 ; j< newImageWidth; j++) {
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
Image applyFilter_high_miss(Image &image, Matrix &filter){
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
Image applyFilter_low_miss(Image &image, Matrix &filter){
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

void memcheck_segfault() {
    Matrix filter = getGaussian(5, 5, 10.0);

    cout << "Loading image..." << endl;
    Image image = loadImage_segfault("lorge_cat.png");

    //cout << "Applying filter..." << endl;

    //Image newImage = applyFilter(image, filter);

    //cout << "Saving image..." << endl;
    //saveImage(newImage, "newImage.png");
}

void memcheck_leak() {
    Matrix filter = getGaussian(5, 5, 10.0);

    cout << "Loading image..." << endl;
    Image* image = loadImage_correct("lorge_cat.png");
}


// this demo must be performed through valgrind, else the new image will not be saved.
void memcheck_invalid_access() {
    Matrix filter = getGaussian(5, 5, 10.0);

    cout << "Loading image..." << endl;
    Image* image = loadImage_correct("smol_cat.png");

    cout << "Applying filter..." << endl;
    Image newImage = applyFilter_invalid_access(*image, filter);

    cout << "Saving image..." << endl;
    saveImage(newImage, "new_smol_cat.png");

    delete image;
}

void cachegrind_high_miss() {
    Matrix filter = getGaussian(5, 5, 10.0);

    cout << "Loading image..." << endl;
    Image* image = loadImage_correct("lorge_cat.png");

    cout << "Applying filter..." << endl;
    auto start = std::chrono::steady_clock::now();

    Image newImage = applyFilter_high_miss(*image, filter);

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;

    cout << "Saving image..." << endl;
    saveImage(newImage, "new_lorge_cat.png");

!!!!!!!!!!!!!!!!!!!!!!!!!!111
!!!!!!!!!!!!!!!!!!!!!!!!!!111
!!!!!!!!!!!!!!!!!!!!!!!!!!111
!!!!!!!!!!!!!!!!!!!!!!!!!!111
!!!!!!!!!!!!!!!!!!!!!!!!!!111
!!!!!!!!!!!!!!!!!!!!!!!!!!111
!!!!!!!!!!!!!!!!!!!!!!!!!!111
!!!!!!!!!!!!!!!!!!!!!!!!!!111
!!!!!!!!!!!!!!!!!!!!!!!!!!111
!!!!!!!!!!!!!!!!!!!!!!!!!!111
!!!!!!!!!!!!!!!!!!!!!!!!!!111
!!!!!!!!!!!!!!!!!!!!!!!!!!111
    why is this faster than low miss bruuh

    // delete image;
    cout << "Done! High cache miss ApplyFilter Took " << elapsed_seconds.count() * 1000 << " ms " << endl;
}

void cachegrind_low_miss() {
    Matrix filter = getGaussian(5, 5, 10.0);

    cout << "Loading image..." << endl;
    Image* image = loadImage_correct("lorge_cat.png");

    cout << "Applying filter..." << endl;
    auto start = std::chrono::steady_clock::now();

    Image newImage = applyFilter_low_miss(*image, filter);

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;

    cout << "Saving image..." << endl;
    saveImage(newImage, "new_lorge_cat.png");

    // delete image;
    cout << "Done! Low cache miss ApplyFilter Took " << elapsed_seconds.count() * 1000 << " ms " << endl;
}


int main()
{
    //memcheck_segfault();
    //memcheck_leak();
    // need to run through valgrind
    //memcheck_invalid_access();
    //cachegrind_high_miss();
    //cachegrind_low_miss();
}
