#! /usr/bin/env python3
import sys
import time
import struct
import curses

import pensel_utils as pu
import pensel_algs as algs

if sys.version_info.major == 2:
    input = raw_input  # noqa


class FilteringWindow(object):
    def __init__(self, *args, **kwargs):
        self.max_x = -10000
        self.min_x = 10000
        self.max_y = -10000
        self.min_y = 10000
        self.max_z = -10000
        self.min_z = 10000

    def run(self, vect, new_chr):
        if new_chr == ord("c"):
            self.max_x = -10000
            self.min_x = 10000
            self.max_y = -10000
            self.min_y = 10000
            self.max_z = -10000
            self.min_z = 10000

        # get min/max data
        if vect.x > self.max_x:
            self.max_x = vect.x
        if vect.x < self.min_x:
            self.min_x = vect.x

        if vect.y > self.max_y:
            self.max_y = vect.y
        if vect.y < self.min_y:
            self.min_y = vect.y

        if vect.z > self.max_z:
            self.max_z = vect.z
        if vect.z < self.min_z:
            self.min_z = vect.z


class RawStatsWindow(FilteringWindow):
    """
    Handles displaying the raw accel stats
    """
    def __init__(self, x, y, width, height):
        super().__init__()
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.window = curses.newwin(height, width, y, x)
        self.window.timeout(0)

    def run(self, pkt, new_chr=None):
        super().run(pkt, new_chr)

        self.window.erase()
        self.window.box()

        self.window.addstr(0,  3, "Accel Data:")
        self.window.addstr(1,  2, "F#:  {:>8}".format(pkt.frame_num))
        self.window.addstr(2,  2, "TS:  {:>8}".format(pkt.timestamp))
        self.window.hline(3,   1, "-", self.width - 2)
        self.window.addstr(4,  2, " X:  {:>8.3f}".format(pkt.x))
        self.window.addstr(5,  2, " Y:  {:>8.3f}".format(pkt.y))
        self.window.addstr(6,  2, " Z:  {:>8.3f}".format(pkt.z))
        self.window.hline(7,   1, "-", self.width - 2)
        self.window.addstr(8,  2, "ΔX:  {:>8.3f}".format(self.max_x - self.min_x))
        self.window.addstr(9,  2, "ΔY:  {:>8.3f}".format(self.max_y - self.min_y))
        self.window.addstr(10, 2, "ΔZ:  {:>8.3f}".format(self.max_z - self.min_z))

        self.window.redrawwin()  # complete redraw
        self.window.noutrefresh()


class GravityDetectWindow(FilteringWindow):
    """
    Handles displaying the raw accel stats
    """
    def __init__(self, x, y, width, height, filter_order=32, output_rate=220, cutoff_freq=5):
        super().__init__()
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.window = curses.newwin(height, width, y, x)
        self.window.timeout(0)

        self.detector = algs.AccelGravityVectorDetection(
            output_rate, cutoff_freq, stop_freq=cutoff_freq*2, order=filter_order)

    def run(self, pkt, new_chr=None):
        vect = self.detector.consume_packet(pkt)

        super().run(vect, new_chr)

        self.window.erase()
        self.window.box()
        self.window.addstr(0, 2, "Gravity Vector:")
        self.window.addstr(2, 2, " X:  {:>8.3f}".format(vect.x))
        self.window.addstr(3, 2, " Y:  {:>8.3f}".format(vect.y))
        self.window.addstr(4, 2, " Z:  {:>8.3f}".format(vect.z))
        self.window.addstr(6, 2, "ΔX:  {:>8.3f}".format(self.max_x - self.min_x))
        self.window.addstr(7, 2, "ΔY:  {:>8.3f}".format(self.max_y - self.min_y))
        self.window.addstr(8, 2, "ΔZ:  {:>8.3f}".format(self.max_z - self.min_z))
        # self.window.addstr(5, 2, "vect: {}".format(vect))

        self.window.redrawwin()  # complete redraw
        self.window.noutrefresh()


