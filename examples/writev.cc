// writev.cc: 模拟客户向服务端发送HTTP请求，获取指定文件
#include "helper.h"

#include <sys/stat.h>

#include <vector>

// HTTP状态码和状态信息
static const char* status_line[] = {"200 OK", "500 Internal server"};

int main() {
    const char* filename = "index.html";
    bool valid = false;  // 客户是否可以访问指定文件
    std::vector<char> file_buf;  // 缓存文件内容
    int connfd = helper::acceptOneConnect("localhost:8888");

    struct stat statbuf;
    if (stat(filename, &statbuf) != -1 && S_ISREG(statbuf.st_mode)) {
        // 验证客户访问权限，实际场景下可能会检查权限数据库或cookie之类
        if (statbuf.st_mode & S_IROTH) {
            file_buf.resize(statbuf.st_size);
            int fd = open(filename, O_RDONLY);
            error::ExitIf(fd == -1, errno, "open() %s", filename);
            if (read(fd, file_buf.data(), statbuf.st_size) != -1) {
                valid = true;  // 读取文件成功
            }
            close(fd);
        }
    }

    char header_buf[1024];  // 保存HTTP应答的状态行、头部字段和一个空行
    bzero(header_buf, sizeof(header_buf));

    if (valid) {
        size_t len = snprintf(header_buf, sizeof(header_buf), "%s %s\r\n",
                "HTTP/1.1", status_line[0]);
        len += snprintf(header_buf + len, sizeof(header_buf) - len,
                "Content-Length: %ld\r\n", static_cast<long>(statbuf.st_size));
        len += snprintf(header_buf + len, sizeof(header_buf) - len, "\r\n");

        // 利用writev避免在用户层的缓冲区重新拼接
        struct iovec iv[2] = {
            {header_buf, len},
            {file_buf.data(), file_buf.size()}
        };
        if (writev(connfd, iv, 2) == -1)
            error::Exit("writev");
    } else {  // 目标文件无效
        size_t len = snprintf(header_buf, sizeof(header_buf), "%s %s\r\n",
                "HTTP/1.1", status_line[1]);
        len += snprintf(header_buf + len, sizeof(header_buf) - len, "%s", "\r\n");
        inet::send(connfd, header_buf, len);
    }

    close(connfd);
    return 0;
}
