FROM fedora:25

RUN dnf update -y

RUN dnf install -y git                    \
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
 && dnf clean all

