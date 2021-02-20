#pragma once

#include <vector>

namespace MathUtil
{
	bool isPowerOf2(int n);
	std::vector<double> transposeMatrix(std::vector<double> data, int rows, int columns);
	void dft1D(std::vector<double>& data, int direction);
	std::vector<double> dft2D(std::vector<double> data, int rows, int columns, int direction);
}
