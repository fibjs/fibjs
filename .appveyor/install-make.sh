# resolve https://sourceforge.net/projects/ezwinports/files/make-4.3-without-guile-w32-bin.zip/download,
# we got https://netix.dl.sourceforge.net/project/ezwinports/make-4.3-without-guile-w32-bin.zip

CUR_DIR=`pwd`

cd "C:/Program Files/Git/mingw64/"
if [ ! -e make.zip ]; then
    curl -sL -o "make.zip" https://netix.dl.sourceforge.net/project/ezwinports/make-4.3-without-guile-w32-bin.zip
fi

unzip -n "make.zip"

cd $CUR_DIR;