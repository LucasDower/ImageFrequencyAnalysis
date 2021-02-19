#include "MathUtil.hpp"
#include "FastDCTLee.hpp"

#include <iostream>
#include <stdexcept>


bool MathUtil::is_power_of_2(int n)
{
	return (n & (n - 1)) == 0;
}

std::vector<double> MathUtil::transpose_matrix(std::vector<double> data, int rows, int columns)
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

std::vector<double> DFT_Rows(std::vector<double> data, int rows, int columns)
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

std::vector<double> MathUtil::DFT_2D(std::vector<double> data, int rows, int columns)
{
	if (!MathUtil::is_power_of_2(rows) || !MathUtil::is_power_of_2(columns))
	{
		throw std::invalid_argument("Dimensions must be powers of two");
	}
	data = DFT_Rows(data, rows, columns);
	data = MathUtil::transpose_matrix(data, rows, columns);
	data = DFT_Rows(data, rows, columns);
	return data;
}