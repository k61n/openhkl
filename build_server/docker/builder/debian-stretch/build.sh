#!/bin/bash

# build the nsxtool image
docker build --force-rm -t nsxtool .

# run the nsxtool container
id=`docker run -d -t nsxtool`

# cp the build artefact from the container to host
docker cp $id:/tmp/nsxtool_1.0.0-1_amd64.deb .

# kill the nsxtool daemon container
docker kill $id

# remove the nsxtool container
docker rm $id

# remove nsxtool image and associated dangling images
docker rmi nsxtool

