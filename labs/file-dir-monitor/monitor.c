#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>

#include "logger.h"

#define BUFFSIZE sizeof(struct inotify_event) * 1024

int fd;
int fw;
int fr;
char* p;
struct inotify_event *event;

void displayEvent(struct inotify_event* e){
    if (e->mask & IN_CREATE)
        infof("%s Created\n", e->name);
    if (e->mask & IN_DELETE)
        warnf("%s Deleted\n", e->name);
    if (e->mask & IN_MODIFY)
        infof("%s Modidied\n", e->name);
    if (e->mask & IN_MOVED_FROM)
        infof("%s Renamed\n",e->name);
    if (e->mask & IN_MOVED_TO)
        infof("New: %s\n", e->name);
}

int main(char argc, char** argv){
    if(argc != 2){
        errorf("Execution: [./monitor <dir>]\n");
        exit(EXIT_FAILURE);
    }
    fd = inotify_init1(O_NONBLOCK);
    if (fd == -1) {
		errorf("Unable to create fd\n");
		exit(EXIT_FAILURE);
	}
    fw = inotify_add_watch(fd, argv[1], IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO );
    char* buff = (char*)malloc(BUFFSIZE);
    while(1){
        fr = read(fd, buff, BUFFSIZE);
        p = buff;
        event = (struct inotify_event*)p;
        for (p = buff; p < buff + fr; ) {
             event = (struct inotify_event *) p;
             displayEvent(event);
             p += sizeof(struct inotify_event) + event->len;
         }
    }
    close(fd);
    return 0;
}
