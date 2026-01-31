# Architecture Documentation

## Overview

This document provides a detailed technical explanation of the Encrypted Reverse Shell system architecture, including network communication flow, encryption mechanisms, and operational workflows.

## System Components

### 1. Common Encryption Module

**Location**: `common/crypto.c` and `common/crypto.h`

**Purpose**: Provides shared encryption/decryption functionality for both client and server.

#### Functions

##### `encrypt_data(char *data, size_t length)`
- **Purpose**: Encrypts data in-place using XOR cipher
- **Algorithm**: XOR each byte with `ENCRYPTION_KEY`
- **Parameters**:
  - `data`: Buffer containing plaintext data
  - `length`: Number of bytes to encrypt
- **Side Effects**: Modifies data buffer in-place

##### `decrypt_data(char *data, size_t length)`
- **Purpose**: Decrypts data in-place using XOR cipher
- **Algorithm**: XOR each byte with `ENCRYPTION_KEY` (symmetric operation)
- **Parameters**:
  - `data`: Buffer containing encrypted data
  - `length`: Number of bytes to decrypt
- **Side Effects**: Modifies data buffer in-place

#### Encryption Algorithm

**XOR Cipher**:
```
encrypted_byte = plaintext_byte XOR ENCRYPTION_KEY
plaintext_byte = encrypted_byte XOR ENCRYPTION_KEY
```

**Properties**:
- Symmetric: Same operation for encryption and decryption
- Fast: Single XOR operation per byte
- **Not secure**: Vulnerable to frequency analysis and known-plaintext attacks
- **Educational only**: Demonstrates concept, not suitable for production

**Configuration**:
```c
#define ENCRYPTION_KEY 0xAA  // Default key (170 in decimal)
```

To change the key, modify this value in `common/crypto.h` and rebuild.

---

### 2. Server Component

**Location**: `server/server.c`

**Purpose**: Listens for incoming client connections and provides interactive command shell.

#### Architecture

```
┌─────────────────────────────────────────┐
│           Server Process                │
├─────────────────────────────────────────┤
│                                         │
│  ┌───────────────────────────────────┐ │
│  │   Socket Initialization           │ │
│  │   - Create socket                 │ │
│  │   - Set SO_REUSEADDR              │ │
│  │   - Bind to port                  │ │
│  │   - Listen for connections        │ │
│  └───────────────────────────────────┘ │
│                  │                      │
│                  ▼                      │
│  ┌───────────────────────────────────┐ │
│  │   Accept Loop                     │ │
│  │   - Wait for client connection    │ │
│  │   - Accept connection             │ │
│  │   - Call handle_client()          │ │
│  └───────────────────────────────────┘ │
│                  │                      │
│                  ▼                      │
│  ┌───────────────────────────────────┐ │
│  │   Client Handler                  │ │
│  │   - Display prompt                │ │
│  │   - Read command from stdin       │ │
│  │   - Encrypt command               │ │
│  │   - Send to client                │ │
│  │   - Receive encrypted output      │ │
│  │   - Decrypt output                │ │
│  │   - Display to user               │ │
│  └───────────────────────────────────┘ │
│                                         │
└─────────────────────────────────────────┘
```

#### Key Functions

##### `main()`
- Initializes server socket
- Binds to specified port (default: 4444)
- Enters accept loop to handle incoming connections
- Sequential client handling (one at a time)

##### `handle_client(int client_socket)`
- Interactive command loop
- Reads commands from user input
- Encrypts commands before transmission
- Receives and decrypts client responses
- Handles "exit" command to disconnect client

#### Network Configuration

```c
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;        // IPv4
server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
server_addr.sin_port = htons(port);       // Network byte order
```

---

### 3. Client Component

**Location**: `client/client.c`

**Purpose**: Connects to server, executes commands, and returns encrypted output.

#### Architecture

```
┌─────────────────────────────────────────┐
│           Client Process                │
├─────────────────────────────────────────┤
│                                         │
│  ┌───────────────────────────────────┐ │
│  │   Initialization                  │ │
│  │   - Parse arguments               │ │
│  │   - Optional: daemonize()         │ │
│  └───────────────────────────────────┘ │
│                  │                      │
│                  ▼                      │
│  ┌───────────────────────────────────┐ │
│  │   Connection Loop (Infinite)      │ │
│  │   - Attempt connection            │ │
│  │   - If failed: sleep & retry      │ │
│  │   - If success: enter cmd loop    │ │
│  └───────────────────────────────────┘ │
│                  │                      │
│                  ▼                      │
│  ┌───────────────────────────────────┐ │
│  │   Command Loop                    │ │
│  │   - Receive encrypted command     │ │
│  │   - Decrypt command               │ │
│  │   - Execute via popen()           │ │
│  │   - Capture output                │ │
│  │   - Encrypt output                │ │
│  │   - Send to server                │ │
│  └───────────────────────────────────┘ │
│                  │                      │
│                  ▼                      │
│  ┌───────────────────────────────────┐ │
│  │   Reconnection Logic              │ │
│  │   - On disconnect: close socket   │ │
│  │   - Sleep RETRY_DELAY seconds     │ │
│  │   - Return to connection loop     │ │
│  └───────────────────────────────────┘ │
│                                         │
└─────────────────────────────────────────┘
```

#### Key Functions

##### `main()`
- Parses command-line arguments (server IP, port, daemon mode)
- Optionally daemonizes process
- Calls `run_client()` to start main loop

