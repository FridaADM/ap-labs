package main

import (
	"fmt"
	"os"
	"strings"
	"path/filepath"
)

// scanDir stands for the directory scanning implementation
func scanDir(dir string) error {
	dirInfo := [5]int{0, 0, 0, 0, 0} //directories, symbolic links, devices, sockets, and oher files
	err := filepath.Walk(dir, func(dir string, fi os.FileInfo, err error) error{
		fi, err = os.Lstat(dir)
		dirInfo[fileType(fi)] += 1 
		return err
	})
	dashes := strings.Repeat("-", len(dir)+2)
	fmt.Println("\nDirectory Scanner Tool")
	fmt.Println("+-------------------------+" + dashes + "+")
	fmt.Println("| Path                    |", dir + "  |")
	fmt.Println("+-------------------------+" + dashes + "+")
	fmt.Println("| Directories             | ", dirInfo[0], "  |")
	fmt.Println("| Symbolic Links          | ", dirInfo[1], "  |")
	fmt.Println("| Devices                 | ", dirInfo[2], "  |")
	fmt.Println("| Sockets                 | ", dirInfo[3], "  |")
	fmt.Println("| Other files             | ", dirInfo[4], "  |")
	fmt.Println("+-------------------------+" + dashes + "+")
	return err
 }

func fileType(fi os.FileInfo) int{
	if fi.IsDir(){ return 0 } //test case
	mode:=fi.Mode()
	if (mode & os.ModeSymlink != 0) { return 1 }
	if (mode & os.ModeDevice != 0) { return 2 }
	if (mode & os.ModeSocket != 0){ return 3 }
	return 4
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: ./dir-scan <directory>")
		os.Exit(1)
	}
	scanDir(os.Args[1])
}
