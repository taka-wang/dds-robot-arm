# mqtt-robot-arm

Control robotic arm with leap motion via MQTT


#### System update

```bash    
sudo apt-get update
sudo apt-get install git wget libssl-dev libc-ares-dev libwebsockets build-essential libtool autoconf automake uuid-dev cmake
```

### Install mosquitto

```bash
wget http://mosquitto.org/files/source/mosquitto-1.4.8.tar.gz
tar zxvf mosquitto-1.4.8.tar.gz
cd mosquitto-1.4.8
make all
sudo make install
sudo ldconfig
sudo useradd -r -m -d /var/lib/mosquitto -s /usr/sbin/nologin -g nogroup mosquitto
#mosquitto -c /etc/mosquitto/mosquitto.conf -d
```

### Install init script for mosquitto broker

```bash
# default mqtt port: 1883, websocket port: 9000
sudo cp mosquitto.conf /etc/mosquitto/mosquitto.conf
sudo mv mosquitto /etc/init.d/mosquitto
sudo chmod +x /etc/init.d/mosquitto
sudo update-rc.d mosquitto defaults
sudo service mosquitto status | stop | start | restart
```

### Test MQTT broker

```bash
mosquitto_sub -d -t hello/world
mosquitto_pub -d -t hello/world -m "Mosquitto Works"
```
