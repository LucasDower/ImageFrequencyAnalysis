#pragma once

#include <vector>

class math_util
{
public:
	static bool is_power_of_2(const unsigned int n);
	static void scale_data(double* data, const unsigned int size, const double factor);
	static void transpose_matrix(double*& data, const unsigned int rows, const unsigned int columns);
	static void dct_2d(double*& data, const unsigned int rows, const unsigned int columns, const bool forward);
	static unsigned char convert_rgb_to_bw(const unsigned char red, const unsigned char green, const unsigned char blue);
private:
	static void dct_rows(double* data, const unsigned int rows, const unsigned int columns, const bool forward);
	static void dct_rows_mt(double* data, const unsigned int rows, const unsigned int columns, const bool forward);
	static void dct_1d(double* data, const unsigned int size, const bool forward);
	static void transpose_non_square(double*& data, const unsigned int rows, const unsigned int columns);
	static void transpose_square(double*& data, const unsigned int size);
};
