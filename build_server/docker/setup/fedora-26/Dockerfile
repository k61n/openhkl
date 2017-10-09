FROM fedora:26

RUN yum update -y

RUN yum install -y git                    \
                   cmake                  \
                   gcc-c++                \
                   eigen3-devel           \
                   hdf5-devel             \
                   libtiff-devel          \
                   fftw3-devel            \ 
                   qt5-qtbase-devel       \ 
                   python3-devel         \ 
                   python3-numpy         \
                   yaml-cpp-devel         \
                   ninja-build            \
                   swig                   \
                   boost-date-time        \
                   boost-filesystem       \
                   boost-test             \
                   boost-devel            \
                   swig                   \
 && yum clean all

