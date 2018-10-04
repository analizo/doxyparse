#!/bin/sh

VERSION=$(cat VERSION)
TEXT=$2
BRANCH=$(git rev-parse --abbrev-ref HEAD)
REPO_FULL_NAME=$(git config --get remote.origin.url | sed 's/.*:\/\/github.com\///;s/.git$//')
TOKEN_SHA=$(cat auth-token)
TOKEN=$(git config --global github.token $TOKEN_SHA)

# Compiling
cmake -G "Unix Makefiles" -Dbuild_parse=ON
make
# make install -> this command is encapsulated by packing

# Port to build-package.sh
# Generating tarball
# tar cvf doxyparse-$VERSION.tar.gz *

# Generating debian package
# cd .. && dkpg-build doxyparse
# dpkg-buildpackage -rfakeroot -d -us -uc

# 1) Create a personal access token through GitHub UI, we only need "Repo" access
# 2) Place it in your config by "git config --global github.token YOUR_TOKEN"
# 3) Use bash script to make CURL request `sh git-release.sh "1.5.0" "Test description"`

generate_post_data()
{
  cat <<EOF
{
  "tag_name": "$VERSION",
  "target_commitish": "$BRANCH",
  "name": "$VERSION",
  "body": "$TEXT",
  "draft": false,
  "prerelease": false
}
EOF
}

echo "Create release $VERSION for repo: $REPO_FULL_NAME branch: $BRANCH"
curl --data "$(generate_post_data)" "https://api.github.com/repos/$REPO_FULL_NAME/releases?access_token=$TOKEN"

# Deploying to analizo.org
# TODO: Refactor this
git remote add analizo-org https://github.com/GCES2018-2/analizo.github.io.git
git config --global push.default simple
git push analizo-org testing-deploy
