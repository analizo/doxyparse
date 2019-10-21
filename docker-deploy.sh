#!/bin/sh

# Represents the latest version of the project according to VERSION file
VERSION=$(cat VERSION)

DOCKERFILE_AMD64=$PWD/Dockerfile-amd64
DOCKERFILE_I386=$PWD/Dockerfile-i386

echo "Latest Doxyparse version is $VERSION";

# double validation in script and in .travis.yml
if [[ "${TRAVIS_BRANCH}" == "master" ]]; then
	echo "Deploying to Docker registry latest Doxyparse...";
	docker login -u "$DOCKER_USERNAME" -p "$DOCKER_PASSWORD";
	docker build -f $DOCKERFILE_AMD64 -t doxyparse:amd64-$VERSION .;
	docker build -f $DOCKERFILE_I386 -t doxyparse:i386-$VERSION .;
	docker tag doxyparse:amd64-$VERSION $DOCKER_USERNAME/doxyparse:amd64-$VERSION;
	docker tag doxyparse:i386-$VERSION $DOCKER_USERNAME/doxyparse:i386-$VERSION;
	docker push $DOCKER_USERNAME/doxyparse:amd64-$VERSION;
	docker push $DOCKER_USERNAME/doxyparse:i386-$VERSION;
else
	echo "Skipping Docker registry deploy";
fi;
