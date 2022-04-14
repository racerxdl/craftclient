package main

import (
	"fmt"
	"time"

	"github.com/sandertv/go-raknet"
)

func main() {
	listener, _ := raknet.Listen("0.0.0.0:19132")
	defer listener.Close()
	fmt.Println("Waiting for connections")
	for {
		conn, _ := listener.Accept()
		fmt.Println("New connection", conn)

		_, _ = conn.Write([]byte("\xfeLucas Teske"))
		time.Sleep(time.Second)
		_, _ = conn.Write([]byte("\xfeSecond packet"))
		time.Sleep(time.Second)

		conn.Close()
	}
}
