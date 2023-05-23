FROM ubuntu:22.04

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
      swig \
      python3-pip \
      python3-sphinx \
      python3-sphinxcontrib.bibtex \
      python3-breathe \
      python-sphinx-press-theme \
      doxygen \
      graphviz && \
      rm -rf /var/lib/apt/lists/*
