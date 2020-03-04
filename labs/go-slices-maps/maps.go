package main

import (
	"golang.org/x/tour/wc"
	"strings"
)

func WordCount(s string) map[string]int {
	mapWithWords := make( map[string]int)
	words := strings.Fields(s)
	
	for _,word := range words{
		mapWithWords[word]++
	}
	return mapWithWords
}

func main() {
	wc.Test(WordCount)
}

