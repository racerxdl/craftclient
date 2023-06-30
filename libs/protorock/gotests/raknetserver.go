package main

import (
	"fmt"
	"time"

	"github.com/sandertv/go-raknet"
)

func main() {
	listener, _ := raknet.Listen("0.0.0.0:1234")
	defer listener.Close()
	fmt.Println("Waiting for connections")
	for {
		conn, _ := listener.Accept()
		fmt.Println("New connection", conn)

		_, _ = conn.Write([]byte("\xfeSmall Packet"))
		time.Sleep(time.Second)
		bigBuffer := make([]byte, 4096)
		for i := 0; i < 4096; i++ {
			bigBuffer[i] = uint8(i)
		}
		copy(bigBuffer, []byte("\xfeBIG PACKET"))
		_, _ = conn.Write([]byte(bigBuffer))
		_, _ = conn.Write([]byte(bigBuffer))
		_, _ = conn.Write([]byte(bigBuffer))
		time.Sleep(time.Second)

		conn.Close()
	}
}
