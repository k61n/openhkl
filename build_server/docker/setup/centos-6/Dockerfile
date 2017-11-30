FROM centos:6

# Install extra-packages
RUN yum install -y epel-release

RUN yum update -y

RUN yum install -y git                    \
                   eigen3-devel           \
                   hdf5-devel             \
                   libtiff-devel          \
                   fftw3-devel            \ 
                   qt5-qtbase-devel       \ 
                   python34-devel         \ 
                   python34-numpy         \
                   make                   \
                   which                  \
                   pcre-devel             \
                   zlib-devel             \
                   unzip                  \
                   redhat-lsb-core        \
                   rpm-build

# Recipe for getting newer g++ than the 4.4 provided by centos6
RUN yum install -y centos-release-scl          \
 && yum install -y devtoolset-4-gcc-c++.x86_64 \
 && yum clean all

ENV PATH=/opt/rh/devtoolset-4/root/usr/bin/:$PATH

ENV MAKEFLAGS=-j8

WORKDIR /tmp
ADD ./deps/ninja-1.8.2.tar.gz .
WORKDIR /tmp/ninja-1.8.2
RUN ./configure.py --bootstrap && mv ninja /usr/sbin

# CMake > 3.0 necessary for finding Python3 properly
WORKDIR /tmp
ADD ./deps/cmake-3.9.4.tar.gz .
WORKDIR /tmp/cmake-3.9.4
RUN ./configure --prefix=/usr/local && make && make install
WORKDIR /tmp/
RUN rm -rf cmake-3.9.4

# Yaml >= 0.5 necessary
WORKDIR /tmp
ADD ./deps/yaml-cpp-master.zip .
RUN unzip yaml-cpp-master.zip
WORKDIR /tmp/yaml-cpp-master/build
RUN cmake -DBUILD_SHARED_LIBS=ON .. && make && make install
WORKDIR /tmp/
RUN rm -rf yaml-cpp-master && rm yaml-cpp-master.zip

# NSXTool requires boost > 1.54 for building (fPIC are used to insure boost static binding to nsx dynamic library)
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

