// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"io"
	"log"
	"net"
	"os"
	"flag"
)

//!+
func main() {
	user:=flag.String("user","Unnamed","your username")
	server:=flag.String("server","localhost:8000","<server>:<port>")
	flag.Parse()

	conn, err := net.Dial("tcp",*server)
	if err != nil {
		log.Fatal(err)
		os.Exit(-1)
	}
	
	io.WriteString(conn,*user+"\n")

	done := make(chan struct{})
	go func() {
		if _,err:=io.Copy(os.Stdout, conn); err!= nil{ 
			os.Exit(-1)
		}
		log.Println("done")
		done <- struct{}{} // signal the main goroutine
	}()
	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
