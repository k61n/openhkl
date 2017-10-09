<<<<<<< HEAD
FROM eurydice76/nsxtool:setup-ubuntu-trusty
=======
FROM eurydice76/nsxtool:setup_trusty
>>>>>>> f964ba3e887facf634dd2227b3091785a1ad9448

WORKDIR /tmp

RUN GIT_SSL_NO_VERIFY=true git clone -b develop https://code.ill.fr/scientific-software/nsxtool.git

WORKDIR /tmp/nsxtool/build

<<<<<<< HEAD
RUN cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_QTAPPS=ON -DBUILD_GSL=ON -DNSX_PYTHON3=ON -DCMAKE_INSTALL_PREFIX=./ ..
=======
RUN cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_QTAPPS=ON -DCMAKE_INSTALL_PREFIX=./ -DBUILD_GSL=ON -DNSX_PYTHON3=ON ..
>>>>>>> f964ba3e887facf634dd2227b3091785a1ad9448
RUN cmake --build . --config Release
RUN cmake --build . --config Release --target install

RUN export NSX_ROOT_DIR=/tmp/nsxtool/build/share/nsxtool

RUN ninja test

<<<<<<< HEAD
RUN cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_QTAPPS=ON -DBUILD_GSL=ON -DNSX_PYTHON3=ON -DBUILD_DEBIAN=ON -DCMAKE_INSTALL_PREFIX=/usr ..
=======
RUN cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_QTAPPS=ON -DBUILD_DEBIAN=ON -DCMAKE_INSTALL_PREFIX=/usr -DNSX_PYTHON3=ON ..
>>>>>>> f964ba3e887facf634dd2227b3091785a1ad9448

RUN cmake --build . --config Release --target package

RUN mv nsxtool*deb /tmp

WORKDIR /tmp

RUN rm -rf nsxtool

