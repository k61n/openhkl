/* 
 * filename: eigen.i
 * author: Jonathan Fisher (j.fisher@fz-juelich.de)
 * description: provide swig typemaps to give automatic conversion between eigen types and numpy arrays
 */

%include "numpy.i"

namespace Eigen {
    class Vector3d;
    class RowVector3d;
    class Matrix3d;
    class Quaterniond;
}

%{
#include <Python.h>
#include <numpy/arrayobject.h>
%}

%typemap(out) Eigen::VectorXd 
{ 
    npy_intp dims[1] = {$1.size()}; 
    PyObject* array = PyArray_SimpleNew(1, dims, NPY_DOUBLE); 
    double* data = ((double *)PyArray_DATA( array )); 
    for (int i = 0; i != dims[0]; ++i){ 
        *data++ = $1.data()[i]; 
    } 
    $result = array; 
}

%typemap(out) Eigen::Vector3d 
{ 
    npy_intp dims[1] = {3}; 
    PyObject* array = PyArray_SimpleNew(1, dims, NPY_DOUBLE); 
    double* data = ((double *)PyArray_DATA( array )); 
    for (int i = 0; i != dims[0]; ++i){ 
        *data++ = $1.data()[i];
        std::cout << "typemap out: " << $1.data()[i] << std::endl;
    } 
    $result = array; 
}

%typemap(out) Eigen::Matrix<double, 3, 1> 
{ 
    npy_intp dims[1] = {3}; 
    PyObject* array = PyArray_SimpleNew(1, dims, NPY_DOUBLE); 
    double* data = ((double *)PyArray_DATA( array )); 
    for (int i = 0; i != dims[0]; ++i){ 
        *data++ = $1.data()[i]; 
    } 
    $result = array; 
} 

%typemap(in) Eigen::MatrixXd (Eigen::MatrixXd TEMP)
{ 
  
  int rows = 0; 
  int cols = 0; 



  PyArrayObject* temp = nullptr;

  std::cout << "entering typemap in...." << std::endl;

  if (PyArray_Check($input)) {

        rows = PyArray_DIM($input,0); 
  cols = PyArray_DIM($input,1); 
      
    temp = (PyArrayObject*)$input;

    //PyArg_ParseTuple($input, "O", &temp);   

      TEMP.resize(rows,cols); 
      TEMP.fill(0); 

      double *  values = ((double *) PyArray_DATA($input)); 
      for (long int i = 0; i != rows; ++i){ 
          for(long int j = 0; j != cols; ++j){ 
              std::cout << "typemap in: " << values[i*rows+j] << std::endl; 
              TEMP(i,j) = values[i*rows+j]; 
          } 
      }   
  }
  $1 = TEMP;
  std::cout << "done typemape(in)" << std::endl;
} 

