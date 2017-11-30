#!/bin/bash

docker build --force-rm -t docker.ill.fr/scientific-software/nsxtool:setup-$1 -f $1/Dockerfile .

