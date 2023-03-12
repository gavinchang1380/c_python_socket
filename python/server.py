# python socket_server.py
import socket
import struct

HOST = "127.0.0.1"
PORT = 5000
send_buffer = bytearray(1024 * 1024)
send_buffer[:4] = b'ABCD'
send_buffer[-4:] = b'DCBA'

def send_msg(sock, msg):
    msg = struct.pack('>I', len(msg)) + msg
    sock.sendall(msg)

def recv_msg(sock):
    raw_msglen = recvall(sock, 4)
    if not raw_msglen:
        return None
    msglen = struct.unpack('>I', raw_msglen)[0]
    print(msglen)
    return recvall(sock, msglen)

def recvall(sock, n):
    data = bytearray()
    while len(data) < n:
        packet = sock.recv(n - len(data))
        if not packet:
            return None
        data.extend(packet)
    return data

def tcpServer():
    # listen the port
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, PORT))
    s.listen(1)  # only connect with 1 client per time
    print("waiting client connect...")

    # accept client connection, or block
    conn, address = s.accept()
    print("connection from ", str(address))

    while True:
        # receive buffer
        recv_buffer = recv_msg(conn)
        if not recv_buffer:
            break
        print(f"receive from client {len(recv_buffer)} bytes: {recv_buffer[:4]} ... {recv_buffer[-4:]}")

        # send buffer
        send_msg(conn, send_buffer)
        print(f"send to client {len(send_buffer)} bytes: {send_buffer[:4]}...{send_buffer[-4:]}")

    conn.close()
    s.close

if __name__ == '__main__':
    tcpServer()