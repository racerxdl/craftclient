package main

import (
	"github.com/sandertv/go-raknet"
)

func main() {
	conn, _ := raknet.Dial("127.0.0.1:19132")
	defer conn.Close()

	b := make([]byte, 1024*1024*4)
	_, _ = conn.Write([]byte{1, 2, 3})
	_, _ = conn.Read(b)
}
