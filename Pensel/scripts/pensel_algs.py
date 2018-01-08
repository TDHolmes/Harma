#! /usr/bin/env python3

from scipy import signal as sig
import numpy as np


class FIRFilter(object):
    def __init__(self, coefficients):
        self.coefficients = coefficients
        self.order = len(coefficients)
        self.buffer = np.zeros(self.order)

    def run(self, new_val):
        out_val = 0
        self.buffer[0] = new_val
        for i in range(self.order - 1, 0, -1):
            if i != 0:
                self.buffer[i] = self.buffer[i - 1]
            out_val += self.coefficients[i] * self.buffer[i]

        return out_val


class VectorFilter(object):
    def __init__(self, *args, **kwargs):
        self.x = FIRFilter(*args, **kwargs)
        self.y = FIRFilter(*args, **kwargs)
        self.z = FIRFilter(*args, **kwargs)


class Vector(object):
    def __init__(self):
        self.x = 0
        self.y = 0
        self.z = 0

    def __repr__(self):
        return "<Vector: {},{},{}>".format(self.x, self.y, self.z)


class VectorDetector(object):
    def __init__(self, coefficients):
        self.order = len(coefficients)
        self.filters = VectorFilter(coefficients)

    def consume_packet(self, new_packet):
        v = Vector()
        # calculate the new filtered values
        v.x = self.filters.x.run(new_packet.x)
        v.y = self.filters.y.run(new_packet.y)
        v.z = self.filters.z.run(new_packet.z)

        return v


class AccelGravityVectorDetection(VectorDetector):
    """
    """
    def __init__(self, data_rate, cutoff_freq, stop_freq=None, order=16, window='parzen'):
        self.data_rate = data_rate
        nyquist = 0.5 * data_rate
        cutoff_norm = cutoff_freq / nyquist

        # get the coefficients and make da filters
        if stop_freq:
            stop_norm = stop_freq / nyquist
            coeffs = sig.firwin2(order, [0, cutoff_norm, stop_norm, 1], [1, 1, 0, 0], window=window)
        else:
            coeffs = sig.firwin2(order, [0, cutoff_norm, 1], [1, 1, 0], window=window)

        super().__init__(coeffs)


class MovementDetection(VectorDetector):
    """
    """
    def __init__(self, data_rate, lower_freq, upper_freq, order=16, window='parzen'):
        self.data_rate = data_rate
        nyquist = 0.5 * data_rate
        lower_norm = lower_freq / nyquist
        upper_norm = upper_freq / nyquist

        # get the coefficients and make da filters
        coeffs = sig.firwin2(
            order, [0, lower_norm, upper_norm, 1], [0, 1, 1, 0], window=window)

        super().__init__(coeffs)


class MovingAverage(object):
    def __init__(self, num_averages):
        self.num_averages = num_averages
        self.ind = 0
        self.arr = np.zeros(num_averages)

    def run(self, new_val):
        self.arr[self.ind] = new_val
        self.ind += 1
        if self.ind >= self.num_averages:
            self.ind = 0

        return sum(self.arr) / self.num_averages


def test_FIR_impulse_response(verbose=True):
    coeffs = sig.firwin2(16, [0, 0.2, 1], [1, 1, 0], window="parzen")
    f = FIRFilter(coeffs)
    if verbose:
        print("Coefficients: {}".format(coeffs))

    out_vals = []
    for i in range(16):
        if i == 0:
            out_vals.append(f.run(1))
        else:
            out_vals.append(f.run(0))

    print("Coefficients: {}".format(coeffs))
    print("      Output: {}".format(out_vals))


def main():
    print("Unit tests!")
    test_FIR_impulse_response()


if __name__ == '__main__':
    main()
