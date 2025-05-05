import socket

# Define the UDP IP address and port to listen on
UDP_IP = "0.0.0.0"
UDP_PORT = 5005

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Listening for UDP packets on {UDP_IP}:{UDP_PORT}")

while True:
    # Receive data from the socket
    data, addr = sock.recvfrom(1024)
    print(f"Received packet from {addr}: {data.decode('utf-8')}")
