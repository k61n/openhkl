#!/bin/bash

# build the nsxtool image
docker build --force-rm --build-arg NSX_GIT_BRANCH=$1 -t nsxtool -f $2/Dockerfile .

# run the nsxtool container
id=`docker run -d -t nsxtool /bin/bash`

# cp the build artefact from the container to host
docker cp $id:/tmp/build-artefacts.tar.gz $2/.

# kill the nsxtool daemon container
docker kill $id

# remove the nsxtool container
docker rm $id

# remove nsxtool image and associated dangling images
docker rmi nsxtool

