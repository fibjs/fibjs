docker build --rm -f android-build-env.amd64 -t fibjs/android-build-env:amd64 .
docker build --rm -f android-build-env.arm64 -t fibjs/android-build-env:arm64 .

docker build --rm -f linux-build-env.amd64 -t fibjs/linux-build-env:amd64 .
docker build --rm -f linux-build-env.i386 -t fibjs/linux-build-env:i386 .
docker build --rm -f linux-build-env.arm64 -t fibjs/linux-build-env:arm64 .
docker build --rm -f linux-build-env.arm -t fibjs/linux-build-env:arm .
docker build --rm -f linux-build-env.mips64 -t fibjs/linux-build-env:mips64 .
docker build --rm -f linux-build-env.ppc64 -t fibjs/linux-build-env:ppc64 .
docker build --rm -f linux-build-env.loong64 -t fibjs/linux-build-env:loong64 .

docker build --rm -f alpine-build-env.amd64 -t fibjs/alpine-build-env:amd64 .
docker build --rm -f alpine-build-env.arm64 -t fibjs/alpine-build-env:arm64 .
docker build --rm -f alpine-build-env.ppc64 -t fibjs/alpine-build-env:ppc64 .

docker build --rm -f alpine-test-env.arm64 -t fibjs/alpine-test-env:arm64 .
docker build --rm -f alpine-test-env.amd64 -t fibjs/alpine-test-env:amd64 .
docker build --rm -f alpine-test-env.ppc64 -t fibjs/alpine-test-env:ppc64 .
