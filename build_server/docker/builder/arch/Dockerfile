FROM base/archlinux

RUN pacman -Syy

RUN pacman -S --noconfirm git           \
                          eigen3        \
                          hdf5          \
                          libtiff       \
                          ninja         \
                          gcc           \
                          cmake         \
                          boost         \
                          yaml-cpp      \
                          swig          \
                          fftw          \
                          gsl           \
                          lsb-release   \
                          python3       \
                          python3-numpy \
                          qt5-base

ARG NSX_GIT_BRANCH=develop

WORKDIR /tmp/

RUN GIT_SSL_NO_VERIFY=true git clone -b ${NSX_GIT_BRANCH} https://code.ill.fr/scientific-software/nsxtool.git

WORKDIR /tmp/nsxtool/build

RUN cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_NSX_APPS=ON -DCMAKE_INSTALL_PREFIX=./ -DUSE_PYTHON3=ON .. \
 && cmake --build . --config Release \
 && cmake --build . --config Release --target install

RUN export NSX_ROOT_DIR=/tmp/nsxtool/build/share/nsxtool

RUN ninja test

              


