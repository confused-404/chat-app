import socket
import struct
import threading

def send_message(sock, message):
    message = message.encode('utf-8')
    message_length = struct.pack('!I', len(message))
    sock.sendall(message_length + message)

def receive_message(sock):
    try:
        raw_msglen = recvall(sock, 4)
        if not raw_msglen:
            print("Failed to receive message length")
            return None
        msglen = struct.unpack('!I', raw_msglen)[0]
        print(f"Expecting message of length: {msglen}")
        message = recvall(sock, msglen)
        if message is None:
            print("Failed to receive full message")
            return None
        return message.decode('utf-8')
    except socket.timeout:
        # print("\n\nReceive operation timed out\n\n")
        return None

def recvall(sock, n):
    data = bytearray()
    while len(data) < n:
        packet = sock.recv(n - len(data))
        if not packet:
            return None
        data.extend(packet)
    return data

def handle_input(sock):
    while True:
        message = input("Enter message: ")
        if message.lower() == 'exit':
            sock.close()
            break
        send_message(sock, message)

def main():
    server_address = ('localhost', 54000)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(5)

    try:
        sock.connect(server_address)
        print("Connected to server")

        prompt = receive_message(sock)
        if prompt:
            print(prompt)

        input_thread = threading.Thread(target=handle_input, args=(sock,))
        input_thread.start()

        while True:
            response = receive_message(sock)
            if response:
                print(response)

    finally:
        sock.close()

if __name__ == "__main__":
    main()