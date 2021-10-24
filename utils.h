/*
 * =====================================================================================
 *
 *       Filename:  utils.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/23/2021 11:34:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author: Xin Li
 *   Organization:  
 *
 * =====================================================================================
 */

#pragma once

#include <vector>
using namespace std;
typedef vector<double> Array;
typedef vector<Array> Matrix; 
typedef vector<Matrix> Image;



Matrix getGaussian(int height, int width, double sigma);
void saveImage(Image &image, const char *filename);
