package main

import (
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/x509"
	"fmt"
)

func main() {
	key, _ := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	data, _ := x509.MarshalPKIXPublicKey(&key.PublicKey)
	fmt.Println(string(data))
}
