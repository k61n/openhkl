#include <H5Cpp.h>
#include <string>

// Auxiliary class to write a 2d table to HDF5 database
class HDF5TableWriter {
 public:
    // Create a H5::DataSet inside the given H5::Group with the given details.
    HDF5TableWriter(
        const H5::Group& datagroup, const H5::DataType& dtype, const std::size_t n_rows,
        const std::size_t n_cols, const std::string& dataset_name);

    ~HDF5TableWriter();

    // Write a given row to the table.
    void writeRow(const std::size_t row_idx, const void* const row_dataptr);

 private:
    H5::DataSet _dataset;
    const H5::DataType _dtype;
    // DataSpace for the data in file; shape = (n_rows, n_cols)
    H5::DataSpace _fspace;
    // DataSpace for the data in memory (a single row)
    H5::DataSpace _mspace;
    static constexpr hsize_t _rank = 2;
    // hyperslab configuration
    hsize_t _start[_rank] = {0, 0};
    hsize_t _stride[_rank];
    hsize_t _block[_rank];
    const hsize_t _count[_rank] = {1, 1};
};


// Auxiliary class to read a 2d table from a HDF5 database
class HDF5TableReader {
 public:
    // Initialize reading of the H5::DataSet.
    HDF5TableReader(const H5::DataSet& dataset);

    ~HDF5TableReader();

    // Read a given row from the table
    void readRow(const std::size_t row_idx, void* const row_dataptr);

 public:
    hsize_t n_rows;
    hsize_t n_cols;

 private:
    const H5::DataSet* const _dataset;
    const H5::DataType _dtype;
    // DataSpace for the data in file; shape = (n_rows, n_cols)
    H5::DataSpace _fspace;
    // DataSpace for the data in memory (a single row)
    H5::DataSpace _mspace;
    // hyperslab configuration
    static constexpr hsize_t _rank = 2;
    hsize_t _start[_rank] = {0, 0};
    hsize_t _stride[_rank];
    hsize_t _block[_rank];
    const hsize_t _count[_rank] = {1, 1};
};
