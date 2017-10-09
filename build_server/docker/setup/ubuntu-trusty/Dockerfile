FROM ubuntu:14.04

RUN export DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       build-essential \
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
       qtbase5-dev \
       libqt5opengl5-dev \
       python3-dev \
       python3-numpy \
       libyaml-cpp-dev \
       libpcre++-dev \
    && apt-get autoclean \
    && apt-get autoremove

WORKDIR /tmp
<<<<<<< HEAD
ADD ../deps/swig-3.0.12.tar.gz .
WORKDIR /tmp/swig-3.0.12
RUN ./configure --prefix=/usr/local && make && make install
WORKDIR /tmp/
=======

ADD swig-3.0.12.tar.gz .

WORKDIR /tmp/swig-3.0.12

RUN ./configure --prefix=/usr/local && make && make install

WORKDIR /tmp/

>>>>>>> f964ba3e887facf634dd2227b3091785a1ad9448
RUN rm -rf swig-3.0.12

