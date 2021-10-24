#include <assert.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <png++/png.hpp>
#include <vector>

#include "utils.h"

using namespace std;

// fix this by retuning a heap allocated object instead
Image *loadImage_correct(const char *filename) {
  png::image<png::rgb_pixel> image(filename);
  Image *imageMatrix =
      new Image(3, Matrix(image.get_height(), Array(image.get_width())));

  int h, w;
  for (h = 0; h < image.get_height(); h++) {
    for (w = 0; w < image.get_width(); w++) {
      (*imageMatrix)[0][h][w] = image[h][w].red;
      (*imageMatrix)[1][h][w] = image[h][w].green;
      (*imageMatrix)[2][h][w] = image[h][w].blue;
    }
  }

  return imageMatrix;
}

// This is corerct, but slow
Image applyFilter_high_miss(Image &image, Matrix &filter) {
  assert(image.size() == 3 && filter.size() != 0);

  int height = image[0].size();
  int width = image[0][0].size();
  int filterHeight = filter.size();
  int filterWidth = filter[0].size();
  int newImageHeight = height - filterHeight + 1;
  int newImageWidth = width - filterWidth + 1;
  int d, i, j, h, w;

  Image newImage(3, Matrix(newImageHeight, Array(newImageWidth)));

  for (j = 0; j < newImageWidth; j++) {
    for (i = 0; i < newImageHeight; i++) {
      for (w = j; w < j + filterWidth; w++) {
        for (h = i; h < i + filterHeight; h++) {
          for (d = 0; d < 3; d++) {
            newImage[d][i][j] += filter[h - i][w - j] * image[d][h][w];
          }
        }
      }
    }
  }

  return newImage;
}

int main() {
  Matrix filter = getGaussian(5, 5, 10.0);
  cout << "Loading image..." << endl;
  Image *image = loadImage_correct("large_cat.png");

  cout << "Applying filter..." << endl;
  auto start = std::chrono::steady_clock::now();
  Image newImage = applyFilter_high_miss(*image, filter);
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;

  cout << "Saving image..." << endl;
  saveImage(newImage, "newImage.png");

  delete image;
  cout << "Done! ApplyFilter Took " << elapsed_seconds.count() * 1000 << " ms "
       << endl;
  return 0;
}
