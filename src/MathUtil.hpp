#pragma once

#include <vector>

namespace MathUtil
{
	bool is_power_of_2(int n);
	std::vector<double> transpose_matrix(std::vector<double> data, int rows, int columns);
	std::vector<double> DFT_2D(std::vector<double> data, int rows, int columns);
}
