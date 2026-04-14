# QConn & Momentics IDE — Target Connectivity Guide

---

## What is QConn?

**QConn** (`qconn`) is a lightweight daemon that runs on a **QNX Neutrino target** (physical board, VM, or QEMU instance). It acts as a communication bridge between the target system and the **Momentics IDE** running on the host machine.

> Think of `qconn` as the agent that "opens the door" to your target — without it, Momentics has no way to reach the running QNX system.
> between us its just a rich, smart `ssh`.

---

## Key Features

| Feature | Description |
|---|---|
| **Remote Deployment** | Transfers compiled binaries from host to target over the network |
| **Remote Execution** | Launches programs on the target directly from Momentics |
| **Debug Support** | Enables GDB-based source-level debugging on the live target |
| **System Profiling** | Feeds real-time CPU, memory, and process data into Momentics tools |
| **Application Profiling** | Supports call-count and timing profiling of target applications |
| **Memory Analysis** | Works with Momentics memory tools to detect leaks and errors |
| **File System Access** | Allows browsing and managing the target file system from the IDE |

---

## How the Connection Works

```
[ Host Machine ]                      [ QNX Target / QEMU VM ]
  Momentics IDE  ──── TCP (port 8000) ────►  qconn daemon
       │                                          │
       │  deploys binary, sends commands          │
       │◄──── output / debug data ────────────────┘
```

1. `qconn` starts automatically on the QNX target (or can be launched manually).
2. It listens on **TCP port 8000** by default.
3. Momentics connects to it using the target's **IP address**.
4. All deployment, execution, and debug sessions flow through this single connection.

### Verify its running on the target
```sh
pidin | grep qonn
```

### if not running, Start QConn on the Target

```sh
# Run on the QNX target shell
qconn &
```

Make sure the target's IP is reachable from the host (ping it first).

### Get its IP

```sh
ifconfig
```

---

## Adding the Target in Momentics

1. Open **Target Navigator → New QNX Target**
2. Click **Add** and enter:
   - **Hostname / IP:** e.g., `192.168.122.219`
   - **Port:** `8000` (default)

---

## Note you can also connect via ssh

### Give your `root` a password in the target

```sh
passwd
```

### Connect from Host

```sh
ssh root@192.168.122.219
```

---

## Step-by-Step: Build & Run a Project in Momentics

### 1. Create a C Project

- **File → New → QNX C Project**
- Set the following options:
  - **Project type:** Executable
  - **Target OS:** QNX Neutrino
  - **Architecture:** `x86_64` *(must match your QEMU configuration)*

---

### 2. Add a Simple C File

```c
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Hello Ehab from QNX on QEMU!\n");

    for(int i = 0; i < 10; i++){
        printf("%d ", i);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");

    return 0;
}
```

---

### 3. Build the Project

- Click the **Build** button (or `Ctrl+B`).
- Momentics compiles the source and generates an **ELF binary** targeting QNX x86_64.

---

### 4. Configure the Run Target

- Go to **Run → Run Configurations**
- Create a new **QNX C/C++ Application** configuration
- Set:
  - **Target:** `192.168.122.219` (your QEMU VM's IP)
  - **Connection:** via `qconn`

---

### 5. Run the Application 🚀

- Click **Run**.
- Momentics will automatically:
  1. Upload the binary to the target via `qconn`
  2. Execute the program on the QNX target
  3. Stream the output back to the **Console** view in the IDE

---

## Expected Output

In the **Momentics Console**:

```
Hello from QNX on QEMU!
0 1 2 3 4 5 6 7 8 9
```

Each line prints one second apart, confirming that the binary is running live on the QNX target.

---

## The file will appear where you uploaded it when its running then disapper when finished

```sh
# ls -la data/home/qnxuser/myapps/
total 30
drwxr-xr-x  2 root    root    4096 2026-04-14 14:22 .
drwx--x---+ 5 qnxuser qnxuser 4096 2026-04-14 13:37 ..
-rwxr-xr-x  1 root    root    7416 2026-04-14 14:22 cproj
# ls data/home/qnxuser/myapps/     
# 
```

---

## Quick Reference

| Step | Action |
|---|---|
| Start daemon | Run `qconn &` on the QNX target |
| Default port | `8000` (TCP) |
| Add target in IDE | Window → Preferences → QNX → Target Systems |
| New project | File → New → QNX C Project |
| Build | `Ctrl+B` or Build button |
| Deploy & run | Run → Run Configurations → QNX C/C++ Application |
