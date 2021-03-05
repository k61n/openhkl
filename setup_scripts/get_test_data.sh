#!/bin/bash

OUT_DIR=test/data

declare -a DATA_FILES=(
	"https://computing.mlz-garching.de/download/testdata/nsx/FutA.nsx"
	"https://computing.mlz-garching.de/download/testdata/nsx/FutA-indexed.nsx"
	"https://computing.mlz-garching.de/download/testdata/nsx/p16106_00029299.tiff"
	"https://computing.mlz-garching.de/download/testdata/nsx/p16106_00029300.tiff"
	"https://computing.mlz-garching.de/download/testdata/nsx/p16106_00029301.tiff"
	"https://computing.mlz-garching.de/download/testdata/nsx/p16106_00029302.tiff"
	"https://computing.mlz-garching.de/download/testdata/nsx/p16106_00029303.tiff"
	"https://computing.mlz-garching.de/download/testdata/nsx/p16106_00029304.tiff"
	"https://computing.mlz-garching.de/download/testdata/nsx/p16106_00029305.tiff"
)


mkdir -p ${OUT_DIR}

for data_file in ${DATA_FILES[@]}; do
	data_file_local=${data_file##*[/\\]}

	echo -e "Downloading \"${data_file_local}\"..."
	wget -O "${OUT_DIR}/${data_file_local}" "${data_file}"
done
