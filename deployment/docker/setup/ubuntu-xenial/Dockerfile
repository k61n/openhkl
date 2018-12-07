FROM ubuntu:xenial

RUN export DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       cmake \
       git \
       g++ \
       ninja-build \
       libeigen3-dev \
       libhdf5-dev \
       libtiff5-dev \
       libfftw3-dev \
       qtbase5-dev \
       libqt5opengl5-dev \
       qt5-default \
       python3-dev \
       python3-numpy \
       libyaml-cpp-dev \
       libpcre++-dev \
       lsb-release \
    && apt-get autoclean \
    && apt-get autoremove

# Swig > 3.0.10 necessary for building
WORKDIR /tmp
ADD ./deps/swig-3.0.12.tar.gz .
WORKDIR /tmp/swig-3.0.12
RUN ./configure --prefix=/usr/local && make && make install
WORKDIR /tmp/
RUN rm -rf swig-3.0.12

# NSXTool requires boost > 1.54 for building (fPIC are used to insure boost static binding to nsx dynamic library)
ADD ./deps/boost_1_59_0.tar.gz .
WORKDIR /tmp/boost_1_59_0
RUN ./bootstrap.sh --with-libraries=date_time,test,filesystem --prefix=/usr/local && ./b2 cxxflags=-fPIC cflags=-fPIC && ./b2 install
WORKDIR /tmp/
RUN rm -rf boost_1_59_0

