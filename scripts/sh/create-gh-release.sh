#!/bin/sh

VERSION=$(cat ../../VERSION)
REPO_FULL_NAME=$(git config --get remote.origin.url | sed 's/.*:\/\/github.com\///;s/.git$//')
TEXT="Deploying version $VERSION of $REPO_FULL_NAME"
TOKEN_SHA=$(cat ../../auth-token)
REPO_URL=https://api.github.com/repos/$REPO_FULL_NAME/releases?access_token=$TOKEN_SHA
BRANCH=$(git rev-parse --abbrev-ref HEAD)

# Sample JSON pattern accepted by GitHub API
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

# Posting Sample JSON to GitHub API using required token and credentials
echo "Creating Release v$VERSION for $REPO_FULL_NAME"
curl --data "$(generate_post_data)" $REPO_URL
