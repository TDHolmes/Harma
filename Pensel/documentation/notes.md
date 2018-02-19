# Thoughts on UI / UX

The Pensel needs to be a multi-function utility that acquires the user's hand
orientation as well as "action" inputs for the user to indicate that something
should happen. (Active / inactive and an auxiliary action, like extrude solder)


# Power Requirements

*Interesting note:* LSM9DS1 forbids VDD from being up before VDD_IO. The minimum
allowed is synchronized boot up. Otherwise, a max of 10 ms delay between VDD_IO
and VDD.

## Digital requirements

### STM32 requirements

130 mA max

### LSM9DS1 Requirements

Doesn't specify. Assume worst case of reported 4.6 mA.

### Totals

130 mA + 4.6 mA = 134.6 mA
134.6 mA * 1.25 = *168.25 mA*


## Analog Requirements

clean 3.3v supply for LSM9DS1 and STM32F302x6/8 analog circuitry.

### LSM9DS1 requirements

600 uA for accel + mag in normal mode
4.0 mA for gyro in normal mode

= 4.6 mA

### STM32 requirements

350 uA max (worst case scenario at 105 deg. C)

### Totals

4.6 mA + 0.350 mA = 4.95 mA
4.95 mA * 1.25 = *6.19 mA*