##### `run_client(const char *server_ip, int port, int daemon_mode)`
- Infinite connection loop
- Attempts connection via `connect_to_server()`
- On success: enters command execution loop
- On failure: sleeps and retries

##### `connect_to_server(const char *server_ip, int port)`
- Creates TCP socket
- Configures server address structure
- Attempts connection
- Returns socket descriptor or -1 on failure

##### `execute_command(const char *command, char *output, size_t output_size)`
- Executes command using `popen()`
- Captures stdout and stderr
- Stores output in provided buffer
- Handles errors gracefully

##### `daemonize()`
- Forks process twice (double fork technique)
- Detaches from terminal
- Closes stdin, stdout, stderr
- Changes working directory to `/`
- Sets umask to 0

#### Daemon Mode

**Double Fork Technique**:
1. First `fork()`: Creates child process
2. Parent exits (child becomes orphan)
3. `setsid()`: Child becomes session leader
4. Second `fork()`: Creates grandchild
5. First child exits (grandchild is not session leader)
6. Grandchild continues as daemon

**Benefits**:
- No controlling terminal
- Cannot acquire controlling terminal
- Runs in background
- Survives terminal closure

---

## Communication Flow

### Complete Workflow

```
Server                                  Client
  │                                       │
  │  1. Listen on port 4444               │
  │◄──────────────────────────────────────┤ 2. Connect to server:4444
  │                                       │
  │  3. Accept connection                 │
  ├──────────────────────────────────────►│ 4. Connection established
  │                                       │
  │  5. User enters: "whoami"             │
  │                                       │
  │  6. Encrypt("whoami")                 │
  │     = [encrypted bytes]               │
  │                                       │
  ├──────────────────────────────────────►│ 7. Receive encrypted command
  │                                       │
  │                                       │  8. Decrypt command
  │                                       │     = "whoami"
  │                                       │
  │                                       │  9. Execute: popen("whoami")
  │                                       │     Output: "user\n"
  │                                       │
  │                                       │ 10. Encrypt("user\n")
  │                                       │     = [encrypted bytes]
  │                                       │
  │◄──────────────────────────────────────┤ 11. Send encrypted output
  │                                       │
  │ 12. Decrypt output                    │
  │     = "user\n"                        │
  │                                       │
  │ 13. Display: "user"                   │
  │                                       │
  │ 14. Prompt: "shell> "                 │
  │                                       │
```

### Data Encryption Example

**Plaintext Command**: `"ls"`

**Encryption Process**:
```
'l' (0x6C) XOR 0xAA = 0xC6
's' (0x73) XOR 0xAA = 0xD9
```

**Encrypted Data**: `[0xC6, 0xD9]`

**Transmitted over network**: `[0xC6, 0xD9]`

**Decryption Process** (client side):
```
0xC6 XOR 0xAA = 0x6C ('l')
0xD9 XOR 0xAA = 0x73 ('s')
```

**Decrypted Command**: `"ls"`

---

## Reconnection Logic

### Client Reconnection Behavior

```
┌─────────────────────┐
│  Start Client       │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│ Try Connect         │
└──────────┬──────────┘
           │
           ├─── Success ───┐
           │               ▼
           │         ┌──────────────┐
           │         │ Command Loop │
           │         └──────┬───────┘
           │                │
           │                │ Connection Lost
           │                ▼
           │         ┌──────────────┐
           │         │ Close Socket │
           │         └──────┬───────┘
           │                │
           │                ▼
           │         ┌──────────────┐
           │         │ Sleep 5 sec  │
           │         └──────┬───────┘
           │                │
           └─── Failure ────┴────► (Loop back to Try Connect)
```

**Configuration**:
```c
#define RETRY_DELAY 5  // Seconds between reconnection attempts
```

**Behavior**:
- Infinite retry loop
- No maximum retry count
- Persistent connection attempts
- Graceful handling of network failures

---

## Security Analysis

### Vulnerabilities (Intentional for Education)

1. **Weak Encryption**
   - XOR cipher is trivially breakable
   - No key exchange mechanism
   - Static key hardcoded in binary

2. **No Authentication**
   - Server accepts any client
   - No password or certificate verification

3. **No Integrity Checking**
   - Data can be modified in transit
   - No HMAC or digital signatures

4. **Command Injection**
   - Server has full shell access
   - No command sanitization

5. **No Rate Limiting**
   - Vulnerable to DoS attacks
   - No connection throttling

### Educational Value

These vulnerabilities are **intentional** to demonstrate:
- Why strong encryption is necessary
- Importance of authentication
- Need for integrity checks
- Risks of command execution
- Defense-in-depth principles

---

## Potential Enhancements

### Security Improvements
- Replace XOR with AES-256 (OpenSSL)
- Implement Diffie-Hellman key exchange
- Add HMAC for integrity verification
- Implement client authentication (passwords/certificates)

### Functionality Improvements
- Multi-client support (threading/select)
- Session logging with timestamps
- File upload/download capabilities
- Configurable command whitelist/blacklist

### Stealth Improvements
- Process name obfuscation
- Traffic pattern randomization
- Polymorphic encryption keys
- Anti-debugging techniques

**Note**: These enhancements are beyond the scope of this educational project but represent real-world considerations.

---

## Conclusion

This architecture demonstrates fundamental concepts in:
- Network programming with sockets
- Symmetric encryption
- Process management
- Client-server communication
- Persistence mechanisms

Understanding these concepts is essential for both offensive security (red team) and defensive security (blue team) professionals.

**Remember**: Use this knowledge responsibly and only in authorized environments.
