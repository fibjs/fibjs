docker build --rm -f alpine-build-env.arm64 -t fibjs/alpine-build-env:arm64 .
docker build --rm -f alpine-build-env.amd64 -t fibjs/alpine-build-env:amd64 .

docker build --rm -f alpine-test-env.amd64 -t fibjs/alpine-test-env:amd64 .
docker build --rm -f alpine-test-env.arm64 -t fibjs/alpine-test-env:arm64 .
