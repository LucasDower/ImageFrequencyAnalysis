#include <string>
#include <iostream>
#include <array>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

short FFT(short int dir, long m, double* x, double* y)
{
    long n, i, i1, j, k, i2, l, l1, l2;
    double c1, c2, tx, ty, t1, t2, u1, u2, z;

    /* Calculate the number of points */
    n = 1;
    for (i = 0; i < m; i++)
        n *= 2;

    /* Do the bit reversal */
    i2 = n >> 1;
    j = 0;
    for (i = 0; i < n - 1; i++) {
        if (i < j) {
            tx = x[i];
            ty = y[i];
            x[i] = x[j];
            y[i] = y[j];
            x[j] = tx;
            y[j] = ty;
        }
        k = i2;
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }

    /* Compute the FFT */
    c1 = -1.0;
    c2 = 0.0;
    l2 = 1;
    for (l = 0; l < m; l++) {
        l1 = l2;
        l2 <<= 1;
        u1 = 1.0;
        u2 = 0.0;
        for (j = 0; j < l1; j++) {
            for (i = j; i < n; i += l2) {
                i1 = i + l1;
                t1 = u1 * x[i1] - u2 * y[i1];
                t2 = u1 * y[i1] + u2 * x[i1];
                x[i1] = x[i] - t1;
                y[i1] = y[i] - t2;
                x[i] += t1;
                y[i] += t2;
            }
            z = u1 * c1 - u2 * c2;
            u2 = u1 * c2 + u2 * c1;
            u1 = z;
        }
        c2 = sqrt((1.0 - c1) / 2.0);
        if (dir == 1)
            c2 = -c2;
        c1 = sqrt((1.0 + c1) / 2.0);
    }

    /* Scaling for forward transform */
    if (dir == 1) {
        for (i = 0; i < n; i++) {
            x[i] /= n;
            y[i] /= n;
        }
    }

    return true;
}

int Powerof2(int n, int* m, int* twopm)
{
    if (n <= 1) {
        *m = 0;
        *twopm = 1;
        return false;
    }

    *m = 1;
    *twopm = 2;
    do {
        (*m)++;
        (*twopm) *= 2;
    } while (2 * (*twopm) <= n);

    if (*twopm != n)
        return false;
    else
        return true;
}

int FFT2D(double** re, double** im, int nx, int ny, int dir)
{
    int i, j;
    int m, twopm;
    double* real, * imag;

    /* Transform the rows */
    real = (double*)malloc(nx * sizeof(double));
    imag = (double*)malloc(nx * sizeof(double));
    if (real == NULL || imag == NULL)
        return false;
    if (!Powerof2(nx, &m, &twopm) || twopm != nx)
        return false;
    for (j = 0; j < ny; j++) {
        for (i = 0; i < nx; i++) {
            real[i] = re[i][j];
            imag[i] = im[i][j];
        }
        FFT(dir, m, real, imag);
        for (i = 0; i < nx; i++) {
            re[i][j] = real[i];
            im[i][j] = imag[i];
        }
    }
    free(real);
    free(imag);

    /* Transform the columns */
    real = (double*)malloc(ny * sizeof(double));
    imag = (double*)malloc(ny * sizeof(double));
    if (real == NULL || imag == NULL)
        return false;
    if (!Powerof2(ny, &m, &twopm) || twopm != ny)
        return false;
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {
            real[j] = re[i][j];
            imag[j] = im[i][j];
        }
        FFT(dir, m, real, imag);
        for (j = 0; j < ny; j++) {
            re[i][j] = real[j];
            im[i][j] = imag[j];
        }
    }
    free(real);
    free(imag);

    return true;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cerr << "Usage: ./ImageFrequencyAnalysis input_file output_file\n";
        return 1;
    }
    int width = 0, height = 0, nrChannels = 0;
    unsigned char* data = stbi_load(argv[1], &width, &height, &nrChannels, STBI_grey);
    if (!data)
    {
        cerr << stbi_failure_reason();
        return 1;
    }
    // Initialise arrays
    double** re;
    double** im;
    re = new double* [width];
    im = new double* [width];
    for (int i = 0; i < width; i++)
    {
        re[i] = new double[height];
        im[i] = new double[height];
    }
    // Populate arrays
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            re[i][j] = data[j * width + i];
            im[i][j] = 0;
        }
    }
    // Perform Fast Fourier Transform.
    FFT2D(re, im, width, height, 1);
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            double mag = sqrt(re[i][j] * re[i][j] + im[i][j] * im[i][j]);
            int scale = 46;
            int translated_x = (i + width / 2) % width;
            int translated_y = (j + height / 2) % height;
            data[translated_y * width + translated_x] = scale * log(1 + mag);
        }
    }
    int res = stbi_write_bmp(argv[2], width, height, 1, data);
    stbi_image_free(data);
	return 0;
}