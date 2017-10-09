#!/bin/bash

docker build --force-rm -t setup-$1 -f $1/Dockerfile .

