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
	python-numpy \
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
	opencv && \
    yes | pacman -Scc
