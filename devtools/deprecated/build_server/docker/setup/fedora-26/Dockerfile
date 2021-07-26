FROM fedora:26

RUN dnf update -y

RUN dnf install -y git              \
                   cmake            \
                   gcc-c++          \
                   eigen3-devel     \
                   hdf5-devel       \
                   libtiff-devel    \
                   fftw3-devel      \ 
                   qt5-qtbase-devel \ 
                   python3-devel    \ 
                   python3-numpy    \
                   yaml-cpp-devel   \
                   ninja-build      \
                   swig             \
                   redhat-lsb-core  \
                   rpm-build        \
 && dnf clean all

# NSXTool requires boost > 1.54 for building (fPIC are used to insure boost static binding to nsx dynamic library)
WORKDIR /tmp
ADD ./deps/boost_1_59_0.tar.gz .
WORKDIR /tmp/boost_1_59_0
RUN ./bootstrap.sh --with-libraries=date_time,test,filesystem --prefix=/usr/local && ./b2 cxxflags=-fPIC cflags=-fPIC && ./b2 install
WORKDIR /tmp/
RUN rm -rf boost_1_59_0

