import socket, os, struct
import rtmidi2
import time

PORT_NAME = "16Samples"

class UdpController:
    def __init__(self):

        self.listen_ip = "0.0.0.0"
        self.port = "5000"
        self.port = int(self.port) if self.port else 5001

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((self.listen_ip, self.port))
        print(f"Listening for UDP packets on {self.listen_ip}:{self.port}")

    def listen(self):
        if self.sock:
            data, addr = self.sock.recvfrom(4)
            return (data, addr)
        else:
            print("UDP connection not initialized")
            return None

    def write(self, message: str):
        self.sock.sendto(message.encode(), (self.listen_ip, self.port))

class MidiController:
    def __init__(self):
        self.midi_out = rtmidi2.MidiOut()
        self.midi_out.open_virtual_port(PORT_NAME)

    def clean(self):
        if self.midi_out:
            self.midi_out.close_port()

    def send_note_on_off(self, channel, note, velocity, duration):
        self.midi_out.send_noteon(channel - 1, note, velocity)
        print(f"Note On - Channel {channel}, Note {note}")
        
        time.sleep(duration)
        
        self.midi_out.send_noteoff(channel - 1, note)
        print(f"Note Off - Channel {channel}, Note {note}")

    def send_note(self, channel, note, velocity=127):
        self.midi_out.send_noteon(channel - 1, note, velocity)
        print(f"Note On - Channel {channel}, Note {note}")

    def send_cc(self, channel, cc, value):
        self.midi_out.send_cc(channel - 1, cc, value)
        print(f"CC {cc}, value {value}, Channel {channel}")

if __name__ == "__main__":
    midi_controller = MidiController()
    udp_controller = UdpController()

    while True:
        result = udp_controller.listen()
        if result:
            data, address = result
            type, channel, cc_or_note, value_velo = struct.unpack("BBBB", data)

            #print(f"Received from {address}: {" ".join("{:02x}".format(x) for x in data)}")
            print(f"{type}, {channel}, {cc_or_note}, {value_velo}")

            if type == 1:
                midi_controller.send_note(channel, cc_or_note, value_velo)

            if type == 2:
                midi_controller.send_cc(channel, cc_or_note, value_velo)

    midi_controller.clean()
