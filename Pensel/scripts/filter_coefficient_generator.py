import sys
from scipy import signal


def main(numtaps, cutoff=0.5):
    return signal.firwin(numtaps, cutoff, width=None, window='hamming')


if __name__ == '__main__':
    vals = main(15, [0.2, 0.3])
    sys.stdout.write("vals = {")
    for ind, v in enumerate(vals):
        sys.stdout.write("{}f".format(v))
        if ind != len(vals) - 1:
            if ind % 4 == 3:
                sys.stdout.write(",\n")
            else:
                sys.stdout.write(", ")

    sys.stdout.write("};\n")
    sys.stdout.flush()
