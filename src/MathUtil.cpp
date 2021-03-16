#include "MathUtil.hpp"
#include "FastDCTLee.hpp"

#include <iostream>
#include <stdexcept>
#include <thread>


bool math_util::is_power_of_2(const unsigned int n)
{
	return (n & (n - 1)) == 0;
}


void math_util::scale_data(double* data, const unsigned int size, const double factor)
{
	for (unsigned int i = 0; i < size; ++i)
	{
		data[i] *= factor;
	}
}


void math_util::transpose_square(double*& data, const unsigned int size)
{
	for (unsigned int i = 0; i < size; ++i)
	{
		for (auto j = i + 1; j < size; ++j)
		{
			const auto temp = data[i * size + j];
			data[i * size + j] = data[j * size + i];
			data[j * size + i] = temp;
		}
	}
}

void math_util::transpose_non_square(double*& data, const unsigned int rows, const unsigned int columns)
{
	const auto num_threads = std::thread::hardware_concurrency();
	const auto size = rows * columns;
	const auto elements_per_thread = size / num_threads;

	auto* output = new double[rows * columns];
	auto* threads = new std::thread[num_threads];

	// Dispatch threads
	if (elements_per_thread > 0)
	{
		for (int i = 0; i < num_threads; ++i)
		{
			int lower = i * elements_per_thread;
			int upper = lower + elements_per_thread;
			threads[i] = std::thread([data, lower, upper, columns, rows, output]()
				{
					int row = 0;
					int column = 0;
					for (int j = lower; j < upper; ++j)
					{
						column = j % columns;
						row = (j - column) / columns;
						output[row * columns + column] = data[column * columns + row];
					}
				});
		}
	}

	// Cleanup
	for (int j = num_threads * elements_per_thread; j < size; ++j)
	{
		int column = j % columns;
		int row = (j - column) / columns;
		output[row * columns + column] = data[column * columns + row];
	}

	// Wait for threads (if any dispatched)
	if (elements_per_thread > 0)
	{
		for (int i = 0; i < num_threads; ++i)
		{
			threads[i].join();
		}
	}

	double* temp = data;
	data = output;
	delete[] temp;
	delete[] threads;
}

void math_util::transpose_matrix(double*& data, const unsigned int rows, const unsigned int columns)
{
	if (rows == columns)
	{
		transpose_square(data, rows);
	}
	else
	{
		transpose_non_square(data, rows, columns);
	}
}


void math_util::dct_1d(double* data, const unsigned int size, const bool forward)
{
	if (forward)
	{
		data[0] /= sqrt(2.0);
		FastDctLee::transform(data, size);
		scale_data(data, size, sqrt(2.0 / size));
	}
	else
	{
		scale_data(data, size, sqrt(size / 2.0));
		FastDctLee::inverseTransform(data, size);
		data[0] *= sqrt(2.0);
	}
	scale_data(data, size, sqrt(2.0 / size));
}

void math_util::dct_rows_mt(double* data, const unsigned int rows, const unsigned int columns, const bool forward)
{
	const auto num_threads = std::thread::hardware_concurrency();
	const auto rows_per_thread = rows / num_threads;
	const auto block_size = rows_per_thread * columns;

	auto* threads = new std::thread[num_threads];

	// Dispatch threads
	for (unsigned int i = 0; i < num_threads; i++)
	{
		threads[i] = std::thread([block_size, data, columns, forward, i, rows_per_thread]()
			{
				for (unsigned int j = 0; j < rows_per_thread; ++j)
				{
					dct_1d(&data[block_size * i + j * columns], columns, forward);
				}
			});
	}

	// Tidy up
	for (auto i = rows_per_thread * num_threads; i < rows; i++)
	{
		dct_1d(&data[i * columns], columns, forward);
	}

	// Wait for threads
	for (unsigned int i = 0; i < num_threads; i++)
	{
		threads[i].join();
	}

	delete[] threads;
}


void math_util::dct_rows(double* data, const unsigned int rows, const unsigned int columns, const bool forward)
{
	for (unsigned int i = 0; i < rows; i++)
	{
		dct_1d(&data[i * columns], columns, forward);
	}
}


void math_util::dct_2d(double*& data, const unsigned int rows, const unsigned int columns, const bool forward)
{
	if (!is_power_of_2(rows) || !is_power_of_2(columns))
	{
		throw std::invalid_argument("Dimensions must be powers of two");
	}
	dct_rows_mt(data, rows, columns, forward);
	transpose_matrix(data, rows, columns);
	dct_rows_mt(data, columns, rows, forward);
}