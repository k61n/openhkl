#!/bin/bash
#  ***********************************************************************************************
#
#  NSXTool: data reduction for neutron single-crystal diffraction
#
#! @file      testplot.sh
#! @brief     Test data files
#!
#! @homepage  https://openhkl.org
#! @license   GNU General Public License v3 or higher (see COPYING)
#! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
#! @authors   see CITATION, MAINTAINER
#! @module-author tweber@ill.fr
#
#  ***********************************************************************************************

file=$1
scannr=$2


function extract_data()
{
	local datanr=$1
	echo "Extracting image ${datanr}..."

	# read data
	data=$(awk \
		"/^IIII*$/ { ++block; inside=1; line=1; }; \
		/^SSSS*$/ { inside=0; } \
		{ if(block==${datanr}+1 && inside && line>2) \
			{ print; } \
		++line; \
		}" \
		D9_QSCAN)

	data1d=$(echo -e ${data})
	data1d="${data1d//[ \t]/\n}"

	echo -e "plot \"-\" w points pt 7\n${data1d}\ne\n" > /tmp/${datanr}.gpl
}


if [[ ! -f $file ]]; then
	echo "Please specify a valid file name."
	exit -1
fi


# find number of images
num=$(awk "/^IIII*$/ { ++line } END { print line-1 }" D9_QSCAN)
echo "$num images in file ${file}."


# if no data set number is given, plot them all
if [[ $scannr -eq "" ]]; then
	for((i=1; i<=${num}; ++i)); do
		extract_data ${i}
		gnuplot -p /tmp/${i}.gpl
	done

# plot only one data set
else
	extract_data ${scannr}
	gnuplot -p /tmp/${scannr}.gpl
fi
