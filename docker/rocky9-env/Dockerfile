FROM rockylinux:9
RUN  dnf install -y dnf-plugins-core \
  && dnf install -y epel-release \
  && dnf config-manager --enable crb \
  && dnf install -y \
      git \
      git-lfs \
      cmake \
      freeglut-devel \
      blosc-devel \
      fftw3-devel \
      mesa-libGL-devel \
      gsl-devel \
      libtiff-devel \
      libpng-devel \
      yaml-cpp-devel \
      python3-devel \
      python3-numpy \
      python3-pip \
      python3-sphinx \
      python3-breathe \
      qt5-qtbase-devel \
      mesa-libGL-devel \
      eigen3-devel \
      doxygen \
      graphviz-devel \
      hdf5-devel \
      swig \
      opencv-devel
RUN git clone https://github.com/qhull/qhull.git
RUN cd qhull \
  && cmake -B build -DCMAKE_BUILD_TYPE=RELEASE \
                    -DCMAKE_INSTALL_PREFIX=/usr/local \
  && cmake --build build -j10 \
  && cd build && make install
RUN python3 -m pip install sphinxcontrib-bibtex sphinx-press-theme
