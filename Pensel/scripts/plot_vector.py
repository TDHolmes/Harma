#! /usr/bin/python
import os
import matplotlib.pyplot as plt

# required import, but unused :( stupid matplotlib
from mpl_toolkits.mplot3d import Axes3D

import penselreport
import pensel_utils as pu


plots_supported = {"accel": (0x22, 2), "mag": (0x23, 2)}


def plot_accel_or_mag(pensel, axis, plot_type):
    """
    Plotting accel and mag packets are identical..
    """
    axis.clear()
    # parse the args required to get the data
    report_and_args = plots_supported[plot_type]
    if len(report_and_args) == 1:
        payload = []
        reportID = report_and_args[0]
    else:
        payload = report_and_args[1:]
        reportID = report_and_args[0]

    # get the data
    retval, payload = pensel.send_report(reportID, payload)
    if retval == 0:
        # parse it
        frame_num, timestamp, x, y, z = pu.parse_accel_packet(payload)
        print("Plotting packet# {} ({} ms)\t\t({}, {}, {})".format(frame_num, timestamp, x, y, z))
        # plot it
        axis.quiver(0.5, 0.5, 0.5, x, y, z, length=0.1, normalize=True)
    else:
        raise penselreport.PenselError("Error! Retval: {}".format(retval))


def main(port, plot_type):
    with penselreport.Pensel(port, args.baudrate, args.verbose) as pensel:
        plt.ion()
        fig = plt.figure()
        ax = fig.gca(projection='3d')
        while True:
            if plot_type == "accel" or plot_type == "mag":
                plot_accel_or_mag(pensel, ax, plot_type)
            plt.show()
            plt.pause(0.0001)


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Visualize 3D vectors from the pensel.')
    parser.add_argument('plot_type', type=str,
                        help='What type of vector to plot. Can be: {}'.format(plots_supported.keys()))
    parser.add_argument('--baudrate', "-b", dest='baudrate', default=115200,
                        help='Baudrate of the serial data to recieve')
    parser.add_argument('--verbose', default=False, action="store_true",
                        help='Whether or not we should print extra debug information')

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

    # port = "/dev/" + choose_port(find_ports())
    port = "/dev/cu.SLAB_USBtoUART"
    main(port, args.plot_type)
