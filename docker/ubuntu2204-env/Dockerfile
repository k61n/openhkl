FROM ubuntu:23.04

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get install -y \
      build-essential \
      cmake \
      libblosc-dev \
      libeigen3-dev \
      libfftw3-dev \
      libgl-dev \
      libgsl-dev \
      libhdf5-dev \
      libopencv-dev \
      libpython3-dev \
      libqhull-dev \
      libtiff-dev \
      libyaml-cpp-dev \
      python3 \
      python3-numpy \
      qt5-qmake \
      qtbase5-dev \
      qtbase5-dev-tools \
      qtchooser \
      qt5dxcb-plugin \
      swig \
      python3-pip \
      git \
      git-lfs
RUN git clone https://iffgit.fz-juelich.de/neutron-simlab/openhkl.git
RUN cd openhkl && mkdir build && cd build && cmake .. && make
