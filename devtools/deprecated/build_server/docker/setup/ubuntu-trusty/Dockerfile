FROM ubuntu:trusty

RUN export DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       build-essential \
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
       software-properties-common \
       python-software-properties \
    && apt-get autoclean \
    && apt-get autoremove

# Add a ppa for getting gcc >= 4.9 (necessary for regex related code)
apt-add-repository "deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu trusty main"
apt-get update
apt-get install g++-4.9
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.9

# Swig > 3.0.10 necessary for building
WORKDIR /tmp
ADD ../deps/swig-3.0.12.tar.gz .
WORKDIR /tmp/swig-3.0.12
RUN ./configure --prefix=/usr/local && make && make install
WORKDIR /tmp/
RUN rm -rf swig-3.0.12

# NSXTool requires boost > 1.54 for building (fPIC are used to insure boost static binding to nsx dynamic library)
ADD ../deps/boost_1_59_0.tar.gz .
WORKDIR /tmp/boost_1_59_0
RUN ./bootstrap.sh --with-libraries=date_time,test,filesystem --prefix=/usr/local && ./b2 cxxflags=-fPIC cflags=-fPIC && ./b2 install
WORKDIR /tmp/
RUN rm -rf boost_1_59_0

