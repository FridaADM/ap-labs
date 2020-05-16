#include <stdio.h>
#include "logger.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>


/*
Colors and texcolor() references: https://www.linuxjournal.com/article/8603
stdarg: https://pubs.opengroup.org/onlinepubs/009695399/basedefs/stdarg.h.html
*/

#define RESET         0
#define BRIGHT        1
#define HIDDEN        8

#define BLACK         0
#define RED           1
#define GREEN         2
#define YELLOW        3
#define BLUE          4
#define MAGENTA       5
#define CYAN          6
#define WHITE         7

#define STDOUT 0
#define SYSLOG 1

int type = 0;

void textcolor(int attr, int fg, int bg) {
  char command[13];
    sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
    printf("%s", command);
}

int initLogger(char *logType) {
    if (strcmp(logType, "syslog") == 0) {
        type = SYSLOG;
        printf("\n");
    }
    else if (strcmp(logType, "stdout") == 0) {
        type = STDOUT;
        printf("\n");
    }
    else {
        printf("\nInvalid log type\n");
    }
    return 0;
}

int infof(const char *format, ...) {
  int flag;
  va_list arg;
  va_start (arg, format);
  if (type == STDOUT) {
      textcolor(BRIGHT, CYAN, HIDDEN);
      printf ("INFO: ");
      textcolor (RESET, WHITE, HIDDEN);
      flag = vprintf (format, arg);
      va_end (arg);
      return flag;
  }
  else {
      openlog("syslog", LOG_NDELAY, LOG_DAEMON);
      vsyslog(LOG_INFO, format, arg);
      closelog();
      va_end(arg);
      return 0;
  }
}

int warnf(const char *format, ...) {
  int flag;
  va_list arg;
  va_start (arg, format);
  if (type == STDOUT) {
      textcolor (BRIGHT, YELLOW, HIDDEN);
      printf ("WARNING: ");
      textcolor(RESET, WHITE, HIDDEN);
      flag = vprintf (format, arg);
      va_end (arg);
      return flag;
  }
  else {
      openlog("syslog", LOG_NDELAY, LOG_DAEMON);
      vsyslog(LOG_WARNING, format, arg);
      closelog();
      va_end(arg);
      return 0;
  }
}

int errorf(const char *format, ...) {
  int flag;
  va_list arg;
  va_start (arg, format);
  if (type == STDOUT) {
      textcolor (BRIGHT, RED, HIDDEN);
      printf ("ERROR: ");
      textcolor(RESET, WHITE, HIDDEN);
      flag = vprintf (format, arg);
      va_end (arg);
      return flag;
  }
  else {
      openlog("syslog", LOG_NDELAY, LOG_DAEMON);
      vsyslog(LOG_ERR, format, arg);
      closelog();
      va_end(arg);
      return 0;
  }
}

int panicf(const char *format, ...) {
  int done;
  va_list arg;
  va_start (arg, format);
  if (type == STDOUT) {
      textcolor(BRIGHT, YELLOW, RED);
      printf ("PANIC:");
      textcolor(RESET, WHITE, HIDDEN);
      printf(" ");
      done = vprintf (format, arg);
      va_end (arg);
      return done;
  }
  else {
      openlog("syslog", LOG_NDELAY, LOG_DAEMON);
      vsyslog(LOG_ALERT, format, arg);
      closelog();
      va_end(arg);
      return 0;
  }
}
