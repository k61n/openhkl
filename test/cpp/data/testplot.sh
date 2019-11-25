#!/bin/bash
#  ***********************************************************************************************
#
#  NSXTool: data reduction for neutron single-crystal diffraction
#
#! @file      testplot.sh
#! @brief     Test data files
#!
#! @homepage  ###HOMEPAGE###
#! @license   GNU General Public License v3 or higher (see COPYING)
#! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
#! @authors   see CITATION, MAINTAINER
#! @module-author tweber@ill.fr
#
#  ***********************************************************************************************

file=$1
scannr=$2

if [[ ! -f $file ]]; then
	echo "Please specify a valid file name."
	exit -1
fi


# find number of images
num=$(awk "/^IIII*$/ { ++line } END { print line-1 }" D9_QSCAN)
echo "$num images in file ${file}. Showing image ${scannr}..."


# read data
data=$(awk \
	"/^IIII*$/ { ++block; inside=1; line=1; }; \
	/^SSSS*$/ { inside=0; } \
	{ if(block==${scannr}+1 && inside && line>2) \
		{ print; } \
	++line; \
	}" \
	D9_QSCAN)

echo -e "${data}"


# plot
plotargs=$(echo -e "plot \"-\" matrix with image\n${data}\ne")
gnuplot -p -e "${plotargs}"

