#include "MathUtil.hpp"
#include "FastDCTLee.hpp"

#include <iostream>
#include <stdexcept>


bool MathUtil::isPowerOf2(int n)
{
	return (n & (n - 1)) == 0;
}

std::vector<double> MathUtil::transposeMatrix(std::vector<double> data, int rows, int columns)
{
	std::vector<double> output;
	for (long long i = 0; i < columns; ++i)
	{
		for (long long j = 0; j < rows; j++)
		{
			output.push_back(data[j * columns + i]);
		}
	}
	return output;
}

std::vector<double> dftRows(std::vector<double> data, int rows, int columns)
{
	std::vector<double> output;
	for (long long i = 0; i < rows; i++)
	{
		std::vector<double> values;
		for (long long j = 0; j < columns; j++)
		{
			values.push_back(data[(columns * i) + j]);
		}
		values[0] *= 1.0 / sqrt(2.0);
		FastDctLee::transform(values);
		for (long long j = 0; j < columns; j++)
		{
			output.push_back(values[j] * sqrt(2.0 / columns));
		}
	}
	return output;
}

std::vector<double> MathUtil::dft2D(std::vector<double> data, int rows, int columns)
{
	if (!MathUtil::isPowerOf2(rows) || !MathUtil::isPowerOf2(columns))
	{
		throw std::invalid_argument("Dimensions must be powers of two");
	}
	data = dftRows(data, rows, columns);
	data = MathUtil::transposeMatrix(data, rows, columns);
	data = dftRows(data, rows, columns);
	return data;
}