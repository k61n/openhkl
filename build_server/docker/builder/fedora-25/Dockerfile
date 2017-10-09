FROM eurydice76/nsxtool:setup-fedora-25

WORKDIR /tmp

RUN GIT_SSL_NO_VERIFY=true git clone -b develop https://code.ill.fr/scientific-software/nsxtool.git

WORKDIR /tmp/nsxtool/build

RUN cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_QTAPPS=ON -DCMAKE_INSTALL_PREFIX=./ -DBUILD_GSL=ON -DNSX_PYTHON3=ON ..
RUN cmake --build . --config Release && cmake --build . --config Release --target install

RUN export NSX_ROOT_DIR=/tmp/nsxtool/build/share/nsxtool&& ninja-build test

WORKDIR /tmp

RUN rm -rf nsxtool

