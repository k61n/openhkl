FROM rockylinux:8
RUN  dnf install -y dnf-plugins-core \
  && dnf install -y epel-release \
  && dnf install -y \
      git \
      git-lfs \
      cmake \
      gcc-toolset-13 \
      freeglut-devel \
      blosc-devel \
      fftw3-devel \
      mesa-libGL-devel \
      gsl-devel \
      libtiff-devel \
      libpng-devel \
      libjpeg-turbo-devel \
      yaml-cpp-devel \
      python3.11-devel \
      python3.11-numpy \
      python3.11-pip \
      qt5-qtbase-devel \
      swig \
      gtk2-devel \
      libwebp-devel \
      mesa-libGL-devel \
      boost \
      boost-thread \
      boost-devel \
      gstreamer1-plugins-base \
  && dnf install -y --enablerepo=powertools \
      eigen3-devel \
      doxygen \ 
      graphviz-devel \
      hdf5-devel \
      jasper-devel \
      openexr-devel \
      libv4l-devel
RUN git clone https://github.com/opencv/opencv.git
RUN cd opencv \
  && cmake -B build -DCMAKE_BUILD_TYPE=RELEASE \
                    -DCMAKE_INSTALL_PREFIX=/usr/local/ \
                    -DOPENCV_GENERATE_PKGCONFIG=ON \
  && cmake --build build -j10 \
  && cd build && make install 
RUN git clone https://github.com/qhull/qhull.git
RUN cd qhull \
  && cmake -B build -DCMAKE_BUILD_TYPE=RELEASE \
                    -DCMAKE_INSTALL_PREFIX=/usr/local \
  && cmake --build build -j10 \
  && cd build && make install
RUN git clone https://iffgit.fz-juelich.de/neutron-simlab/openhkl.git
RUN cd openhkl \
  && git lfs install \
  && CC=/opt/rh/gcc-toolset-13/root/bin/gcc \
     CXX=/opt/rh/gcc-toolset-13/root/bin/g++ \
     CMAKE_CXX_COMPILER_AR=/opt/rh/gcc-toolset-13/root/bin/gcc-ar \
     CMAKE_CXX_COMPILER_RANLIB=/opt/rh/gcc-toolset-13/root/bin/gcc-ranlib \
     cmake -B build \
     -DCMAKE_INSTALL_PREFIX=/usr/local/ \
  && cmake --build build -j10
RUN cd openhkl/build && ctest -j12
