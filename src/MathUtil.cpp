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

void MathUtil::dft1D(std::vector<double>& data, int direction)
{
	if (direction == 1)
	{
		data[0] *= 1.0 / sqrt(2.0);
		FastDctLee::transform(data);
		for (int i = 0; i < data.size(); i++)
		{
			data[i] *= sqrt(2.0 / data.size());
		}
		return;
	}
	else
	{
		for (int i = 0; i < data.size(); i++)
		{
			data[i] *= sqrt(data.size() / 2.0);
		}
		FastDctLee::inverseTransform(data);
		data[0] *= sqrt(2.0);
		for (int i = 0; i < data.size(); i++)
		{
			data[i] /= (data.size() / 2.0);
		}
		return;
	}
	return;
}

std::vector<double> dftRows(std::vector<double> data, int rows, int columns, int direction)
{
	std::vector<double> output;
	for (long long i = 0; i < rows; i++)
	{
		std::vector<double> row_values;
		for (long long j = 0; j < columns; j++)
		{
			row_values.push_back(data[(columns * i) + j]);
		}
		MathUtil::dft1D(row_values, direction);
		for (long long j = 0; j < columns; j++)
		{
			output.push_back(row_values[j]);
		}
	}
	return output;
}

std::vector<double> MathUtil::dft2D(std::vector<double> data, int rows, int columns, int direction)
{
	if (!MathUtil::isPowerOf2(rows) || !MathUtil::isPowerOf2(columns))
	{
		throw std::invalid_argument("Dimensions must be powers of two");
	}
	data = dftRows(data, rows, columns, direction);
	data = MathUtil::transposeMatrix(data, rows, columns);
	data = dftRows(data, rows, columns, direction);
	return data;
}