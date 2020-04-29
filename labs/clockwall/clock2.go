// Clock2 is a concurrent TCP server that periodically writes the time.
package main

import (
	"io"
	"log"
	"net"
	"os"
	"strings"
	"time"
)

func handleConn(c net.Conn) {
	defer c.Close()

	timezone := os.Getenv("TZ")
	location, err := time.LoadLocation(timezone)
	if err != nil {
		log.Fatal(err)
	}

	for {
		_, err := io.WriteString(c, timezone + strings.Repeat(" ", 10 - len(timezone)) + " : " + time.Now().In(location).Format("15:04:05\n"))
		if err != nil {
			return // e.g., client disconnected
		}
		time.Sleep(1 * time.Second)
	}
}

func main() {
	if len(os.Args) < 2 {
		log.Fatalf("How to execute: TZ=[timezone] go run clock2.go -port [port]")
	}

	port := os.Args[2]
	server := "localhost:" + port

	listener, err := net.Listen("tcp", server)
	if err != nil {
		log.Fatal(err)
	}
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err) // e.g., connection aborted
			continue
		}
		go handleConn(conn) // handle connections concurrently
	}
}
