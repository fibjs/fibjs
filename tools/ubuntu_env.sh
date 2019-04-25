
apt install clang -y
apt install g++ -y
apt install make -y
apt install cmake -y
apt install git -y
apt install ccache -y

apt install g++-multilib -y

apt install g++-arm-linux-gnueabihf -y

apt install g++-aarch64-linux-gnu -y

apt install g++-mips-linux-gnu -y

apt install g++-mips64-linux-gnuabi64 -y

apt install g++-powerpc-linux-gnu -y

apt install g++-powerpc64-linux-gnu -y

rm -f /usr/include/asm
rm -f /usr/include/i386-linux-gnu
rm -f /usr/include/x86_64-linux-gnux32

ln -s x86_64-linux-gnu /usr/include/i386-linux-gnu
ln -s x86_64-linux-gnu /usr/include/x86_64-linux-gnux32
