FROM opensuse:42.3

RUN zypper --non-interactive update

RUN zypper --non-interactive install git                 \
                                     make                \
                                     gcc-c++             \
                                     eigen3-devel        \
                                     hdf5-devel          \
                                     libtiff-devel       \
                                     fftw3-devel         \
                                     libqt5-qtbase-devel \
                                     python3-devel       \
                                     python3-numpy       \
                                     python3-numpy-devel \
                                     yaml-cpp-devel      \
                                     pcre-devel          \
                                     zlib-devel          \
                                     ninja               \
                                     lsb5-core           \
                                     rpm-build           \
 && zypper clean --all

# CMake > 3.0 necessary for finding Python3 properly
WORKDIR /tmp
ADD ./deps/cmake-3.9.4.tar.gz .
WORKDIR /tmp/cmake-3.9.4
RUN ./configure --prefix=/usr/local && make && make install
WORKDIR /tmp/
RUN rm -rf cmake-3.9.4

# NSXTool requires boost > 1.54 for building (fPIC are used to insure boost static binding to nsx dynamic library)
WORKDIR /tmp
ADD ./deps/boost_1_59_0.tar.gz .
WORKDIR /tmp/boost_1_59_0
RUN ./bootstrap.sh --with-libraries=date_time,test,filesystem --prefix=/usr/local && ./b2 cxxflags=-fPIC cflags=-fPIC && ./b2 install
WORKDIR /tmp/
RUN rm -rf boost_1_59_0

# Swig > 3.0.10 necessary for building
WORKDIR /tmp
ADD ./deps/swig-3.0.12.tar.gz .
WORKDIR /tmp/swig-3.0.12
RUN ./configure --prefix=/usr/local && make && make install
WORKDIR /tmp/
RUN rm -rf swig-3.0.12