class MovementDetectWindow(FilteringWindow):
    """
    Handles displaying the raw accel stats
    """
    def __init__(self, x, y, width, height, filter_order=64, output_rate=220,
                 lower_freq=20, upper_freq=100):
        super().__init__()
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.window = curses.newwin(height, width, y, x)
        self.window.timeout(0)

        self.detector = algs.MovementDetection(
            output_rate, lower_freq, upper_freq, order=filter_order)

    def run(self, pkt, new_chr=None):
        vect = self.detector.consume_packet(pkt)

        super().run(vect, new_chr)

        self.window.erase()
        self.window.box()
        self.window.addstr(0, 4, "Movement:")
        self.window.addstr(2, 2, " X:  {:>8.3f}".format(vect.x))
        self.window.addstr(3, 2, " Y:  {:>8.3f}".format(vect.y))
        self.window.addstr(4, 2, " Z:  {:>8.3f}".format(vect.z))
        self.window.addstr(6, 2, "ΔX:  {:>8.3f}".format(self.max_x - self.min_x))
        self.window.addstr(7, 2, "ΔY:  {:>8.3f}".format(self.max_y - self.min_y))
        self.window.addstr(8, 2, "ΔZ:  {:>8.3f}".format(self.max_z - self.min_z))
        # self.window.addstr(5, 2, "vect: {}".format(vect))

        self.window.redrawwin()  # complete redraw
        self.window.noutrefresh()


def main(stdscr, port, baud, verbose, enable_moving_ave=True, num_averages=16):
    ACCEL_REPORT = 0x83
    # Clear screen
    stdscr.clear()
    stdscr.timeout(0)  # miliseconds

    begin_x = 5
    begin_y = 3
    height = 12
    width = 17
    raw_stats = RawStatsWindow(begin_x, begin_y, width, height)

    begin_x = 30
    begin_y = 3
    height = 10
    width = 18
    grav_detect = GravityDetectWindow(begin_x, begin_y, width, height)

    begin_x = 60
    begin_y = 3
    height = 10
    width = 18
    move_detect = MovementDetectWindow(begin_x, begin_y, width, height)
    moving_ave_x = algs.MovingAverage(num_averages=num_averages)
    moving_ave_y = algs.MovingAverage(num_averages=num_averages)
    moving_ave_z = algs.MovingAverage(num_averages=num_averages)

    with pu.Pensel(port, baud, verbose) as pi:
        while True:
            pkt = pi.get_packet()
            new_chr = stdscr.getch()
            if pkt:
                report, retval, payload = pkt
                if report != ACCEL_REPORT or retval != 0:
                    continue

                packed_data = struct.pack("B" * len(payload), *payload)
                pkt = pu.parse_accel_packet(packed_data)

                if enable_moving_ave:
                    pkt.x = moving_ave_x.run(pkt.x)
                    pkt.y = moving_ave_y.run(pkt.y)
                    pkt.z = moving_ave_z.run(pkt.z)

                raw_stats.run(pkt, new_chr)
                grav_detect.run(pkt, new_chr)
                move_detect.run(pkt, new_chr)

                stdscr.refresh()
            else:
                time.sleep(0.0001)


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Receive input data from the Pensel.')
    parser.add_argument('--baudrate', "-b", dest='baudrate', default=None,
                        help='Baudrate of the serial data to recieve')
    parser.add_argument('--verbose', default=False, action="store_true",
                        help='Whether or not we should print extra debug information')
    parser.add_argument('--moving-ave', default=False, action="store_true",
                        help='Whether or not to apply a simple moving average to data first')
    parser.add_argument('--num-averages', type=int, default=16,
                        help='Number of averages to perform [default: 16]')

    args = parser.parse_args()

    port = "/dev/cu.SLAB_USBtoUART"

    curses.wrapper(main, port, args.baudrate, args.verbose, args.moving_ave,
                   args.num_averages)
