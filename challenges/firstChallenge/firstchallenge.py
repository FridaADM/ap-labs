def findLongestSubstring(string):
    strLen = len(string)
    first = 0
    startIndex = 0
    chars = {}
    chars[string[0]] = 0
    maxLen = 0
    for i in range(1, strLen):
        if string[i] not in chars:
            chars[string[i]] = i
        else: 
            if chars[string[i]] >= first:
                currLen = i - first
                if maxLen < currLen:
                    maxLen = currLen
                    startIndex = first
                first = chars[string[i]] + 1
            chars[string[i]] = i
    if maxLen < i - first:
        maxLen = i - first
        startIndex = first
    print(maxLen)
    return string[startIndex : startIndex + maxLen]

if __name__ == "__main__":  
    string = input("Type the string:")
    print(findLongestSubstring(string))  

