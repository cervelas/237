import socket, os
from dotenv import load_dotenv

load_dotenv()

# Define the UDP IP address and port to listen on
UDP_IP = os.getenv("UDP_IP")
UDP_PORT = os.getenv("UDP_PORT")
UDP_PORT = int(UDP_PORT) if UDP_PORT else 5001

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

msg = "Hello World".encode()
sock.sendto(msg, (UDP_IP, UDP_PORT))
