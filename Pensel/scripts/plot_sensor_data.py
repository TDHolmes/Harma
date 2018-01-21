#! /usr/bin/env python3
import time
import struct
import matplotlib.pyplot as plt
import numpy as np

# from penselreport import Pensel
import pensel_utils as pu
import pensel_algs as pa

mag_offsets = [1111.877197265625, 1219.1551513671875, -951.0]


class LSM303DLHC_Parser(object):
    SAMPLES_PER_SECOND = 100  # conservative

    def __init__(self, port, baudrate, verbose=0):
        self.verbose = verbose
        self.port = port
        self.baudrate = baudrate

    def post_plot(self, num_samples=100, plot_mag=True, plot_accel=True, plot_accel_movement=True,
                  fft=False, nyquist_freq=None):
        """
        """
        # get the packets
        accel_packets = []
        mag_packets = []
        accel_done = False if (plot_accel or plot_accel_movement) else True
        mag_done = False if plot_mag else True

        end_time = (num_samples / self.SAMPLES_PER_SECOND) * 1.2 + time.time()

        with pu.Pensel(self.port, None, self.verbose) as pi:

            # turn on accel & mag streaming
            pi.log("Turning on input streaming...")
            # retval, response = pi.send_report(0x20, payload=[0xc])
            pi.log("Gathering packets...")
            try:
                while time.time() < end_time:
                    packet = pi.get_packet()
                    if packet:
                        report, retval, payload = packet
                        if report == 0x83 and retval == 0 and (plot_accel or plot_accel_movement):  # accel
                            packed_data = struct.pack("B" * len(payload), *payload)
                            pkt = pi.parse_accel_packet(packed_data)
                            accel_packets.append(pkt)
                            if self.verbose:
                                pi.log("Got Accel packet# {}".format(pkt.frame_num))

                        elif report == 0x84 and retval == 0 and plot_mag:  # mag
                            packed_data = struct.pack("B" * len(payload), *payload)
                            pkt = pi.parse_mag_packet(packed_data)
                            mag_packets.append(pkt)
                            if self.verbose:
                                pi.log("Got Mag packet# {}".format(pkt.frame_num))

                    if len(accel_packets) >= num_samples:
                        accel_done = True
                    if len(mag_packets) >= num_samples:
                        mag_done = True

                    # check if we're done!
                    if accel_done and mag_done:
                        pi.log("Done collecting data!")
                        break
            except KeyboardInterrupt:
                print("Keyboard interrupt! Swallowing...")

        if accel_done is False or mag_done is False:
            print("WARNING: Timed out before getting enough packets.")

        # Markers: S, 8, >, <, ^, v, o, X, P, d
        fig, ax = plt.subplots()

        if plot_accel:
            X_accel_axis = [i for i in range(len(accel_packets))]
            x_accel = [pkt.x for pkt in accel_packets]
            y_accel = [pkt.y for pkt in accel_packets]
            z_accel = [pkt.z for pkt in accel_packets]

            if fft:
                x_accel = np.fft.fft(x_accel)
                y_accel = np.fft.fft(y_accel)
                z_accel = np.fft.fft(z_accel)
                factor = nyquist_freq / len(X_accel_axis)
                X_accel_axis = [v * factor for v in X_accel_axis]

            ax.plot(X_accel_axis, x_accel, color="red", label='Accel: X')
            ax.plot(X_accel_axis, y_accel, color="green", label='Accel: Y')
            ax.plot(X_accel_axis, z_accel, color="blue", label='Accel: Z')

        if plot_accel_movement:
            movement = pa.MovementDetection(
                data_rate=220, lower_freq_reject=1, lower_freq_pass=2,
                upper_freq_pass=8, upper_freq_reject=10)

            X_accel_axis = [i for i in range(len(accel_packets))]
            movements = [movement.consume_packet(pkt) for pkt in accel_packets]
            x_accel = [pkt.x for pkt in movements]
            y_accel = [pkt.y for pkt in movements]
            z_accel = [pkt.z for pkt in movements]

            if fft:
                x_accel = np.fft.fft(x_accel)[:len(x_accel) // 2]
                y_accel = np.fft.fft(y_accel)[:len(y_accel) // 2]
                z_accel = np.fft.fft(z_accel)[:len(z_accel) // 2]
                X_accel_axis = X_accel_axis[:len(X_accel_axis) // 2]
                factor = nyquist_freq / len(X_accel_axis)
                X_accel_axis = [v * factor for v in X_accel_axis]

            ax.plot(X_accel_axis, x_accel, color="cyan", label='Accel Movment: X')
            ax.plot(X_accel_axis, y_accel, color="magenta", label='Accel Movment: Y')
            ax.plot(X_accel_axis, z_accel, color="black", label='Accel Movment: Z')

        if plot_mag:
            X_mag_axis = [i for i in range(len(mag_packets))]
            x_mag = [pkt.x for pkt in mag_packets]
            y_mag = [pkt.y for pkt in mag_packets]
            z_mag = [pkt.z for pkt in mag_packets]

            if fft:
                x_mag = np.fft.fft(x_mag)[:len(x_accel) // 2]
                y_mag = np.fft.fft(y_mag)[:len(y_accel) // 2]
                z_mag = np.fft.fft(z_mag)[:len(z_accel) // 2]
                X_mag_axis = X_mag_axis[:len(X_mag_axis) // 2]
                factor = nyquist_freq / len(X_mag_axis)
                X_mag_axis = [v * factor for v in X_mag_axis]

            ax.plot(X_mag_axis, x_mag, color="black", label='Mag: X')
            ax.plot(X_mag_axis, y_mag, color="magenta", label='Mag: Y')
            ax.plot(X_mag_axis, z_mag, color="cyan", label='Mag: Z')

        # plt.ion()
        ax.legend(loc='lower right')
        plt.show()


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Plot sensor data from the Pensel.')
    # parser.add_argument('file', type=str,
    #                     help='Filename to playback recorded data.')
    parser.add_argument('--mag', action="store_true",
                        help='Determine whether or not to plot mag.')
    parser.add_argument('--accel', action="store_true",
                        help='Determine whether or not to plot accel.')
    parser.add_argument('--accel-movement', action="store_true",
                        help='Determine whether or not to plot accel movement.')
    parser.add_argument('--fft', action="store_true", help='FFT plot of the data collected.')
    parser.add_argument("--nyquist", type=int, default=None, help="Nyquist frequency of the data given.")

    parser.add_argument('--verbose', action="store_true",
                        help='Verbose output toggle.')
    parser.add_argument('--samples', type=int, default=100,
                        help='number of samples to aquire before plotting')
    parser.add_argument('--baudrate', type=int, default=None,
                        help='baudrate of coms')

    args = parser.parse_args()

    if not args.accel and not args.mag and not args.accel_movement:
        print("Error: Must specify at least one plot option")
        raise SystemExit(-1)

    if args.fft and args.nyquist is None:
        print("Error: Can't specify FFT plotting without nyquist frequency")
        raise SystemExit(-2)

    lsm = LSM303DLHC_Parser("/dev/tty.SLAB_USBtoUART", args.baudrate, verbose=args.verbose)
    lsm.post_plot(plot_mag=args.mag, plot_accel=args.accel,
                  plot_accel_movement=args.accel_movement, num_samples=args.samples,
                  fft=args.fft, nyquist_freq=args.nyquist)
