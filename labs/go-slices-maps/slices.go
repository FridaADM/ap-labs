package main

import "golang.org/x/tour/pic"

func Pic(dx, dy int) [][]uint8 {
	a := make([][]uint8, dx)
    for x := range a {
        a[x] = make([]uint8, dy)
        for y := range a[x] {
        a[x][y] = uint8(x)
        }
    }
    return a
}

func main() {
	pic.Show(Pic)
}

