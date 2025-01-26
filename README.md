# Chat Application

This is a Winsock C++ chat server built using sockets, paired with a Python client script for testing purposes.

## Compilation and Setup

To compile the C++ chat server using `g++`:

```bash
$ g++ src/main.cpp src/network/NetworkUtils.cpp src/client/ClientManager.cpp -o bin/main -lWs2_32
```

After compiling, run the resulting `bin/main.exe` file to host the server:

```bash
$ ./bin/main.exe
```

## Running the Tester Client

To run the Python client for testing the server:

```bash
$ python src/tester.py
```

## Features

- **Server**:
  - Built with Winsock in C++ for handling multiple client connections.
  - Efficient socket-based communication.

- **Client**:
  - Support for multiple clients
  - A Python script designed for testing the server's functionality.

## Future Enhancements
- Implement authentication and user management.
- Enhance error handling and robustness.

## Requirements

- **Server**:
  - Windows OS (for Winsock API).
  - `g++` compiler.

- **Client**:
  - Python 3.x.

## Notes
- Ensure the server is running before starting the Python tester client.
- Additional features and functionality can be added to expand the chat application's capabilities.

