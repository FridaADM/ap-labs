#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int longestSubstring(char *str){
    int helper[128];
    int maxLen = 0;
    int len = 0;
    int i = 0;

    memset(helper, 0xff, sizeof(helper));
    
    while (*str != '\0') {
        if (helper[*str] == -1) {
            len++;
        } 
		else {
            if (i - helper[*str] > len){
                len++;
            } 
			else {
	            len = i - helper[*str];
            }
        }
        
        if (len > maxLen){
            maxLen = len;
        }
        helper[*str++] = i++;
    }
    
    return maxLen;
}

int main(void){
    char *str = "abcabcbb";
	printf("\nInput: %s", str);
	printf("\nLength of the longest substring without repeating characters: %d\n", longestSubstring(str));
	return 0;
}
