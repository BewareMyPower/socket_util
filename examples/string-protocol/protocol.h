// protocol.h
#pragma once

#include <socket_util.h>
using namespace socket_util;

namespace protocol {

class String : noncopyable {
private:
#pragma pack(1)
    struct Packet {
        uint32_t length;
        char data[1];  // 仅用于方便取得末尾连续字节序列(长度未知)的首地址
    };
#pragma pack()
    static constexpr size_t HEADER_SIZE = sizeof(Packet) - 1;

    Packet& parse() noexcept { return *reinterpret_cast<Packet*>(bytes_.data()); }
    const Packet& parse() const noexcept { return static_cast<const Packet&>(const_cast<String*>(this)->parse()); }

public:
    explicit String(size_t datasize = 4092) : bytes_(HEADER_SIZE + datasize) {}

    bool getline(FILE* fp) noexcept {
        auto& packet = parse();
        if (!fgets(bytes_.data() + 4, bytes_.size() - HEADER_SIZE, fp))
            return false;

        setLength(strlen(packet.data));
        return true;
    }

    void removeNewline() noexcept {
        auto& packet = parse();
        uint32_t length = getLength();
        while (length > 0 && packet.data[length - 1] == '\n') {
            packet.data[length - 1] = '\0';
            length--;
            if (length > 0 && packet.data[length - 1] == '\r') {
                packet.data[length - 1] = '\0';
                length--;
            }
        }
        setLength(length);
    }

    bool recvFrom(int fd, int flags = 0) noexcept {
        auto& packet = parse();
        if (!inet::recvNBytes(fd, &packet.length, sizeof(packet.length), flags)) {
            setLength(0);
            return false;
        }

        auto length = getLength();
        bytes_.resize(HEADER_SIZE + length + 1);
        if (!inet::recvNBytes(fd, &packet.data[0], length, flags)) {
            setLength(0);
            return false;
        }

        bytes_.back() = '\0';
        return true;
    }

    bool sendTo(int fd, int flags = 0) const noexcept {
        return inet::sendAll(fd, bytes_.data(), HEADER_SIZE + getLength(), flags);
    }

    std::string getBuffer() const noexcept { return parse().data; }

    uint32_t getLength() const noexcept { return ntohl(parse().length); }

private:
    std::vector<char> bytes_;

    void setLength(uint32_t length) noexcept { parse().length = htonl(length); }
};

}
