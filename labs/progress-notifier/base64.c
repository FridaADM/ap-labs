#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "logger.h"

#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

struct stat st;
unsigned long size;
unsigned long writtenBytes;

static const unsigned char d[] = {
    66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize){
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const uint8_t *data = (const uint8_t *)data_buf;
   size_t resultIndex = 0;
   size_t x;
   uint32_t n = 0;
   int padCount = dataLength % 3;
   uint8_t n0, n1, n2, n3;

   for (x = 0; x < dataLength; x += 3) {
      n = ((uint32_t)data[x]) << 16;
      if((x+1) < dataLength)
         n += ((uint32_t)data[x+1]) << 8;
      if((x+2) < dataLength)
         n += data[x+2];
      
      n0 = (uint8_t)(n >> 18) & 63;
      n1 = (uint8_t)(n >> 12) & 63;
      n2 = (uint8_t)(n >> 6) & 63;
      n3 = (uint8_t)n & 63;
    
      if(resultIndex >= resultSize) return 1; 
      result[resultIndex++] = base64chars[n0];
      if(resultIndex >= resultSize) return 1;
      result[resultIndex++] = base64chars[n1];

      if((x+1) < dataLength){
         if(resultIndex >= resultSize) return 1;
         result[resultIndex++] = base64chars[n2];
      }

      
      if((x+2) < dataLength){
         if(resultIndex >= resultSize) return 1;
         result[resultIndex++] = base64chars[n3];
      }
   }

   
   if (padCount > 0) { 
      for (; padCount < 3; padCount++) { 
         if(resultIndex >= resultSize) return 1;  
         result[resultIndex++] = '=';
      } 
   }

   if(resultIndex >= resultSize) return 1;  
   result[resultIndex] = 0;
   return 0;
}

int base64decode (char *in, size_t inLen, unsigned char *out, size_t *outLen) { 
    char *end = in + inLen;
    char iter = 0;
    uint32_t buf = 0;
    size_t len = 0;
    
    while (in < end) {
        unsigned char c = d[*in++];
        
        switch (c) {
        case WHITESPACE: continue;   
        case INVALID:    return 1;   
        case EQUALS:
            in = end;
            continue;
        default:
            buf = buf << 6 | c;
            iter++;
            if (iter == 4) {
                if ((len += 3) > *outLen) return 1;
                *(out++) = (buf >> 16) & 255;
                *(out++) = (buf >> 8) & 255;
                *(out++) = buf & 255;
                buf = 0; iter = 0;

            }   
        }
    }
   
    if (iter == 3) {
        if ((len += 2) > *outLen) return 1;
        *(out++) = (buf >> 10) & 255;
        *(out++) = (buf >> 2) & 255;
    }
    else if (iter == 2) {
        if (++len > *outLen) return 1;
        *(out++) = (buf >> 4) & 255;
    }

    *outLen = len;
    return 0;
}

static void progress(int task) {
  unsigned long percentage = (writtenBytes * 100) / size;
    if(task == 0){
        infof("Encoding %d%% complete\n", percentage);
    }
    else if(task == 1){
        infof("Decoding %d%% complete\n", percentage);
    }
}

int main(int argc, char **argv){
    if(argc != 3){
        errorf("Binary should be executed as following [ ./base64 --<task> <file.txt> ] \n");
        exit(1);
    }

    if (signal(SIGINT, progress) == SIG_ERR)
    errorf("Unable to map signal to function progress.\n");

   if (strcmp(argv[1], "--encode") == 0) {
    int fd, fdwrite, buffreadsize, buffwritesize;
    char *buffread, *buffwrite;

    fd = open(argv[2], O_RDONLY);
    if (fd == -1) {
      errorf("Error opening file\n");
      close(fd);
      exit(1);
    }
    stat(argv[2], &st);
    size = st.st_size;
    fdwrite = open("encoded.txt", O_WRONLY | O_CREAT, 0755);
    if (fdwrite == -1) {
      errorf("Error opening file\n");
      close(fdwrite);
      exit(1);
    }

    buffread = (char *)malloc(3);
    buffwrite = (char *)malloc(4);
    buffreadsize = 3;
    buffwritesize = 4;

    int readed;
    while ((readed = read(fd, buffread, buffreadsize)) > 0) {
      base64encode(buffread, buffreadsize, buffwrite,
             buffwritesize);
      write(fdwrite, buffwrite, buffwritesize);
      writtenBytes += readed;
      memset(buffread, 0, buffreadsize);
            progress(0);
    }
    close(fd);
    close(fdwrite);
        infof("Encode task finished and successfully written in encoded.txt\n");
  }
    else if (strcmp(argv[1], "--decode") == 0) {
    int fd, fw, buffreadsize;
    size_t *buffwritesize;
    char *buffread;
    unsigned char *buffwrite;

    fd = open(argv[2], O_RDONLY);
    if (fd == -1) {
      errorf("Error opening file\n");
      close(fd);
      exit(1);
    }
    stat(argv[2], &st);
    size = st.st_size;
    fw = open("decoded.txt", O_WRONLY | O_CREAT, 0755);
    if (fw == -1) {
      errorf("Error opening file\n");
      close(fw);
      exit(1);
    }

    buffread = (char *)malloc(4);
    buffwrite = (unsigned char *)malloc(3);
    buffwritesize = (size_t *) malloc(sizeof(size_t));
    buffreadsize = 4;
    *buffwritesize = 3;

    int readed;
    while ((readed = read(fd, buffread, buffreadsize)) > 0) {
      base64decode(buffread, buffreadsize, buffwrite,
             buffwritesize);
      write(fw, buffwrite, *buffwritesize);
      writtenBytes += readed;
      memset(buffread, 0, buffreadsize);
            progress(1);
    }
    close(fd);
    close(fw);

        if(strcmp(argv[1],"encoded.txt")){
            warnf("The file may not be correctly decoded.\n");
        }
        infof("Decoded\n");
  }
    else {
    errorf("--<task> [encode | decode]");
  }

    return 0;
}
