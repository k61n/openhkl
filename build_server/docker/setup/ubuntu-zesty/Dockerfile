FROM ubuntu:17.04

RUN export DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       cmake \
       git \
       g++ \
       ninja-build \
       libboost-date-time-dev \
       libboost-filesystem-dev \
       libboost-test-dev\
       libeigen3-dev \
       libhdf5-dev \
       libtiff5-dev \
       libfftw3-dev \
       libgsl-dev \
       qtbase5-dev \
       libqt5opengl5-dev \
       python3-dev \
       python3-numpy \
       libyaml-cpp-dev \
       libpcre++-dev \
    && apt-get autoclean \
    && apt-get autoremove

WORKDIR /tmp

ADD swig-3.0.12.tar.gz .

WORKDIR /tmp/swig-3.0.12

RUN ./configure --prefix=/usr/local && make && make install

WORKDIR /tmp/

RUN rm -rf swig-3.0.12

