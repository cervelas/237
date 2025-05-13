import rtmidi2
import time

PORT_NAME = "Python MIDI Out"


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


if __name__ == "__main__":
    # Define notes and channels
    note1 = 60  # Middle C for Channel 1
    note2 = 62  # D for Channel 2
    velocity = 100
    duration = 0.5  # seconds
    
    controller = MidiController()

    for _ in range(5):
        # Send notes on Channel 1 and Channel 2
        controller.send_note_on_off(1, note1, velocity, duration)
        time.sleep(0.5)
        controller.send_note_on_off(2, note2, velocity, duration)

    controller.clean()

