#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>

int main() {
    int epfd = epoll_create1(0);
    struct epoll_event event, events[1];

    event.events = EPOLLIN;
    event.data.fd = 0;
    epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &event);

    while (1) {
        int n = epoll_wait(epfd, events, 1, -1); 

        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == 0) {
                char buf[1];
                read(0, buf, 1);
                printf("Key pressed: %c\n", buf[0]);
            }
        }
    }
    return 0;
}
