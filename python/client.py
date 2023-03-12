# python socket_client.py
import socket
import struct
import os

HOST = "127.0.0.1"
PORT = 5000
send_buffer = bytearray(1024 * 1024)
send_buffer[:4] = b'abcd'
send_buffer[-4:] = b'dcba'

def send_msg(sock, msg):
    msg = struct.pack('>I', len(msg)) + msg
    sock.sendall(msg)

def recv_msg(sock):
    raw_msglen = recvall(sock, 4)
    if not raw_msglen:
        return None
    msglen = struct.unpack('>I', raw_msglen)[0]
    return recvall(sock, msglen)

def recvall(sock, n):
    data = bytearray()
    while len(data) < n:
        packet = sock.recv(n - len(data))
        if not packet:
            return None
        data.extend(packet)
    return data

def tcpClient():
    host = "127.0.0.1"
    port = 5000

    # connect with the server
    conn = socket.socket()
    conn.connect((HOST, PORT))

    while True:
        input()

        # send buffer
        send_msg(conn, send_buffer)
        print(f"send to server {len(send_buffer)} bytes: {send_buffer[:4]}...{send_buffer[-4:]}")

        # receive buffer
        recv_buffer = recv_msg(conn)
        if not recv_buffer:
            break
        print(f"receive from server {len(recv_buffer)} bytes: {recv_buffer[:4]} ... {recv_buffer[-4:]}")
    s.close()

if __name__ == '__main__':
    tcpClient()
