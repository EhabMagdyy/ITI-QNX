# Process Creation in QNX 8: fork() and exec*()

In QNX 8, as in other POSIX-compliant operating systems, process management relies on two fundamental system calls: `fork()` and the `exec` family of functions.

---

## 1. The fork() System Call
The `fork()` function is used to create a new process by duplicating the existing one.

* **Behavior:** It creates an exact copy of the calling process (the **parent**) and calls it the **child**.
* **Return Value:** * Returns **0** to the child process.
    * Returns the **PID of the child** to the parent process.
    * Returns **-1** if the creation failed.
* **QNX Context:** In QNX, the child inherits many attributes from the parent, but they have separate memory spaces. The child starts with a single thread (TID 1), regardless of how many threads the parent had.

## 2. The exec*() Family
The `exec` family of functions replaces the current process image with a new process image.

* **Behavior:** When a process calls an `exec` function, it stops executing its current code and starts executing the code of a specified file. The process PID remains the same, but the memory, data, and stack are wiped and replaced.
* **The Fork-Exec Pattern:** Most applications use these together. The parent calls `fork()`, and inside the resulting child process, `exec()` is called to run a different program.

---

## 3. Comparison of exec*() Variants

The variations in the `exec` family are determined by the suffixes appended to the name. These suffixes dictate how arguments are passed and how the executable is located.

### Suffix Key:
| Suffix | Meaning | Description |
| :--- | :--- | :--- |
| **l** | **List** | Arguments are passed as a comma-separated list ending in `NULL`. |
| **v** | **Vector** | Arguments are passed as an array of strings (`char *argv[]`). |
| **p** | **Path** | Searches the `PATH` environment variable to find the executable. |
| **e** | **Environment** | Allows passing a custom environment array to the new process. |

### Summary Comparison Table:

| Function | Arg Style | Path Search | Env Source | Use Case |
| :--- | :--- | :--- | :--- | :--- |
| **execl** | List | No (Full path req) | Parent's Env | When path and args are fixed. |
| **execlp** | List | **Yes** | Parent's Env | **Most common**; runs shell commands easily. |
| **execle** | List | No (Full path req) | **Custom Env** | When you need specific env variables for the child. |
| **execv** | Vector | No (Full path req) | Parent's Env | When the number of arguments is dynamic. |
| **execvp** | Vector | **Yes** | Parent's Env | Dynamic arguments with PATH search flexibility. |
| **execve** | Vector | No (Full path req) | **Custom Env** | Full control over arguments and environment. |

---

## Summary of Execution Flow

1.  **Original Process** calls `fork()`.
2.  **Parent Process** continues or waits for the child.
3.  **Child Process** exists as a clone of the parent.
4.  **Child Process** calls `exec*()`.
5.  **New Program** (e.g., `ls`) loads into the child's PID.
6.  **Child Process** (now running the new program) terminates.
7.  **Parent Process** receives the termination signal.

---

## Output
```
[Parent] PID: 634882, TID: 1
--------------------------------------------------
[Parent] Created child with PID: 634898
[Child] PID: 634898, TID: 1, Parent PID: 634882

Executing ls -l ...
total 14
-rw-r--r--  1 root root   36 2026-05-13 17:54 keep_files
-rwxr-xr-x  1 root root 7632 2026-05-13 18:12 lab_2
--------------------------------------------------
[Parent] Child has exited. Parent process cleaning up and exiting.
```