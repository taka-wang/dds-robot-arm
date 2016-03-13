# goleap

A websocket client in golang to read leap motion sensor data concurrently.

![leapmotion](http://cdn.shopify.com/s/files/1/0365/6765/products/Packaging-front_3_4-web_large.png?v=1409245447)

#### Leap Motion API
- [Json format](https://developer.leapmotion.com/documentation/javascript/supplements/Leap_JSON.html#json-tracking-data-format
)
- [Pointable.stabilizedTipPosition](https://developer.leapmotion.com/documentation/javascript/api/Leap.Pointable.html#Pointable.stabilizedTipPosition)

#### Install golang 1.6 & zmq binding & gomotion

```bash
sudo apt-get install pkg-config

# depends on your platform
curl -O https://storage.googleapis.com/golang/go1.6.linux-amd64.tar.gz 
tar -xvf go1.6.linux-amd64.tar.gz
sudo mv go /usr/local

# set go bin to path
nano ~/.profile
export PATH=$PATH:/usr/local/go/bin

go get github.com/taka-wang/zmq3
go get github.com/taka-wang/gomotion
```