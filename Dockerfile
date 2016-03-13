FROM takawang/ubuntu:vl
MAINTAINER Taka Wang <taka@cmwang.net>
 
## Set environment variable
ENV DEBIAN_FRONTEND noninteractive
RUN rm /bin/sh && ln -s /bin/bash /bin/sh

ENV ZMQ_VER 3.2.5
ENV ZMQ_SRC https://github.com/zeromq/zeromq3-x/releases/download/v$ZMQ_VER/zeromq-$ZMQ_VER.tar.gz
ENV CZMQ_GIT git://github.com/zeromq/czmq.git

## Install required development packages
RUN apt-get update
RUN apt-get install -y git wget build-essential autoconf pkg-config libssl-dev libwrap0-dev libc-ares-dev uuid-dev automake libtool cmake

## Install libzmq
WORKDIR  /
RUN wget $ZMQ_SRC \
    && tar xvzf zeromq-$ZMQ_VER.tar.gz \
    && cd zeromq-$ZMQ_VER \
    && ./configure \
    && make && make install && ldconfig \
    && cd .. && rm -rf zeromq*

## Install CZMQ
WORKDIR  /
RUN git clone $CZMQ_GIT \
    && cd czmq \
    && ./autogen.sh \
    && ./configure \
    && make && make install && ldconfig \
    && cd .. && rm -rf czmq*

## Install librs232
WORKDIR  /
RUN git clone https://github.com/taka-wang/librs232.git \
    && cd librs232 \
    && ./autogen.sh \
    && ./configure --prefix=/usr \
    && make && make install && ldconfig \
    && cd .. && rm -rf librs232

## Build dds-robot-arm
COPY . /dds-robot-arm/
RUN mkdir -p /dds-robot-arm/build \
    && cd /dds-robot-arm/build \
    && cmake .. && make \
    && make install
