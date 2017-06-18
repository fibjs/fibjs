#!/bin/bash

set -ev

if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    if [[ $ARCH == "amd64" ]]; then
        ./bin/Linux_amd64_release/fibjs test/main.js
    else
        ./bin/Linux_i386_release/fibjs test/main.js
    fi
else # darwin
    if [[ $ARCH == "amd64" ]]; then
        ./bin/Darwin_amd64_release/fibjs test/main.js
    else
        ./bin/Darwin_i386_release/fibjs test/main.js
    fi
fi

exit 0;