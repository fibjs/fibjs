if [[ ! -z $USE_VENDER_DIST ]]; then
    ./scripts/download_vender.sh;

    libs="exif
exlib
expat
gd
gtest
gumbo
jpeg
jssdk
leveldb
mbedtls
mongo
msgpack
pcre
png
snappy
sqlite
tiff
umysql
unzip
uuid
v8
webp
winiconv
zlib"

    for lib in ${libs}
    do
        if [ -e bin/Windows_amd64_release/${lib}.lib ]; then
            mv bin/Windows_amd64_release/${lib}.lib bin/Windows_amd64_release/lib${lib}.lib;
        fi
    done
fi