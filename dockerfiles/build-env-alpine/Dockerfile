FROM alpine:latest

LABEL AUTHOR="Richard"
LABEL AUTHOR_EMAIL="richardo2016@gmail.com"

RUN apk update && apk upgrade

RUN apk update
RUN apk add --no-cache clang clang-dev alpine-sdk xz dpkg ncurses && \
    apk add --update --no-cache cmake && \
    apk add --update --no-cache linux-headers

RUN ls -l /usr/bin/cc /usr/bin/c++ /usr/bin/clang /usr/bin/clang++ /usr/bin/tput

RUN ln -sf /usr/bin/clang /usr/bin/cc
RUN ln -sf /usr/bin/clang++ /usr/bin/c++

RUN update-alternatives --install /usr/bin/cc cc /usr/bin/clang 10
RUN update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 10

RUN update-alternatives --auto cc
RUN update-alternatives --auto c++

RUN update-alternatives --display cc
RUN update-alternatives --display c++

RUN ls -l /usr/bin/cc /usr/bin/c++ 

RUN cc --version
RUN c++ --version