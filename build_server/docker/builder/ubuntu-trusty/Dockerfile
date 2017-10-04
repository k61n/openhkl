FROM eurydice76/nsxtool:setup_trusty

WORKDIR /tmp

RUN GIT_SSL_NO_VERIFY=true git clone -b develop https://code.ill.fr/scientific-software/nsxtool.git

WORKDIR /tmp/nsxtool/build

RUN cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_QTAPPS=ON -DCMAKE_INSTALL_PREFIX=./ -DBUILD_GSL=ON -DNSX_PYTHON3=ON ..
RUN cmake --build . --config Release
RUN cmake --build . --config Release --target install

RUN export NSX_ROOT_DIR=/tmp/nsxtool/build/share/nsxtool

RUN ninja test

RUN cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_QTAPPS=ON -DBUILD_DEBIAN=ON -DCMAKE_INSTALL_PREFIX=/usr -DNSX_PYTHON3=ON ..

RUN cmake --build . --config Release --target package

RUN mv nsxtool*deb /tmp

WORKDIR /tmp

RUN rm -rf nsxtool

