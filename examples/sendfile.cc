// sendfile.cc
#include "helper.h"

#include <sys/sendfile.h>
#include <sys/stat.h>

int main() {
    int fd = helper::open("index.html", O_RDONLY);

    struct stat statbuf;
    if (fstat(fd, &statbuf) == -1)
        error::Exit(errno, "fstat()");

    int connfd = helper::acceptOneConnect("localhost:8888");

    ssize_t num_send = sendfile(connfd, fd, nullptr, statbuf.st_size);
    if (num_send == -1) {
        error::Exit(errno, "sendfile()");
    } else if (num_send < statbuf.st_size) {
        error::Exit("sendfile() return %zd (filesize: %ld)",
                num_send, static_cast<long>(statbuf.st_size));
    } else {
        printf("sendfile() success!\n");
        close(connfd);
    }
    return 0;
}
