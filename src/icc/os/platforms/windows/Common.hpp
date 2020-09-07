//
// Created by RedRa on 15.04.2020.
//

#ifndef ICC_COMMON_HPP
#define ICC_COMMON_HPP

extern "C" {

#include <winsock2.h>
#include <windows.h>

}

namespace icc {
namespace os {

struct Handle {
    HANDLE handle_;
};

constexpr Handle kInvalidHandle{nullptr};

enum class EventType {
  kRead,
  kWrite,
  kError,
};

#define PORT 5150
#define DATA_BUFF_SIZE 8192

// Typedef definition
typedef struct {
  OVERLAPPED overlapped;
  WSABUF dataBuf;
  CHAR buffer[DATA_BUFF_SIZE];
  DWORD bytesSend;
  DWORD bytesRecv;
} IOOperationData, * LpperIOOperationData;

typedef struct {
  SOCKET            socket;
  SOCKADDR_STORAGE  clientAddr;
  // Other information useful to be associated with the handle
} HandleData, * LpperHandleData;

}
}

#endif //ICC_COMMON_HPP
