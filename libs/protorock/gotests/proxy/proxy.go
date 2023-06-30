package main

import (
	"errors"
	"fmt"
	"net"
	"sync"

	"github.com/sandertv/gophertunnel/minecraft"
	"github.com/sandertv/gophertunnel/minecraft/auth"
	"github.com/sandertv/gophertunnel/minecraft/protocol/packet"
	"github.com/sirupsen/logrus"
	"golang.org/x/oauth2"
)

var log = logrus.New()

// The following program implements a proxy that forwards players from one local address to a remote address.
func main() {
	log.Info("Loading Xbox Token\n")
	tkn, err := LoadToken()
	if err != nil {
		tkn, err = auth.RequestLiveToken()
		if err != nil {
			log.Fatalf("error getting token: %s\n", err)
		}
		err = SaveToken(tkn)
		if err != nil {
			log.Fatalf("error saving token: %s\n", err)
		}
	}
	src := auth.RefreshTokenSource(tkn)

	// p, err := minecraft.NewForeignStatusProvider("10.10.5.43:19132")
	// if err != nil {
	// 	panic(err)
	// }
	listener, err := minecraft.ListenConfig{
		// StatusProvider: p,
		PacketFunc: func(header packet.Header, payload []byte, src, dst net.Addr) {
			log.Warnf("Sending packet %d with %d bytes: %v -> %v", header.PacketID, len(payload), src, dst)
		},
	}.Listen("raknet", "0.0.0.0:19000")
	if err != nil {
		panic(err)
	}
	defer listener.Close()
	for {
		c, err := listener.Accept()
		if err != nil {
			panic(err)
		}
		go handleConn(c.(*minecraft.Conn), listener, src)
	}
}

// handleConn handles a new incoming minecraft.Conn from the minecraft.Listener passed.
func handleConn(conn *minecraft.Conn, listener *minecraft.Listener, src oauth2.TokenSource) {
	fmt.Println("New connection")
	serverConn, err := minecraft.Dialer{
		TokenSource: src,
		ClientData:  conn.ClientData(),
		// PacketFunc: func(header packet.Header, payload []byte, src, dst net.Addr) {
		// 	log.Infof("Sending packet %d with %d bytes: %v -> %v", header.PacketID, len(payload), src, dst)
		// },
	}.Dial("raknet", "10.10.5.43:19132")
	if err != nil {
		panic(err)
	}
	fmt.Println("Upstream connected")
	var g sync.WaitGroup
	g.Add(2)
	go func() {
		fmt.Println("Sending Start Game")
		if err := conn.StartGame(serverConn.GameData()); err != nil {
			panic(err)
		}
		g.Done()
	}()
	go func() {
		fmt.Println("Sending Spawn")
		if err := serverConn.DoSpawn(); err != nil {
			panic(err)
		}
		g.Done()
	}()
	g.Wait()

	go func() {
		fmt.Println("Game flow")
		defer listener.Disconnect(conn, "connection lost")
		defer serverConn.Close()
		for {
			pk, err := conn.ReadPacket()
			if err != nil {
				return
			}
			if err := serverConn.WritePacket(pk); err != nil {
				if disconnect, ok := errors.Unwrap(err).(minecraft.DisconnectError); ok {
					_ = listener.Disconnect(conn, disconnect.Error())
				}
				return
			}
		}
	}()
	go func() {
		defer serverConn.Close()
		defer listener.Disconnect(conn, "connection lost")
		for {
			pk, err := serverConn.ReadPacket()
			if err != nil {
				if disconnect, ok := errors.Unwrap(err).(minecraft.DisconnectError); ok {
					_ = listener.Disconnect(conn, disconnect.Error())
				}
				return
			}
			if err := conn.WritePacket(pk); err != nil {
				return
			}
		}
	}()
}
