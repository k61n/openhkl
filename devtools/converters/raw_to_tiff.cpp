/*
 * raw_to_tiff.cpp
 * author: Jonathan Fisher
 *         j.fisher@fz-juelich.de, jonathan.m.fisher@gmail.com
 *         Forschungszentrum Juelich GmbH
 *
 * desc: simple command-line utitily to convert a raw binary file to grayscale
 * tiff
 */

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <tiffio.h>
#include <vector>

using std::atoi;
using std::cout;
using std::endl;
using std::fstream;
using std::ios_base;
using std::string;
using std::vector;

template <typename NumericType> double compute_average(const void* void_data, int length)
{
    const NumericType* data = reinterpret_cast<const NumericType*>(void_data);

    double average = 0.0;
    double factor = 1.0 / static_cast<double>(length);

    for (int i = 0; i < length; ++i)
        average += data[i] * factor;

    return average;
}

void print_usage()
{
    cout << "usage: raw_to_tiff filename width height bpp [swap_endian] \n\n"
         << "Convert raw binary data to greyscale tiff,\n\n"
         << "filename  name of input file\n"
         << "width     width in pixels\n"
         << "height    height in pixels\n"
         << "bpp       bytes per pixel (1 or 2)\n"
         << "\n"
         << "NOTE: the filesize MUST be exactly equal to width*height*bpp" << endl;
}

void swap_endian(vector<char>& data)
{
    size_t size = data.size() / 2;

    for (size_t i = 0; i < size; ++i) {
        char c = data[2 * i];
        data[2 * i] = data[2 * i + 1];
        data[2 * i + 1] = c;
    }
}

int write_tiff(const string filename, int width, int height, int bpp, char* buffer)
{
    TIFF* file;

    file = TIFFOpen(filename.c_str(), "w");

    if (!file) {
        cout << "Error: could not open " << filename << " for writing" << endl;
        return 1;
    }

    TIFFSetField(file, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(file, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(file, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(file, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);

    TIFFSetField(file, TIFFTAG_BITSPERSAMPLE, bpp * 8);
    TIFFSetField(file, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(file, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

    for (int i = 0; i < height; ++i)
        TIFFWriteScanline(file, &buffer[i * width * bpp], i);

    TIFFClose(file);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 5) {
        print_usage();
        return 0;
    }

    int width, height, bpp, filesize;
    string filename;
    fstream file;
    vector<char> data;

    width = atoi(argv[2]);
    height = atoi(argv[3]);
    bpp = atoi(argv[4]);

    if (width <= 0) {
        cout << "Error: width must be positive" << endl;
        return 1;
    }

    if (height <= 0) {
        cout << "Error: height must be positive" << endl;
        return 1;
    }

    if (bpp != 1 && bpp != 2) {
        cout << "Error: bpp must take the value 1 or 2" << endl;
        return 1;
    }

    filename = argv[1];
    file.open(filename, ios_base::in | ios_base::binary);

    if (!file.is_open()) {
        cout << "Error: could not open " << filename << "for reading." << endl;
        return 1;
    }

    // get filesize
    file.seekg(0, ios_base::end);
    filesize = file.tellg();
    file.seekg(0, ios_base::beg);

    // assert that data has expected size
    if (filesize != width * height * bpp) {
        cout << "Error: raw data size must equal width*height*bpp" << endl;
        return 1;
    }

    // allocate buffer and read file
    data.resize(filesize);
    file.read(&data[0], filesize);

    // failure to read all bytes
    if (!file) {
        cout << "Error: could not read input file: only " << file.gcount() << " bytes read" << endl;
        return 1;
    }

    // close file since we are finished with it
    file.close();

    /*if ( bpp == 2) {
        // check for problems with enddianness
        double avg = compute_average<uint16_t>(&data[0], width*height);

        if ( avg > 16000.0 ) {
            cout << "The average is to high, swapping endianness...  ";
            swap_endian(data);
            avg = compute_average<uint16_t>(&data[0], width*height);
        }

        cout << "The average is " << avg << endl;
        }*/

    // swap enddianness
    if (argc >= 6 && string("swap_endian") == argv[5]) {
        cout << "swapping endianness" << endl;
        swap_endian(data);
    }

    // remove .raw from filename if necessary
    int len = filename.size();

    if (len > 4)
        if (filename.substr(len - 4, 4) == ".raw")
            filename = filename.substr(0, len - 4);

    // write the tiff file
    if (write_tiff(filename + ".tiff", width, height, bpp, &data[0])) {
        cout << "Error: could not successfully write tiff!" << endl;
        return 1;
    }

    return 0;
}
