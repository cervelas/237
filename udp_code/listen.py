import socket, os
from dotenv import load_dotenv

load_dotenv()

# Define the UDP IP address and port to listen on
UDP_IP = os.getenv("UDP_IP")
UDP_PORT = os.getenv("UDP_PORT")
UDP_PORT = int(UDP_PORT) if UDP_PORT else 5001

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Listening for UDP packets on {UDP_IP}:{UDP_PORT}")

while True:
    # Receive data from the socket
    data, addr = sock.recvfrom(1024)
    print(f"Received packet from {addr}: {data.decode('utf-8')}")
