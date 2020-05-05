// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"flag"
	"strings"
	"time"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

type user struct{
	canal client
	name string
	add string
	adm bool
	connection net.Conn
}

var users=[]user{}

var (
	entering = make(chan user)
	leaving  = make(chan user)
	messages = make(chan string) // all incoming client messages
	admin = false
	banned = make(chan user)
)

func broadcaster() {
	clients := make(map[client]bool) // all connected clients
	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}

		case cli := <-entering:
			
			clients[cli.canal] = true

		case cli := <-leaving:
			delete(clients, cli.canal)
			close(cli.canal)
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)

	ircServer := "irc-server > "
	userCount := 0
	who := conn.RemoteAddr().String()
	u := user{}
	input := bufio.NewScanner(conn)
	for input.Scan() {
		if(userCount == 0){
			who = input.Text()
			ch <- ircServer + "Your user :" + who + " is successfully logged in!"
			messages <- "New connected user" + who	

			u.canal, u.name, u.add, u.adm, u.connection = ch, who, conn.RemoteAddr().String(), false, conn

			fmt.Println(ircServer + "New connected user ", u.name)
			if(!admin){
				fmt.Println(ircServer + u.name + " was promoted as ADMIN")
				u.adm = true
				u.canal <- ircServer + "Congrats, you were the first user"
				u.canal <- ircServer + "You are the new IRC Server Admin"
				admin = true
				
			}
			users = append(users, u)
			entering <- u
			userCount =- 1
		}else{
			mes := input.Text()
			command := strings.Split(mes," ")
			switch command[0]{

			case "/user":
				if(len(command) == 2){
					us := command[1]
					response := "Error, no such user"
					for user := range users{
						if(users[user].name == us){
							response = getUserinfo(users[user])
						}
					}
					u.canal <- ircServer + response
				}else{
					u.canal <- ircServer + "Error, no such user"
				}
				break
			
			case "/time":
				u.canal <- ircServer + getTime()
				break

			case "/users":
				list := ""
				for x := range users{
					list += (users[x].name + ", ")
				}
				u.canal <- ircServer + list[0:len(list)-2]
				break
			
			case "/kick":
				if(u.adm){
					if(len(command) >= 2){
						us := command[1]
						f := false
						for user := range users{
							if(users[user].name == us){
								kicked := users[user]
								kicked.canal <- ircServer + "You are kicked from this channel"
								kicked.canal <- ircServer + "Bad language is not allowed in this channel"
								fmt.Println(ircServer + kicked.name + " was kicked")
								users[user].connection.Close()
								banned <- users[user]
								messages <- ircServer + us + " was kicked from channel for bad language policy violation"
								f = true
								break
							}
						}
						if(!f){
							u.canal <- ircServer + "Error, no such user"
						}
					}else{u.canal <- ircServer + "Error, no such user"}
				}else{u.canal<-"Error, you are not the admin."}
				break

			case "/msg":
				if(len(command) >= 2){
					us := command[1]
					msg := strings.Join(command[2:len(command)]," ")
					f := false
					for user := range users{
						if(users[user].name == us){
							users[user].canal <- ircServer + u.name + " direct:"+ msg
							f = true
							break
						}
					}
					if(!f){
						u.canal <- ircServer + "Error, no such user"
					}
				}else{u.canal <- ircServer + "Error, no such user"}
				break

			default:
				messages <- who + ": " + mes
				break
			}
		}
	}


	select{
		case <- banned:
			leaving <- u
			conn.Close()
		default:
			leaving <- u
			messages <- who + " has left"
			fmt.Println(ircServer + who + " left")
			conn.Close()
	}
	
}
func getUserinfo(u user) string{
	return "username: "+u.name+" IP: "+ u.add

}
func getTime() string {
	return "Local Time: America/Mexico_City " + time.Now().Format("15:04")
}
func clientWriter(conn net.Conn, ch <- chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) 
	}
}
//!-handleConn

//!+main
func main() {
	direction := flag.String("host","localhost","an address")
	port := flag.String("port","8000","a port number")
	flag.Parse()
	add := strings.Join([]string{*direction,*port}, ":")

	listener, err := net.Listen("tcp", add)
	if err != nil {
		log.Fatal(err)
	}

	name := "irc-server >"
	fmt.Println(name,"Simple IRC Server started at", add)
	fmt.Println(name,"Ready for receiving new clients")

	go broadcaster()
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main
