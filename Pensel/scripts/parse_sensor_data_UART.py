import serial
import os
import sys

# Mag 0x14FE -0x0605 0x6A00
# Acc -0x0229 -0x28 0x038B


class LSM303DLHC_Parser(object):
    def __init__(self, serialport, baudrate, verbose=0):
        self.serialport = serialport
        self.baudrate = baudrate
        self.accel_packets = []
        self.mag_packets = []
        self.num_averages = 100
        self.verbose = verbose

        self.serial = serial.Serial(self.serialport, self.baudrate, timeout=1)

    def update_data(self, record_data=None):
        line = self.serial.readline()   # read a '\n' terminated line
        if record_data:
            with open(record_data, "a") as f:
                f.write(line)
        linesplit = line.split(" ")

        if linesplit[0] == "Mag":
            pkt = (int(linesplit[1], 16), int(linesplit[2], 16), int(linesplit[3], 16))
            if self.verbose == 2:
                print("New Mag Packet: {}".format(pkt))
            self.mag_packets.append(pkt)

        elif linesplit[0] == "Acc":
            pkt = (int(linesplit[1], 16), int(linesplit[2], 16), int(linesplit[3], 16))
            if self.verbose == 2:
                print("New Accel Packet: {}".format(pkt))
            self.accel_packets.append(pkt)

        else:
            if self.verbose == 1:
                print('Unrecognized line! "{}"'.format(line))

    def average_data(self):
        num_lines = 0
        if len(self.accel_packets) >= self.num_averages:
            array = [val[0] for val in self.accel_packets[-self.num_averages:]]
            ave_accel_x = float(sum(array) - min(array) - max(array)) / (self.num_averages - 2)
            array = [val[1] for val in self.accel_packets[-self.num_averages:]]
            ave_accel_y = float(sum(array) - min(array) - max(array)) / (self.num_averages - 2)
            array = [val[2] for val in self.accel_packets[-self.num_averages:]]
            ave_accel_z = float(sum(array) - min(array) - max(array)) / (self.num_averages - 2)

            print("Accel X: {: .3f}".format(ave_accel_x))
            print("Accel Y: {: .3f}".format(ave_accel_y))
            print("Accel Z: {: .3f}".format(ave_accel_z))
            print("\n")
            num_lines += 4

        if len(self.mag_packets) >= self.num_averages:
            array = [val[0] for val in self.mag_packets[-self.num_averages:]]
            ave_mag_x = float(sum(array) - min(array) - max(array)) / (self.num_averages - 2)
            array = [val[1] for val in self.mag_packets[-self.num_averages:]]
            ave_mag_y = float(sum(array) - min(array) - max(array)) / (self.num_averages - 2)
            array = [val[2] for val in self.mag_packets[-self.num_averages:]]
            ave_mag_z = float(sum(array) - min(array) - max(array)) / (self.num_averages - 2)

            print("  Mag X: {: .3f}".format(ave_mag_x))
            print("  Mag Y: {: .3f}".format(ave_mag_y))
            print("  Mag Z: {: .3f}".format(ave_mag_z))
            num_lines += 3
        sys.stdout.write("\033[{}A".format(num_lines + 1))
        sys.stdout.flush()

    def workloop(self, filename):
        while True:
            self.update_data(filename)
            self.average_data()


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Process data from the Pensel.')
    parser.add_argument('--baudrate', "-b", dest='baudrate', default=115200,
                        help='Baudrate of the serial data to recieve')
    parser.add_argument('--record', "-r", dest='filename', default=None, type=str,
                        help='Filename to optionally record the output to.')

    args = parser.parse_args()

    def find_ports():
        ports = []
        dev_dir = os.listdir("/dev/")
        for thing in dev_dir:
            if "cu." in thing:
                ports.append(thing)

        return ports

    def choose_port(list_of_ports):
        print("\nPorts:")
        for ind, port in enumerate(list_of_ports):
            print("\t{}: {}".format(ind, port))
        while True:
            try:
                choice = int(raw_input("Which port do you choose? "))
            except Exception:
                print("Invalid choice.")
                continue

            if choice < len(list_of_ports):
                return list_of_ports[choice]
            else:
                print("Invalid index.")

    port = "/dev/" + choose_port(find_ports())

    print("Attaching to {} at {} baud".format(port, args.baudrate))
    lsm = LSM303DLHC_Parser(port, args.baudrate)
    lsm.workloop(args.filename)
