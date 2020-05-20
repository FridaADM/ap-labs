package main

import (
    "time"
    "fmt"
    "strconv"
)

var i int
var seconds int

func main() {
    ch1 := make(chan struct{})
    ch2 := make(chan struct{})
    ch3 := make(chan struct{})

    go func() {
        ticker := time.NewTicker(1 * time.Second)
        i = 0
        seconds := 0
    loop:
        for {
            ch1 <- struct{}{}
            select {
            case <-ch2:
                i++
            case <-ticker.C:
                seconds++
                <-ch2
                i++
                fmt.Printf("\rAverage communications per second: %d: %d", seconds, 2*i/seconds)
                if seconds >= 100 {
                    ticker.Stop()
                    break loop
                }
            }
        }

        ch3 <- struct{}{}
    }()

    go func() {
        for {
            <-ch1
            ch2 <- struct{}{}
        }
    }()
    <-ch3
    fmt.Println()


  str := "Total sent messages: "+strconv.Itoa(i)
  fmt.Println(str)
  
}
