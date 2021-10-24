#include <assert.h>
#include <cmath>
#include <png++/png.hpp>

#include "utils.h"

Matrix getGaussian(int height, int width, double sigma) {
  Matrix kernel(height, Array(width));
  double sum = 0.0;
  int i, j;

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      kernel[i][j] = exp(-(i * i + j * j) / (2 * sigma * sigma)) /
                     (2 * M_PI * sigma * sigma);
      sum += kernel[i][j];
    }
  }

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      kernel[i][j] /= sum;
    }
  }

  return kernel;
}

void saveImage(Image &image, const char *filename) {
  assert(image.size() == 3);

  int height = image[0].size();
  int width = image[0][0].size();
  int x, y;

  png::image<png::rgb_pixel> imageFile(width, height);

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      imageFile[y][x].red = image[0][y][x];
      imageFile[y][x].green = image[1][y][x];
      imageFile[y][x].blue = image[2][y][x];
    }
  }
  imageFile.write(filename);
}
