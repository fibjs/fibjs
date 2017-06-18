#!/bin/bash

set -ev

if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    if [[ $ARCH == "amd64" ]]; then
      cd test
      ../bin/Linux_amd64_release/fibjs main.js
    fi
else # darwin
    if [[ $ARCH == "amd64" ]]; then
        cd test
        ../bin/Darwin_amd64_release/fibjs main.js
    fi
fi

exit 0;