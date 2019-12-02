package main

import (
	"fmt"
	"io"
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
		c, err := listener.Accept()
		if err != nil {
			log.Fatalln(err)
		}
		go func() {
			defer c.Close()

			o, err := net.Dial("tcp", server_addr)
			if err != nil {
				log.Fatal(err)
			}
			defer o.Close()

			_, _, err = relay(c, o)
		}()
	}
}

func relay(left, right net.Conn) (int64, int64, error) {
	type res struct {
		N   int64
		Err error
	}
	ch := make(chan res)

	go func() {
		n, err := io.Copy(right, left)
		ch <- res{n, err}
	}()

	n, err := io.Copy(left, right)
	rs := <-ch

	if err == nil {
		err = rs.Err
	}
	return n, rs.N, err
}
