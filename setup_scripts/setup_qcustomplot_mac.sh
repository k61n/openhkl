#!/bin/bash

TMPBUILDDIR=${TMPDIR}/qcp
QMAKEBIN=/usr/local/opt/qt/bin/qmake

mkdir -p $TMPBUILDDIR
cd $TMPBUILDDIR

if ! wget https://www.qcustomplot.com/release/2.0.1/QCustomPlot-source.tar.gz; then
	echo "Cannot download QCustomPlot."
	exit -1
fi

if ! wget https://www.qcustomplot.com/release/2.0.1/QCustomPlot-sharedlib.tar.gz; then
	echo "Cannot download QCustomPlot-sharedlib."
	exit -1
fi

tar xzf QCustomPlot-source.tar.gz
tar xzf QCustomPlot-sharedlib.tar.gz

mv qcustomplot-sharedlib qcustomplot-source/

cd qcustomplot-source/qcustomplot-sharedlib/sharedlib-compilation
if ! ${QMAKEBIN} ; then
	echo "QMaking QCustomPlot failed."
	exit -1
fi
if ! make; then
	echo "Making QCustomPlot failed."
	exit -1
fi

install_name_tool -id "@rpath/libqcustomplot.dylib" libqcustomplot.dylib
install_name_tool -id "@rpath/libqcustomplotd.dylib" libqcustomplotd.dylib

cp ../../qcustomplot.h /usr/local/include/
cp *.dylib /usr/local/lib/
