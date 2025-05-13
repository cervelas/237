import socket, os
from dotenv import load_dotenv


class UdpController:
    def __init__(self):
        load_dotenv()

        self.listen_ip = os.getenv("UDP_IP")
        self.port = os.getenv("UDP_PORT")
        self.port = int(self.port) if self.port else 5001

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((self.listen_ip, self.port))
        print(f"Listening for UDP packets on {self.listen_ip}:{self.port}")

    def listen(self):
        if self.sock:
            data, addr = self.sock.recvfrom(1024)
            return (data, addr)
        else:
            print("UDP connection not initialized")
            return None

    def write(self, message: str):
        self.sock.sendto(message.encode(), (self.listen_ip, self.port))
