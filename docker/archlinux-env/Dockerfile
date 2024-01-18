FROM archlinux:base

RUN pacman -Syu --noconfirm && \
    pacman -Sy --noconfirm --needed \
        cmake \
        make \
        clang \
        gcc \
        git \
        git-lfs \
        mesa \
        qt5-base \
        hdf5 \
        python3 \
        python-pipx\
        python-numpy \
        python-setuptools \
        python-sphinx \
        python-sphinxcontrib-bibtex \
        python-breathe \
        swig \
        blosc \
        yaml-cpp \
        libtiff \
        fftw \
        gsl \
        eigen \
        qhull \
        vtk \
        glew \
        fmt \
        doxygen \
        graphviz \
        opencv && \
    yes | pacman -Scc
