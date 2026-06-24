# Thread-Safe In-Memory File System (C++)

A high-performance, concurrent, and memory-safe In-Memory File System implemented in modern C++. This project demonstrates production-grade Low-Level Design (LLD) principles, focusing on clean object-oriented architecture, efficient lookups, and robust multi-threaded synchronization.

## 🚀 Key Features

- **Hierarchical N-ary Tree Structure:** Represents real-world directory nesting.
- **O(1) Global Lookups:** Utilizes a global path hash map to bypass slow sequential directory traversals.
- **Thread-Safe Operations:** Implements a Readers-Writer locking pattern enabling concurrent reads and safe exclusive writes.
- **Deadlock-Free Design:** Adheres to a strict lock-acquisition hierarchy preventing cyclic wait conditions.
- **Zero Manual Memory Management:** Leverages smart pointers (`std::shared_ptr`) and RAII to eliminate memory leaks and dangling references.

---

## 🛠️ Architectural Design Choices

### 1. Composite Design Pattern
Instead of relying on costly runtime type-casting (`dynamic_cast`), the system uses the **Composite Design Pattern**. Both `File` (Leaf) and `Folder` (Composite) inherit from a unified `Entity` base class. Common structural operations are exposed polymorphically at the base layer.

### 2. Concurrency & Synchronization
To maximize throughput in read-heavy file system environments:
- **Reads (`list`):** Acquire a shared lock (`std::shared_lock<std::shared_mutex>`), allowing multiple threads to query directories simultaneously.
- **Writes (`createEntity`, `deleteEntity`, `renameEntity`):** Acquire an exclusive lock (`std::unique_lock<std::shared_mutex>`), blocking concurrent operations to safely update both the directory tree and the global lookup map.

---

## 📁 Class Structure

- `Entity` (Abstract Base Component)
  - `File` (Leaf Component - represents data files)
  - `Folder` (Composite Component - manages internal child vectors)
- `ThreadSafeFileSystem` (Facade/Controller - manages API contracts, indexing, and locks)

---

## 💻 Getting Started

### Prerequisites
- A compiler supporting **C++17** or higher (e.g., GCC 7+, Clang 5+, MSVC 2017+)
- CMake (Optional, minimum 3.10+)

### Compilation via Terminal
```bash
g++ -std=c++17 main.cpp -o file_system -lpthread
./file_system