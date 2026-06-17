#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/slop.sock"

int main(int argc, char *argv[]) {
    const char *path = (argc >= 2) ? argv[1] : getenv("PWD");
    if (!path) return 0;

    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) return 0;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sock);
        return 0;
    }

    /* Send path */
    write(sock, path, strlen(path));
    shutdown(sock, SHUT_WR);

    /* Read response */
    char buf[128];
    ssize_t n;
    while ((n = read(sock, buf, sizeof(buf))) > 0) {
        write(1, buf, n);
    }

    close(sock);
    return 0;
}
