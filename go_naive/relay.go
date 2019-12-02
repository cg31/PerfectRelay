package main

import (
	"fmt"
	"log"
	"net"
)

var listen_addr = "127.0.0.1:1090"
var server_addr = "127.0.0.1:5201"

func main() {
	listener, err := net.Listen("tcp", listen_addr)
	if err != nil {
		log.Fatal(err)
	}
	defer listener.Close()

	fmt.Println("Listening on: ", listen_addr)
	fmt.Println("Proxying to: ", server_addr)
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Fatalln(err)
		}

		go func() {
			defer conn.Close()

			outbound, err := net.Dial("tcp", server_addr)
			if err != nil {
				log.Fatal(err)
			}
			defer outbound.Close()

			relay(conn, outbound)
		}()
	}
}

func relay(inbound, outbound net.Conn) {

	buf := make([]byte, 130*1024)
	for {
		_, err := inbound.Read(buf[:])
		if err != nil {
			log.Fatalln(err)
		}

		_, err = outbound.Write(buf[:])
		if err != nil {
			log.Fatalln(err)
		}
	}
}
