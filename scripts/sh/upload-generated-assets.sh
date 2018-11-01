#!/bin/bash

# Check dependencies.
set -e
xargs=$(which gxargs || which xargs)

# Validate settings.
[ "$TRACE" ] && set -x

ARCH=$(uname -m)
if [[ "$ARCH" == "x86_64" ]]; then
  ARCH=amd64
else if [[ "$ARCH" == "i686" ]]; then 
  ARCH=i386
else
  echo "Unsupported $ARCH architecture!"; 
fi;
fi;

# Define variables.
VERSION=$(cat ../../VERSION)
TOKEN_SHA=$(cat ../../auth-token)
CURL_ARGS="-LJO#"
WGET_ARGS="--content-disposition --auth-no-challenge --no-cookie"
REPO_FULL_NAME=$(git config --get remote.origin.url | sed 's/.*:\/\/github.com\///;s/.git$//')
LATEST_TAG=$(curl --silent "https://api.github.com/repos/$REPO_FULL_NAME/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/')
REPO_URL=https://api.github.com/repos/$REPO_FULL_NAME
AUTH="Authorization: token $TOKEN_SHA"
REPO_RELEASES=$REPO_URL/releases
REPO_TAGS="$REPO_RELEASES/tags/$LATEST_TAG"
RESPONSE=$(curl -sH "$AUTH" $REPO_TAGS)
FILENAME=./doxyparse_$VERSION-$ARCH.tar.gz

# Validate token.
curl -o /dev/null -sH "$AUTH" $REPO_URL || { echo "Error: Invalid repo, token or network issue!";  exit 1; }

echo $RESPONSE
# Read asset tags.
# Get ID of the release.
eval $(echo "$RESPONSE" | grep -m 1 "id.:" | grep -w id | tr : = | tr -cd '[[:alnum:]]=')
[ "$ID" ] || { echo "Error: Failed to get release id for tag: $LATEST_TAG"; echo "$RESPONSE" | awk 'length($0)<100' >&2; exit 1; }

RELEASE_ID="$ID"
REPO_ASSET="https://uploads.github.com/repos/$REPO_FULL_NAME/releases/$RELEASE_ID/assets?name=$(basename $FILENAME)"

# Get ID of the asset based on given filename.
ID=""
eval $(echo "$RESPONSE" | grep -C1 "name.:.\+$FILENAME" | grep -m 1 "id.:" | grep -w id | tr : = | tr -cd '[[:alnum:]]=')
ASSET_ID="$ID"

if [ "$ASSET_ID" = "" ]; then
    echo "No need to overwrite asset"
else
    echo "Deleting asset($ASSET_ID)... "
    curl "$GITHUB_OAUTH_BASIC" -X "DELETE" -H "Authorization: token $TOKEN_SHA" "https://api.github.com/repos/$REPO_FULL_NAME/releases/assets/$ASSET_ID"
fi

# Upload asset
echo "Uploading asset... "

# Construct url
curl "$GITHUB_OAUTH_BASIC" --data-binary @"$FILENAME" -H "Authorization: token $TOKEN_SHA" -H "Content-Type: application/octet-stream" $REPO_ASSET
