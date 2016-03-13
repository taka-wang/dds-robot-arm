package main

import (
	"fmt"
	"log"
	"os"
	"runtime"
	"strconv"

	"github.com/takawang/gomotion"
	zmq "github.com/takawang/zmq3"
)

var endpoint string = "ipc:///tmp/dummy"

func main() {

	if len(os.Args) > 1 {
		endpoint = os.Args[1]
	}
	fmt.Println("endpoint:", endpoint)

	// zmq pub
	sender, zerr := zmq.NewSocket(zmq.PUB)
	if zerr != nil {
		log.Fatal(zerr)
	}

	defer sender.Close()
	sender.Bind(endpoint)

	// Get a leap motion device.
	runtime.GOMAXPROCS(runtime.NumCPU())
	device, gerr := gomotion.GetDevice("ws://127.0.0.1:6437/v3.json")
	if gerr != nil {
		log.Fatal(gerr)
	}
	device.Listen()
	fmt.Println("start listening leap motion data..")
	defer device.Close()

	for frame := range device.Pipe {
		if frame.Frame != nil && len(frame.Frame.Pointables) > 0 {
			leapPoint := frame.Frame.Pointables[0].StabilizedTipPosition
			fmt.Println("sensor:", leapPoint[0:2])
			sender.Send(strconv.FormatFloat(float64(leapPoint[0]*2), 'e', -1, 32), zmq.SNDMORE)
			sender.Send(strconv.FormatFloat(float64(leapPoint[1]), 'e', -1, 32), 0)
		}
	}
}
