FROM docker.ill.fr/scientific-software/nsxtool:setup-ubuntu-zesty

ARG NSX_GIT_BRANCH=develop

WORKDIR /tmp/

RUN GIT_SSL_NO_VERIFY=true git clone -b ${NSX_GIT_BRANCH} https://code.ill.fr/scientific-software/nsxtool.git

WORKDIR /tmp/nsxtool/build

RUN cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_NSX_APPS=ON -DUSE_PYTHON3=ON -DCMAKE_INSTALL_PREFIX=./ .. \
 && cmake --build . --config Release \
 && cmake --build . --config Release --target install

RUN export NSX_ROOT_DIR=/tmp/nsxtool/build/share/nsxtool && ninja test

RUN cmake -G "Ninja" -DBUILD_PACKAGE=ON -DCMAKE_INSTALL_PREFIX=/usr .. && cmake --build . --config Release --target package

# Move the generated package to /tmp and remove nsxtool sources
WORKDIR /tmp
RUN mv nsxtool/build/nsxtool*deb . \
 && tar cvzf build-artefacts.tar.gz *.deb \
 && rm -rf nsxtool

