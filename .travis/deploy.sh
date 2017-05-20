#!/bin/bash

set -ev

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
  brew install https://raw.githubusercontent.com/kadwanev/bigboybrew/master/Library/Formula/sshpass.rb;
fi

sshpass -p $DEPLOY_PASS scp -r ./${TRAVIS_TAG}/ $DEPLOY_USER@$DEPLOY_HOST:$DEPLOY_PATH/$TRAVIS_TAG

exit 0;
