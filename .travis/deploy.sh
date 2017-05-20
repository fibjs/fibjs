#!/bin/bash

set -ev

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
  brew install https://raw.githubusercontent.com/kadwanev/bigboybrew/master/Library/Formula/sshpass.rb;
fi

cd ${TRAVIS_TAG}
sshpass -p $DEPLOY_PASS scp -r ./ $DEPLOY_USER@$DEPLOY_HOST:$DEPLOY_PATH/$TRAVIS_TAG

exit 0;
