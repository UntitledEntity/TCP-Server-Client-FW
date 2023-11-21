# TCP Server Client Framework

This TCP Server (Linux-based) and Client (Windows-based) framework makes it incredibly easy to implement a TCP server for transmitting data/files, authorizing users, and much more. 
This is designed and written in modern C++ and is created in a readable fashion, allowing it to be built upon to fit any application.

# Encryption 

This utilizes [AES encryption](https://en.wikipedia.org/wiki/AES), which passes a randomly generated encryption key on a client connection. 

# Compilation

The [TCPServer](https://github.com/UntitledEntity/TCP-Server-Client-FW/tree/main/TCPServer) directory contains a [cmpl.sh file](https://github.com/UntitledEntity/TCP-Server-Client-FW/blob/main/TCPServer/cmpl.sh) which utilizes g++ to compile the files for the server.

# Dependencies 
- [OpenSSL](https://www.openssl.org/)
- [WinSock](https://learn.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock) (Windows Client)
- [Socket](https://github.com/torvalds/linux/blob/master/include/linux/socket.h) (Linux Server)

## License

> Copyright (c) 2023 Jacob Gluska/UntitledEntity

This project is licensed under the [GNU Affero General Public License v3.0](https://www.gnu.org/licenses/agpl-3.0.en.htm) - see the [LICENSE](https://github.com/UntitledEntity/intertwined-web/blob/main/LICENSE) file for details.
