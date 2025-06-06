import socket
import time
import struct

UDP_IP = "255.255.255.255"     # Broadcast address
UDP_PORT = 5000               # Must match Arduino port

BROADCAST_INTERVAL = 5.0       # Seconds

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

print(f"Broadcasting time to {UDP_IP}:{UDP_PORT} every {BROADCAST_INTERVAL}s...")

try:
    while True:
        # Get current time in milliseconds (only last 32 bits)
        current_time_ms = int(time.time() * 1000) & 0xFFFFFFFF
        data = struct.pack('>I', current_time_ms)  # 4-byte big-endian unsigned int
        sock.sendto(data, (UDP_IP, UDP_PORT))
        print(f"Sent timestamp: {current_time_ms}")
        time.sleep(BROADCAST_INTERVAL)
except KeyboardInterrupt:
    print("Broadcast stopped.")
    sock.close()
