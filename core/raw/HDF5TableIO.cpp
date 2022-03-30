#include "HDF5TableIO.h"

HDF5TableWriter::HDF5TableWriter(
    const H5::Group& datagroup, const H5::DataType& dtype, const std::size_t n_rows,
    const std::size_t n_cols, const std::string& dataset_name)
    : _dtype{dtype}
{
    // create the table as a DataSet of shape (n_rows, n_cols)
    const hsize_t dims[_rank] = {n_rows, n_cols};
    _fspace = H5::DataSpace(_rank, dims);
    _dataset = datagroup.createDataSet(dataset_name, _dtype, _fspace);
    // hyperslab from data in file (a single row), shape = (1, n_cols)
    _stride[0] = _block[0] = 1;
    _stride[1] = _block[1] = n_cols;
    // DataSpace for the data in memory (a single row)
    const hsize_t mdims[] = {n_cols};
    _mspace = H5::DataSpace(1, mdims);
}


HDF5TableWriter::~HDF5TableWriter()
{
    _fspace.selectNone();
}


void HDF5TableWriter::writeRow(const std::size_t row_idx, const void* const row_dataptr)
{
    // move the start position of the slab to the beginning of the row
    _start[0] = static_cast<hsize_t>(row_idx);
    _fspace.selectHyperslab(H5S_SELECT_SET, _count, _start, _stride, _block);
    _dataset.write(row_dataptr, _dtype, _mspace, _fspace);
}


HDF5TableReader::HDF5TableReader(const H5::DataSet& dataset)
    : _dataset{&dataset}, _dtype{dataset.getDataType()}, _fspace{dataset.getSpace()}
{
    // obtain DataSet properties
    // expects that rank = 2; ie. `_fspace.getSimpleExtentNdims() == 2`
    hsize_t fdims[_rank];
    _fspace.getSimpleExtentDims(fdims);
    n_rows = fdims[0];
    n_cols = fdims[1];
    // hyperslab from data in file (a single row), shape = (1, n_cols)
    _stride[0] = _block[0] = 1;
    _stride[1] = _block[1] = n_cols;
    // DataSpace for the data in memory (a single row)
    const hsize_t mdims[] = {n_cols};
    _mspace = H5::DataSpace(1, mdims);
}

HDF5TableReader::~HDF5TableReader()
{
    _fspace.selectNone();
}


void HDF5TableReader::readRow(const std::size_t row_idx, void* const row_dataptr)
{
    // move the start position of the slab to the beginning of the row
    _start[0] = static_cast<hsize_t>(row_idx);
    _fspace.selectHyperslab(H5S_SELECT_SET, _count, _start, _stride, _block);
    _dataset->read(row_dataptr, _dtype, _mspace, _fspace);
}
