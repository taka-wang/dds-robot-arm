FROM takawang/gozmq:x86
MAINTAINER Taka Wang <taka@cmwang.net>


WORKDIR /go
RUN go get github.com/takawang/gomotion && go get github.com/takawang/zmq3

## Load app files
ADD . /go

## Default command
CMD go build