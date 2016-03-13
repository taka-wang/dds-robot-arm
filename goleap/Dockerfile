FROM takawang/golang:x86
MAINTAINER Taka Wang <taka@cmwang.net>

## Install required development packages
RUN apt-get update
RUN apt-get install -y pkg-config


WORKDIR /go
RUN go get github.com/takawang/gomotion && go get github.com/takawang/zmq3

## Load app files
ADD . /go

## Default command
CMD go build