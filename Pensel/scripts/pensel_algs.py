#! /usr/bin/env python3

from scipy import signal as sig
import numpy as np


class FIRFilter(object):
    def __init__(self, *, coefficients):
        self.coefficients = coefficients
        self.order = len(coefficients)
        self.buffer = np.zeros(self.order)
        self.ind = 0

    def run(self, new_val):
        out_val = 0
        self.buffer[0] = new_val
        for i in range(self.order - 1, 0, -1):
            if i != 0:
                self.buffer[i] = self.buffer[i - 1]
            out_val += self.coefficients[i] * self.buffer[i]
        self.ind += 1

        if self.ind >= self.order:
            return out_val
        else:
            return 0


class IIRFilter(object):
    def __init__(self, *, numerator, denominator, order):
        self.numerator = numerator
        self.denominator = denominator
        self.order = order
        self.buffer = np.zeros(self.order)
        self.ind = 0

    def run(self, new_val):
        # Move the new data in
        self.buffer[0] = new_val
        for i in range(self.order - 1, 0, -1):
            if i != 0:
                self.buffer[i] = self.buffer[i - 1]
        self.ind += 1

        if self.ind >= self.order:
            # Crank out the data if our buffer is full
            data = sig.lfilter(self.numerator, self.denominator, self.buffer)
            # print(data)
            return data[-1]
        else:
            return 0


class VectorFilter(object):
    def __init__(self, *args, ftype="FIR", **kwargs):
        if ftype == "FIR":
            self.x = FIRFilter(*args, **kwargs)
            self.y = FIRFilter(*args, **kwargs)
            self.z = FIRFilter(*args, **kwargs)
        elif ftype == "IIR":
            self.x = IIRFilter(*args, **kwargs)
            self.y = IIRFilter(*args, **kwargs)
            self.z = IIRFilter(*args, **kwargs)
        else:
            raise RuntimeError(f"ftype must be FIR or IIR. (given: {ftype})")


class Vector(object):
    def __init__(self):
        self.x = 0
        self.y = 0
        self.z = 0

    def __repr__(self):
        return "<Vector: {},{},{}>".format(self.x, self.y, self.z)


class VectorDetector(object):
    def __init__(self, *args, **kwargs):
        # self.order = len(coefficients)
        self.filters = VectorFilter(*args, **kwargs)

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
    def __init__(self, data_rate, lower_freq_reject, lower_freq_pass,
                 upper_freq_pass, upper_freq_reject, order=64, window='parzen'):
        self.data_rate = data_rate
        nyquist = 0.5 * data_rate  # noqa
        # lower_norm_reject = lower_freq_reject / nyquist
        # lower_norm_pass = lower_freq_pass / nyquist
        # upper_norm_reject = upper_freq_reject / nyquist
        upper_norm_pass = upper_freq_pass / nyquist

        self.velocity = Vector()
        self.distance = Vector()

        # get the coefficients and make da filters
        # coeffs = sig.firwin2(
        #     order, [0, lower_norm_reject, lower_norm_pass, upper_norm_pass, upper_norm_reject, 1],
        #     [1, 1, 1, 1, 0, 0], window=window)
        ripple = 5  # noqa [dB, no clue what this number really should be.]

        num, den = sig.iirfilter(order, upper_norm_pass, btype='lowpass', ftype='butter')

        # super()__init__(coefficients=coeffs, ftype="FIR")
        super().__init__(numerator=num, denominator=den, order=order, ftype="IIR")

    def calc_velocity(self, accel_pkt):
        """
        Given a band-pass filtered accel input packet, decide the current velocity.
        """
        def clamp_velocity(vel):
            _lower = 0.025
            _upper = 5
            _fact1 = 0.8
            _fact2 = 0.3

            sign = 1 if vel >= 0 else -1
            vel = abs(vel)
            if vel <= _lower:
                return 0
            elif vel > _lower and vel < _upper:
                return sign * (vel - _lower) * _fact1
            else:
                return sign * ((vel - _lower) * _fact2)

        def clamp_accel(acc):
            _lower = 2
            _upper = 50
            _fact1 = 0.8
            _fact2 = 0.3

            sign = 1 if acc >= 0 else -1
            acc = abs(acc)
            if acc <= _lower:
                return 0
            elif acc > _lower and acc < _upper:
                return sign * (acc - _lower) * _fact1
            else:
                return sign * ((acc - _lower) * _fact2)

        def decay_velocity(vel):
            """
            """
            decay_factor = 0.8
            sign = 1 if vel >= 0 else -1
            vel = abs(vel)
            if vel < 1:
                return vel * decay_factor * sign
            else:
                return vel * sign

        new_vel_x = accel_pkt.x * (1 / self.data_rate)
        new_vel_y = accel_pkt.y * (1 / self.data_rate)
        new_vel_z = accel_pkt.z * (1 / self.data_rate)

        self.velocity.x += clamp_velocity(new_vel_x)
        self.velocity.x = decay_velocity(self.velocity.x)
        self.velocity.y += clamp_velocity(new_vel_y)
        self.velocity.y = decay_velocity(self.velocity.y)
        self.velocity.z += clamp_velocity(new_vel_z)
        self.velocity.z = decay_velocity(self.velocity.z)

        return self.velocity

    def calc_distance(self, velocity_pkt):
        """
        Given a filtered velocity input packet, decide the accumilated distance.
        """
        new_vel_x = velocity_pkt.x * (1 / self.data_rate)
        new_vel_y = velocity_pkt.y * (1 / self.data_rate)
        new_vel_z = velocity_pkt.z * (1 / self.data_rate)

        self.distance.x += new_vel_x
        self.distance.y += new_vel_y
        self.distance.z += new_vel_z

        return self.distance


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
