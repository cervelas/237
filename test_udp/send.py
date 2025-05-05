import socket

# Define the UDP IP address and port to listen on
UDP_IP = "localhost"
UDP_PORT = 5005

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

msg = "Hello World".encode()
sock.sendto(msg, ("127.0.0.1", UDP_PORT))
