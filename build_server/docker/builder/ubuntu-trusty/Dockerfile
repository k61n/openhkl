FROM eurydice76/nsxtool:setup-ubuntu-trusty

WORKDIR /tmp

RUN GIT_SSL_NO_VERIFY=true git clone -b feature/docker-support https://code.ill.fr/scientific-software/nsxtool.git

WORKDIR /tmp/nsxtool/build

RUN cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_QTAPPS=OFF -DBUILD_GSL=ON -DNSX_PYTHON3=ON -DCMAKE_INSTALL_PREFIX=./ .. \
 && cmake --build . --config Release \
 && cmake --build . --config Release --target install

RUN export NSX_ROOT_DIR=/tmp/nsxtool/build/share/nsxtool && ninja test

RUN cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_QTAPPS=ON -DBUILD_GSL=ON -DNSX_PYTHON3=ON -DBUILD_PACKAGE=ON -DCMAKE_INSTALL_PREFIX=/usr .. \
 && cmake --build . --config Release --target package

# Move the generated package to /tmp and remove nsxtool sources
WORKDIR /tmp
RUN mv nsxtool/build/nsxtool*deb . \
 && rm -rf nsxtool

