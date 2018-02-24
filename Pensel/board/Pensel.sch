EESchema Schematic File Version 2
LIBS:pensel_components
LIBS:74xgxx
LIBS:74xx
LIBS:ac-dc
LIBS:actel
LIBS:adc-dac
LIBS:allegro
LIBS:Altera
LIBS:analog_devices
LIBS:analog_switches
LIBS:atmel
LIBS:audio
LIBS:battery_management
LIBS:bbd
LIBS:bosch
LIBS:brooktre
LIBS:cmos_ieee
LIBS:cmos4000
LIBS:conn
LIBS:contrib
LIBS:cypress
LIBS:dc-dc
LIBS:device
LIBS:digital-audio
LIBS:diode
LIBS:display
LIBS:dsp
LIBS:elec-unifil
LIBS:ESD_Protection
LIBS:ftdi
LIBS:gennum
LIBS:graphic
LIBS:hc11
LIBS:intel
LIBS:interface
LIBS:ir
LIBS:Lattice
LIBS:linear
LIBS:logo
LIBS:maxim
LIBS:mechanical
LIBS:memory
LIBS:microchip_dspic33dsc
LIBS:microchip_pic10mcu
LIBS:microchip_pic12mcu
LIBS:microchip_pic16mcu
LIBS:microchip_pic18mcu
LIBS:microchip_pic32mcu
LIBS:microchip
LIBS:microcontrollers
LIBS:motor_drivers
LIBS:motorola
LIBS:motors
LIBS:msp430
LIBS:nordicsemi
LIBS:nxp_armmcu
LIBS:onsemi
LIBS:opto
LIBS:Oscillators
LIBS:philips
LIBS:Power_Management
LIBS:power
LIBS:powerint
LIBS:pspice
LIBS:references
LIBS:regul
LIBS:relays
LIBS:rfcom
LIBS:sensors
LIBS:silabs
LIBS:siliconi
LIBS:stm8
LIBS:stm32
LIBS:supertex
LIBS:switches
LIBS:texas
LIBS:transf
LIBS:transistors
LIBS:ttl_ieee
LIBS:valves
LIBS:video
LIBS:wiznet
LIBS:Worldsemi
LIBS:Xicor
LIBS:xilinx
LIBS:zetex
LIBS:Zilog
LIBS:LSM9DS1
LIBS:MF_Aesthetics
LIBS:MF_Connectors
LIBS:MF_Discrete_Semiconductor
LIBS:MF_Displays
LIBS:MF_Frequency_Control
LIBS:MF_IC_Analog
LIBS:MF_IC_Digital
LIBS:MF_IC_Power
LIBS:MF_LEDs
LIBS:MF_Passives
LIBS:MF_Sensors
LIBS:MF_Switches
LIBS:Pensel-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L USB_OTG P1
U 1 1 58C8D089
P 4900 1250
F 0 "P1" H 5050 1700 50  0000 C CNN
F 1 "USB_PROC" H 4900 1600 50  0000 C CNN
F 2 "TylerCustom:2013499-1" V 4850 1150 50  0001 C CNN
F 3 "" V 4850 1150 50  0000 C CNN
	1    4900 1250
	-1   0    0    -1  
$EndComp
$Comp
L SW_Push SW2
U 1 1 58C8D3DE
P 2900 7150
F 0 "SW2" H 2900 7435 50  0000 C CNN
F 1 "MAIN" H 2900 7344 50  0000 C CNN
F 2 "Buttons_Switches_SMD:SW_SPST_B3SL-1002P" H 2900 7350 50  0001 C CNN
F 3 "" H 2900 7350 50  0001 C CNN
	1    2900 7150
	1    0    0    -1  
$EndComp
$Comp
L SW_Push SW3
U 1 1 58C8D416
P 4050 7150
F 0 "SW3" H 4050 7435 50  0000 C CNN
F 1 "AUX" H 4050 7344 50  0000 C CNN
F 2 "Buttons_Switches_SMD:SW_SPST_B3SL-1002P" H 4050 7350 50  0001 C CNN
F 3 "" H 4050 7350 50  0001 C CNN
	1    4050 7150
	1    0    0    -1  
$EndComp
$Comp
L SW_SP3T SW4
U 1 1 58C8D445
P 5350 7250
F 0 "SW4" H 5200 7350 50  0000 C CNN
F 1 "MODE_SEL" H 5250 7050 50  0000 C CNN
F 2 "TylerCustom:CL-SB-13B-01" H 4725 7425 50  0001 C CNN
F 3 "" H 4725 7425 50  0001 C CNN
	1    5350 7250
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X05 J3
U 1 1 58C8D76F
P 9500 6050
F 0 "J3" H 9500 6465 50  0000 C CNN
F 1 "JTAG" H 9500 6374 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x05_Pitch1.27mm" H 9500 4850 50  0001 C CNN
F 3 "" H 9500 4850 50  0001 C CNN
	1    9500 6050
	1    0    0    -1  
$EndComp
$Comp
L LED D2
U 1 1 58C8D8C6
P 10200 4600
F 0 "D2" H 10100 4700 50  0000 C CNN
F 1 "LED0" H 10300 4700 50  0000 C CNN
F 2 "LEDs:LED_0402" H 10200 4600 50  0001 C CNN
F 3 "" H 10200 4600 50  0001 C CNN
	1    10200 4600
	1    0    0    -1  
$EndComp
$Comp
L LED D3
U 1 1 58C8D90A
P 10200 4900
F 0 "D3" H 10100 5000 50  0000 C CNN
F 1 "LED1" H 10300 5000 50  0000 C CNN
F 2 "LEDs:LED_0402" H 10200 4900 50  0001 C CNN
F 3 "" H 10200 4900 50  0001 C CNN
	1    10200 4900
	1    0    0    -1  
$EndComp
$Comp
L R R5
U 1 1 58C8D940
P 10550 4600
F 0 "R5" V 10450 4600 50  0000 C CNN
F 1 "330R" V 10550 4600 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 10480 4600 50  0001 C CNN
F 3 "" H 10550 4600 50  0001 C CNN
	1    10550 4600
	0    1    1    0   
$EndComp
$Comp
L R R6
U 1 1 58C8D998
P 10550 4900
F 0 "R6" V 10450 4900 50  0000 C CNN
F 1 "330R" V 10550 4900 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 10480 4900 50  0001 C CNN
F 3 "" H 10550 4900 50  0001 C CNN
	1    10550 4900
	0    1    1    0   
$EndComp
$Comp
L +3V3 #PWR28
U 1 1 58C8E248
P 1700 4950
F 0 "#PWR28" H 1700 4800 50  0001 C CNN
F 1 "+3V3" H 1800 5050 50  0000 C CNN
F 2 "" H 1700 4950 50  0001 C CNN
F 3 "" H 1700 4950 50  0001 C CNN
	1    1700 4950
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR31
U 1 1 58C8E2CA
P 900 5050
F 0 "#PWR31" H 900 4900 50  0001 C CNN
F 1 "+5V" H 915 5223 50  0000 C CNN
F 2 "" H 900 5050 50  0001 C CNN
F 3 "" H 900 5050 50  0001 C CNN
	1    900  5050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR33
U 1 1 58C8E340
P 1350 5650
F 0 "#PWR33" H 1350 5400 50  0001 C CNN
F 1 "GND" H 1355 5477 50  0001 C CNN
F 2 "" H 1350 5650 50  0001 C CNN
F 3 "" H 1350 5650 50  0001 C CNN
	1    1350 5650
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR6
U 1 1 58C8E65E
P 1350 1050
F 0 "#PWR6" H 1350 900 50  0001 C CNN
F 1 "+3V3" V 1350 1250 50  0000 C CNN
F 2 "" H 1350 1050 50  0001 C CNN
F 3 "" H 1350 1050 50  0001 C CNN
	1    1350 1050
	1    0    0    -1  
$EndComp
NoConn ~ 4600 1450
$Comp
L +5V #PWR2
U 1 1 58C8E8C2
P 4550 950
F 0 "#PWR2" H 4550 800 50  0001 C CNN
F 1 "+5V" H 4600 1100 50  0000 C CNN
F 2 "" H 4550 950 50  0001 C CNN
F 3 "" H 4550 950 50  0001 C CNN
	1    4550 950 
	-1   0    0    -1  
$EndComp
$Comp
L STM32F302x6/8 U1
U 1 1 58C8C717
P 2300 1800
F 0 "U1" H 2300 2750 60  0000 C CNN
F 1 "STM32F302x6/8" H 2300 1850 60  0000 C CNB
F 2 "Housings_DFN_QFN:QFN-32-1EP_5x5mm_Pitch0.5mm" H 2250 2150 60  0001 C CNN
F 3 "" H 2250 2150 60  0001 C CNN
	1    2300 1800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR11
U 1 1 58C8F21E
P 4900 1700
F 0 "#PWR11" H 4900 1450 50  0001 C CNN
F 1 "GND" H 4905 1527 50  0001 C CNN
F 2 "" H 4900 1700 50  0001 C CNN
F 3 "" H 4900 1700 50  0001 C CNN
	1    4900 1700
	-1   0    0    -1  
$EndComp
Text GLabel 1100 1500 0    51   BiDi ~ 0
SDA
Text GLabel 700  1600 0    51   Output ~ 0
SCL
$Comp
L R R2
U 1 1 58C907EC
P 750 1450
F 0 "R2" H 600 1500 50  0000 L CNN
F 1 "4K7" V 750 1450 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 680 1450 50  0001 C CNN
F 3 "" H 750 1450 50  0001 C CNN
	1    750  1450
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 58C90838
P 1150 1350
F 0 "R1" H 1000 1400 50  0000 L CNN
F 1 "4K7" V 1150 1350 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 1080 1350 50  0001 C CNN
F 3 "" H 1150 1350 50  0001 C CNN
	1    1150 1350
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR7
U 1 1 58C90A46
P 950 1200
F 0 "#PWR7" H 950 1050 50  0001 C CNN
F 1 "+3V3" V 950 1400 50  0000 C CNN
F 2 "" H 950 1200 50  0001 C CNN
F 3 "" H 950 1200 50  0001 C CNN
	1    950  1200
	1    0    0    -1  
$EndComp
Text GLabel 3250 1250 2    51   BiDi ~ 0
SWDIO
Text GLabel 3400 1950 2    51   BiDi ~ 0
LED0
Text GLabel 9800 5850 2    51   BiDi ~ 0
SWDIO
$Comp
L +3V3 #PWR27
U 1 1 58C929E0
P 10800 4550
F 0 "#PWR27" H 10800 4400 50  0001 C CNN
F 1 "+3V3" H 10815 4723 50  0000 C CNN
F 2 "" H 10800 4550 50  0001 C CNN
F 3 "" H 10800 4550 50  0001 C CNN
	1    10800 4550
	1    0    0    -1  
$EndComp
Text GLabel 10000 4900 0    51   BiDi ~ 0
LED1
Text GLabel 10000 4600 0    51   BiDi ~ 0
LED0
Text GLabel 3250 1150 2    51   BiDi ~ 0
SWD-SWCLK
Text GLabel 3400 2150 2    51   BiDi ~ 0
BTN_AUX
Text GLabel 9800 6050 2    51   BiDi ~ 0
SWD-SWO
Text GLabel 3400 1850 2    51   BiDi ~ 0
LED1
Text GLabel 3400 2050 2    51   Input ~ 0
BTN_MAIN
Text GLabel 3400 2450 2    51   BiDi ~ 0
MODE_0
Text GLabel 3400 2350 2    51   BiDi ~ 0
MODE_1
Text GLabel 3400 2250 2    51   BiDi ~ 0
MODE_2
Text GLabel 9800 5950 2    51   BiDi ~ 0
SWD-SWCLK
Text GLabel 9800 6250 2    60   Input ~ 0
RESET_n
Text GLabel 3250 7150 2    51   Output ~ 0
BTN_MAIN
Text GLabel 4450 7150 2    51   Output ~ 0
BTN_AUX
Text GLabel 5700 7350 2    51   BiDi ~ 0
MODE_2
Text GLabel 6150 7250 2    51   BiDi ~ 0
MODE_1
Text GLabel 5650 7150 2    51   BiDi ~ 0
MODE_0
Text GLabel 2150 2950 3    51   BiDi ~ 0
SWD-SWO
NoConn ~ 9750 6150
NoConn ~ 9250 6150
$Comp
L +3V3 #PWR43
U 1 1 58C98219
P 5800 6850
F 0 "#PWR43" H 5800 6700 50  0001 C CNN
F 1 "+3V3" H 5815 7023 50  0000 C CNN
F 2 "" H 5800 6850 50  0001 C CNN
F 3 "" H 5800 6850 50  0001 C CNN
	1    5800 6850
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR47
U 1 1 58C98367
P 5850 7650
F 0 "#PWR47" H 5850 7500 50  0001 C CNN
F 1 "+3V3" H 6000 7750 50  0000 C CNN
F 2 "" H 5850 7650 50  0001 C CNN
F 3 "" H 5850 7650 50  0001 C CNN
	1    5850 7650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR46
U 1 1 58C9852B
P 5100 7250
F 0 "#PWR46" H 5100 7000 50  0001 C CNN
F 1 "GND" H 5105 7077 50  0001 C CNN
F 2 "" H 5100 7250 50  0001 C CNN
F 3 "" H 5100 7250 50  0001 C CNN
	1    5100 7250
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR41
U 1 1 58C98DC6
P 3150 6800
F 0 "#PWR41" H 3150 6650 50  0001 C CNN
F 1 "+3V3" H 3300 6850 50  0000 C CNN
F 2 "" H 3150 6800 50  0001 C CNN
F 3 "" H 3150 6800 50  0001 C CNN
	1    3150 6800
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR42
U 1 1 58C98E45
P 4300 6800
F 0 "#PWR42" H 4300 6650 50  0001 C CNN
F 1 "+3V3" H 4450 6850 50  0000 C CNN
F 2 "" H 4300 6800 50  0001 C CNN
F 3 "" H 4300 6800 50  0001 C CNN
	1    4300 6800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR45
U 1 1 58C99280
P 3800 7150
F 0 "#PWR45" H 3800 6900 50  0001 C CNN
F 1 "GND" H 3805 6977 50  0001 C CNN
F 2 "" H 3800 7150 50  0001 C CNN
F 3 "" H 3800 7150 50  0001 C CNN
	1    3800 7150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR44
U 1 1 58C992DC
P 2650 7150
F 0 "#PWR44" H 2650 6900 50  0001 C CNN
F 1 "GND" H 2655 6977 50  0001 C CNN
F 2 "" H 2650 7150 50  0001 C CNN
F 3 "" H 2650 7150 50  0001 C CNN
	1    2650 7150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR23
U 1 1 58CA455E
P 1350 3050
F 0 "#PWR23" H 1350 2800 50  0001 C CNN
F 1 "GND" H 900 3100 50  0001 C CNN
F 2 "" H 1350 3050 50  0001 C CNN
F 3 "" H 1350 3050 50  0001 C CNN
	1    1350 3050
	1    0    0    -1  
$EndComp
Text Notes 600  3450 0    39   ~ 0
nBOOT1  BOOT0      Boot Mode\n  x        0      Main Flash memory\n  1        1      System memory \n  0        1      Embedded SRAM
$Comp
L C C5
U 1 1 58CA7843
P 4050 2800
F 0 "C5" H 4050 2900 50  0000 L CNN
F 1 "4.7 uF" V 4000 2550 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 4088 2650 50  0001 C CNN
F 3 "" H 4050 2800 50  0001 C CNN
	1    4050 2800
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 58CA78D8
P 5050 2800
F 0 "C6" H 5050 2900 50  0000 L CNN
F 1 "0.1 uF" V 5000 2550 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 5088 2650 50  0001 C CNN
F 3 "" H 5050 2800 50  0001 C CNN
	1    5050 2800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR25
U 1 1 58CA7A83
P 4300 3100
F 0 "#PWR25" H 4300 2850 50  0001 C CNN
F 1 "GND" H 4305 2927 50  0001 C CNN
F 2 "" H 4300 3100 50  0001 C CNN
F 3 "" H 4300 3100 50  0001 C CNN
	1    4300 3100
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR18
U 1 1 58CA7B03
P 4300 2600
F 0 "#PWR18" H 4300 2450 50  0001 C CNN
F 1 "+3V3" H 4150 2700 50  0000 C CNN
F 2 "" H 4300 2600 50  0001 C CNN
F 3 "" H 4300 2600 50  0001 C CNN
	1    4300 2600
	1    0    0    -1  
$EndComp
$Comp
L R R10
U 1 1 58CA540C
P 1100 2250
F 0 "R10" V 1000 2250 50  0000 L CNN
F 1 "4K7" V 1100 2250 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 1030 2250 50  0001 C CNN
F 3 "" H 1100 2250 50  0001 C CNN
	1    1100 2250
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR14
U 1 1 58CAB83A
P 1100 2050
F 0 "#PWR14" H 1100 1900 50  0001 C CNN
F 1 "+3V3" H 950 2150 50  0000 C CNN
F 2 "" H 1100 2050 50  0001 C CNN
F 3 "" H 1100 2050 50  0001 C CNN
	1    1100 2050
	1    0    0    -1  
$EndComp
Text GLabel 10750 6150 0    51   BiDi ~ 0
SDA
Text GLabel 10750 6250 0    51   Output ~ 0
SCL
Text Notes 1200 4050 0    39   ~ 8
Digital Supply
$Comp
L R R11
U 1 1 58CAF070
P 3150 7000
F 0 "R11" H 3250 7050 50  0000 L CNN
F 1 "4K7" V 3150 7000 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 3080 7000 50  0001 C CNN
F 3 "" H 3150 7000 50  0001 C CNN
	1    3150 7000
	1    0    0    -1  
$EndComp
$Comp
L R R12
U 1 1 58CAF10F
P 4300 7000
F 0 "R12" H 4400 7050 50  0000 L CNN
F 1 "4K7" V 4300 7000 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 4230 7000 50  0001 C CNN
F 3 "" H 4300 7000 50  0001 C CNN
	1    4300 7000
	1    0    0    -1  
$EndComp
$Comp
L R R13
U 1 1 58CAF1AE
P 5600 7000
F 0 "R13" H 5700 7050 50  0000 L CNN
F 1 "4K7" V 5600 7000 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 5530 7000 50  0001 C CNN
F 3 "" H 5600 7000 50  0001 C CNN
	1    5600 7000
	1    0    0    -1  
$EndComp
$Comp
L R R14
U 1 1 58CAF254
P 6100 7050
F 0 "R14" H 6200 7100 50  0000 L CNN
F 1 "4K7" V 6100 7050 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 6030 7050 50  0001 C CNN
F 3 "" H 6100 7050 50  0001 C CNN
	1    6100 7050
	1    0    0    -1  
$EndComp
$Comp
L R R15
U 1 1 58CAF2FB
P 5650 7500
F 0 "R15" H 5750 7550 50  0000 L CNN
F 1 "4K7" V 5650 7500 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 5580 7500 50  0001 C CNN
F 3 "" H 5650 7500 50  0001 C CNN
	1    5650 7500
	1    0    0    -1  
$EndComp
$Comp
L C C10
U 1 1 58CE08B2
P 1700 5250
F 0 "C10" H 1700 5350 50  0000 L CNN
F 1 "1 uF" V 1750 5000 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 1738 5100 50  0001 C CNN
F 3 "" H 1700 5250 50  0001 C CNN
	1    1700 5250
	1    0    0    -1  
$EndComp
$Comp
L C C11
U 1 1 58CE099C
P 900 5350
F 0 "C11" H 750 5450 50  0000 L CNN
F 1 "1 uF" V 850 5100 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 938 5200 50  0001 C CNN
F 3 "" H 900 5350 50  0001 C CNN
	1    900  5350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR16
U 1 1 58C8E696
P 1250 2300
F 0 "#PWR16" H 1250 2050 50  0001 C CNN
F 1 "GND" H 800 2350 50  0001 C CNN
F 2 "" H 1250 2300 50  0001 C CNN
F 3 "" H 1250 2300 50  0001 C CNN
	1    1250 2300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR40
U 1 1 58CE1ED0
P 10750 6350
F 0 "#PWR40" H 10750 6100 50  0001 C CNN
F 1 "GND" H 10300 6400 50  0001 C CNN
F 2 "" H 10750 6350 50  0001 C CNN
F 3 "" H 10750 6350 50  0001 C CNN
	1    10750 6350
	1    0    0    -1  
$EndComp
NoConn ~ 2250 2850
NoConn ~ 2350 2850
$Comp
L TEST_1P J1
U 1 1 58E08122
P 4000 1750
F 0 "J1" H 4050 1800 50  0000 L CNN
F 1 "TEST_1P" H 3900 1700 50  0000 L CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 4200 1750 50  0001 C CNN
F 3 "" H 4200 1750 50  0001 C CNN
	1    4000 1750
	1    0    0    -1  
$EndComp
$Comp
L TEST_1P J7
U 1 1 58E08B4D
P 8650 4650
F 0 "J7" H 8708 4770 50  0000 L CNN
F 1 "Fiducial" V 8700 4400 50  0000 L CNN
F 2 "Fiducials:Fiducial_1mm_Dia_2.54mm_Outer_CopperTop" H 8850 4650 50  0001 C CNN
F 3 "" H 8850 4650 50  0001 C CNN
	1    8650 4650
	1    0    0    -1  
$EndComp
$Comp
L TEST_1P J6
U 1 1 58E08E2A
P 8450 4650
F 0 "J6" H 8508 4770 50  0000 L CNN
F 1 "Fiducial" V 8500 4400 50  0000 L CNN
F 2 "Fiducials:Fiducial_1mm_Dia_2.54mm_Outer_CopperTop" H 8650 4650 50  0001 C CNN
F 3 "" H 8650 4650 50  0001 C CNN
	1    8450 4650
	1    0    0    -1  
$EndComp
NoConn ~ 8650 4650
NoConn ~ 8450 4650
Text Notes 8550 4450 0    60   ~ 0
top\n
Text GLabel 2450 2950 3    51   Input ~ 0
BREV-0
Text GLabel 2550 2950 3    51   Input ~ 0
BREV-1
Text GLabel 8100 5900 0    51   Input ~ 0
BREV-0
Text GLabel 7450 5900 0    51   Input ~ 0
BREV-1
$Comp
L GND #PWR37
U 1 1 5A135ACE
P 7550 6300
F 0 "#PWR37" H 7550 6050 50  0001 C CNN
F 1 "GND" H 7555 6127 50  0001 C CNN
F 2 "" H 7550 6300 50  0001 C CNN
F 3 "" H 7550 6300 50  0001 C CNN
	1    7550 6300
	1    0    0    -1  
$EndComp
$Comp
L R R8
U 1 1 5A135E99
P 7550 6100
F 0 "R8" H 7650 6100 50  0000 L CNN
F 1 "4K7" V 7550 6100 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 7480 6100 50  0001 C CNN
F 3 "" H 7550 6100 50  0001 C CNN
	1    7550 6100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR38
U 1 1 5A135FE2
P 8200 6300
F 0 "#PWR38" H 8200 6050 50  0001 C CNN
F 1 "GND" H 8205 6127 50  0001 C CNN
F 2 "" H 8200 6300 50  0001 C CNN
F 3 "" H 8200 6300 50  0001 C CNN
	1    8200 6300
	1    0    0    -1  
$EndComp
$Comp
L R R9
U 1 1 5A135FEE
P 8200 6100
F 0 "R9" H 8300 6100 50  0000 L CNN
F 1 "4K7" V 8200 6100 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 8130 6100 50  0001 C CNN
F 3 "" H 8200 6100 50  0001 C CNN
	1    8200 6100
	1    0    0    -1  
$EndComp
Text Notes 7600 5200 0    51   ~ 10
Board Rev ID
Text GLabel 3250 1550 2    51   Input ~ 0
UART_RX
Text GLabel 3250 1650 2    51   Input ~ 0
UART_TX
Text GLabel 10750 5950 0    51   Input ~ 0
UART_TX
Text GLabel 10750 6050 0    51   Input ~ 0
UART_RX
$Comp
L C C_NOSTUFF2
U 1 1 5A7F81E3
P 4300 1550
F 0 "C_NOSTUFF2" V 4450 1300 50  0000 L CNN
F 1 "0.1 uF" H 4050 1650 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 4338 1400 50  0001 C CNN
F 3 "" H 4300 1550 50  0001 C CNN
	1    4300 1550
	1    0    0    -1  
$EndComp
$Comp
L C C_NOSTUFF1
U 1 1 5A7F8285
P 4300 1050
F 0 "C_NOSTUFF1" V 4150 1000 50  0000 L CNN
F 1 "0.1 uF" H 4100 950 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 4338 900 50  0001 C CNN
F 3 "" H 4300 1050 50  0001 C CNN
	1    4300 1050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR12
U 1 1 5A7F8C45
P 4300 1750
F 0 "#PWR12" H 4300 1500 50  0001 C CNN
F 1 "GND" H 4305 1577 50  0001 C CNN
F 2 "" H 4300 1750 50  0001 C CNN
F 3 "" H 4300 1750 50  0001 C CNN
	1    4300 1750
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR1
U 1 1 5A7F8CCB
P 4300 850
F 0 "#PWR1" H 4300 600 50  0001 C CNN
F 1 "GND" H 4305 677 50  0001 C CNN
F 2 "" H 4300 850 50  0001 C CNN
F 3 "" H 4300 850 50  0001 C CNN
	1    4300 850 
	0    1    -1   0   
$EndComp
$Comp
L +5V #PWR29
U 1 1 5A7FA8DD
P 2300 4950
F 0 "#PWR29" H 2300 4800 50  0001 C CNN
F 1 "+5V" H 2400 5050 50  0000 C CNN
F 2 "" H 2300 4950 50  0001 C CNN
F 3 "" H 2300 4950 50  0001 C CNN
	1    2300 4950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR34
U 1 1 5A7FA8E3
P 3050 5700
F 0 "#PWR34" H 3050 5450 50  0001 C CNN
F 1 "GND" H 3055 5527 50  0001 C CNN
F 2 "" H 3050 5700 50  0001 C CNN
F 3 "" H 3050 5700 50  0001 C CNN
	1    3050 5700
	1    0    0    -1  
$EndComp
$Comp
L C C9
U 1 1 5A7FA8EF
P 3450 5250
F 0 "C9" H 3450 5350 50  0000 L CNN
F 1 "1 uF" V 3500 5000 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 3488 5100 50  0001 C CNN
F 3 "" H 3450 5250 50  0001 C CNN
	1    3450 5250
	1    0    0    -1  
$EndComp
$Comp
L C C8
U 1 1 5A7FA8F5
P 2300 5250
F 0 "C8" H 2150 5350 50  0000 L CNN
F 1 "1 uF" V 2250 5000 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 2338 5100 50  0001 C CNN
F 3 "" H 2300 5250 50  0001 C CNN
	1    2300 5250
	1    0    0    -1  
$EndComp
Text Notes 2900 4050 0    39   ~ 8
Analog Supply
$Comp
L +3.3VA #PWR30
U 1 1 5A7FAD85
P 3450 4950
F 0 "#PWR30" H 3450 4800 50  0001 C CNN
F 1 "+3.3VA" H 3600 5050 50  0000 C CNN
F 2 "" H 3450 4950 50  0001 C CNN
F 3 "" H 3450 4950 50  0001 C CNN
	1    3450 4950
	1    0    0    -1  
$EndComp
$Comp
L +3.3VA #PWR8
U 1 1 5A7FB38D
P 8100 1250
F 0 "#PWR8" H 8100 1100 50  0001 C CNN
F 1 "+3.3VA" H 7900 1300 50  0000 C CNN
F 2 "" H 8100 1250 50  0001 C CNN
F 3 "" H 8100 1250 50  0001 C CNN
	1    8100 1250
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR3
U 1 1 5A7FB5B4
P 8450 950
F 0 "#PWR3" H 8450 800 50  0001 C CNN
F 1 "+3V3" H 8500 1100 50  0000 C CNN
F 2 "" H 8450 950 50  0001 C CNN
F 3 "" H 8450 950 50  0001 C CNN
	1    8450 950 
	1    0    0    -1  
$EndComp
Text GLabel 8400 1750 0    51   Output ~ 0
SCL
Text GLabel 8400 1850 0    51   BiDi ~ 0
SDA
$Comp
L GND #PWR21
U 1 1 5A7FC65B
P 8400 2700
F 0 "#PWR21" H 8400 2450 50  0001 C CNN
F 1 "GND" H 8405 2527 50  0001 C CNN
F 2 "" H 8400 2700 50  0001 C CNN
F 3 "" H 8400 2700 50  0001 C CNN
	1    8400 2700
	-1   0    0    -1  
$EndComp
Text GLabel 10300 2000 2    51   Output ~ 0
AG_INT1
Text GLabel 10300 2100 2    51   Output ~ 0
AG_INT2
$Comp
L C C4
U 1 1 5A7FD40D
P 10250 2800
F 0 "C4" H 10250 2900 50  0000 L CNN
F 1 "0.1 uF" V 10200 2550 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 10288 2650 50  0001 C CNN
F 3 "" H 10250 2800 50  0001 C CNN
	1    10250 2800
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 5A7FD78B
P 10500 2650
F 0 "C3" H 10500 2750 50  0000 L CNN
F 1 "10 nF" V 10550 2400 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 10538 2500 50  0001 C CNN
F 3 "" H 10500 2650 50  0001 C CNN
F 4 "16v cap or more (must gurentee 1 nF under 11v bias)" H 10500 2650 60  0001 C CNN "Notes"
	1    10500 2650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR22
U 1 1 5A7FD8C1
P 10400 2950
F 0 "#PWR22" H 10400 2700 50  0001 C CNN
F 1 "GND" H 10405 2777 50  0001 C CNN
F 2 "" H 10400 2950 50  0001 C CNN
F 3 "" H 10400 2950 50  0001 C CNN
	1    10400 2950
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR24
U 1 1 5A7FE088
P 9300 3050
F 0 "#PWR24" H 9300 2800 50  0001 C CNN
F 1 "GND" H 9305 2877 50  0001 C CNN
F 2 "" H 9300 3050 50  0001 C CNN
F 3 "" H 9300 3050 50  0001 C CNN
	1    9300 3050
	-1   0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 5A7FEF7A
P 8300 1400
F 0 "C2" H 8300 1500 50  0000 L CNN
F 1 "0.1 uF" V 8250 1150 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 8338 1250 50  0001 C CNN
F 3 "" H 8300 1400 50  0001 C CNN
	1    8300 1400
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 5A7FF315
P 8100 1400
F 0 "C1" H 7950 1500 50  0000 L CNN
F 1 "10 uF" V 8050 1150 39  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 8138 1250 50  0001 C CNN
F 3 "" H 8100 1400 50  0001 C CNN
	1    8100 1400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR10
U 1 1 5A7FF3C9
P 7950 1650
F 0 "#PWR10" H 7950 1400 50  0001 C CNN
F 1 "GND" H 7955 1477 50  0001 C CNN
F 2 "" H 7950 1650 50  0001 C CNN
F 3 "" H 7950 1650 50  0001 C CNN
	1    7950 1650
	0    1    -1   0   
$EndComp
$Comp
L LSM9DS1 U2
U 1 1 5A7F95E5
P 9300 1850
F 0 "U2" H 9350 2930 51  0000 C CNN
F 1 "LSM9DS1" H 9350 2831 59  0000 C CNB
F 2 "LSM9DS1:PQFN24P43_350X300X102L35X23N" H 9300 1850 51  0001 C CNN
F 3 "" H 9300 1850 51  0001 C CNN
	1    9300 1850
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR15
U 1 1 5A8017E5
P 8050 2050
F 0 "#PWR15" H 8050 1900 50  0001 C CNN
F 1 "+3V3" V 7950 2100 50  0000 C CNN
F 2 "" H 8050 2050 50  0001 C CNN
F 3 "" H 8050 2050 50  0001 C CNN
	1    8050 2050
	1    0    0    -1  
$EndComp
Text Notes 7550 2400 0    39   ~ 0
In I2C mode, \nthese are the \nLSB of the Accel/\ngyro and mag\naddress
$Comp
L +3V3 #PWR17
U 1 1 5A802071
P 8400 2350
F 0 "#PWR17" H 8400 2200 50  0001 C CNN
F 1 "+3V3" V 8300 2400 50  0000 C CNN
F 2 "" H 8400 2350 50  0001 C CNN
F 3 "" H 8400 2350 50  0001 C CNN
	1    8400 2350
	0    -1   -1   0   
$EndComp
Text Notes 8000 2700 0    39   ~ 0
To select I2C\nmode, tie CS\nhigh
Text GLabel 10300 1100 2    51   Output ~ 0
M_DRDY
$Comp
L +3V3 #PWR9
U 1 1 5A803FB2
P 10300 1250
F 0 "#PWR9" H 10300 1100 50  0001 C CNN
F 1 "+3V3" V 10400 1300 50  0000 C CNN
F 2 "" H 10300 1250 50  0001 C CNN
F 3 "" H 10300 1250 50  0001 C CNN
	1    10300 1250
	0    1    1    0   
$EndComp
NoConn ~ 10200 1750
$Comp
L +3.3VA #PWR5
U 1 1 5A8A45C3
P 1200 1050
F 0 "#PWR5" H 1200 900 50  0001 C CNN
F 1 "+3.3VA" V 1200 1300 50  0000 C CNN
F 2 "" H 1200 1050 50  0001 C CNN
F 3 "" H 1200 1050 50  0001 C CNN
	1    1200 1050
	1    0    0    -1  
$EndComp
Text Notes 2600 4750 0    39   ~ 0
- LSM9DS1 requirements\n600 uA for accel + mag in normal mode\n4.0 mA for gyro in normal mode\n= 4.6 mA\n\n- STM32 requirements\n350 uA max (worst case scenario at 105 deg. C)\n\n4.6 mA + 0.350 mA = 4.95 mA\n4.95 mA * 1.25 = *6.19 mA*
Text Notes 850  4700 0    39   ~ 0
- STM32 requirements\n130 mA max\n\n- LSM9DS1 Requirements\nDoesn’t specify. Assume \nworst case of reported 4.6 mA.\n\n130 mA + 4.6 mA = 134.6 mA\n134.6 mA * 1.25 = *168.25 mA*
$Comp
L NCP508 U4
U 1 1 5A8A693E
P 3050 5200
F 0 "U4" H 3050 5591 39  0000 C CNN
F 1 "NCP508" H 3050 5516 39  0000 C CIB
F 2 "TO_SOT_Packages_SMD:SC-70-5" H 3050 5200 39  0001 C CNN
F 3 "" H 3050 5200 39  0001 C CNN
	1    3050 5200
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR32
U 1 1 5A8A732B
P 2450 5400
F 0 "#PWR32" H 2450 5250 50  0001 C CNN
F 1 "+3V3" V 2500 5550 50  0000 C CNN
F 2 "" H 2450 5400 50  0001 C CNN
F 3 "" H 2450 5400 50  0001 C CNN
	1    2450 5400
	1    0    0    -1  
$EndComp
$Comp
L NCP114 U3
U 1 1 5A8A874C
P 1350 5200
F 0 "U3" H 1350 5591 39  0000 C CNN
F 1 "NCP114" H 1350 5516 39  0000 C CIB
F 2 "TO_SOT_Packages_SMD:SC-70-5" H 1350 5200 39  0001 C CNN
F 3 "" H 1350 5200 39  0001 C CNN
	1    1350 5200
	1    0    0    -1  
$EndComp
Text Label 3800 1350 0    60   ~ 0
USB-
Text Label 3800 1250 0    60   ~ 0
USB+
Text Label 3650 1750 0    60   ~ 0
test_pin
$Comp
L GND #PWR39
U 1 1 5A8AC037
P 9150 6350
F 0 "#PWR39" H 9150 6100 50  0001 C CNN
F 1 "GND" H 8700 6400 50  0001 C CNN
F 2 "" H 9150 6350 50  0001 C CNN
F 3 "" H 9150 6350 50  0001 C CNN
	1    9150 6350
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR36
U 1 1 5A8AC34E
P 9150 5850
F 0 "#PWR36" H 9150 5700 50  0001 C CNN
F 1 "+3V3" H 9165 6023 50  0000 C CNN
F 2 "" H 9150 5850 50  0001 C CNN
F 3 "" H 9150 5850 50  0001 C CNN
	1    9150 5850
	1    0    0    -1  
$EndComp
Text GLabel 3400 2550 2    51   Input ~ 0
AG_INT1
Text GLabel 3250 1050 2    51   Input ~ 0
AG_INT2
Text GLabel 2050 2950 3    51   Output ~ 0
M_DRDY
Text Notes 3800 3500 0    39   ~ 0
place one 0.1 uF cap near each VDD pin\nplace 4.7 uF near STM\nplace 1 uF cap near VDDA
$Comp
L C C13
U 1 1 5A8DD993
P 4550 2800
F 0 "C13" H 4550 2900 50  0000 L CNN
F 1 "0.1 uF" V 4500 2550 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 4588 2650 50  0001 C CNN
F 3 "" H 4550 2800 50  0001 C CNN
	1    4550 2800
	1    0    0    -1  
$EndComp
$Comp
L C C7
U 1 1 5A8DE13D
P 4300 2800
F 0 "C7" H 4300 2900 50  0000 L CNN
F 1 "0.1 uF" V 4250 2550 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 4338 2650 50  0001 C CNN
F 3 "" H 4300 2800 50  0001 C CNN
	1    4300 2800
	1    0    0    -1  
$EndComp
Text Notes 7850 4150 0    51   ~ 10
Fiducial Marks
Text Notes 9900 4200 0    51   ~ 10
Indicator LEDs
Text Notes 9350 5300 0    51   ~ 10
Connectors
Text Notes 3650 6150 0    51   ~ 10
Buttons / Switches
Text Notes 7400 600  0    51   ~ 10
LSM9DS1 Accel/Mag/Gyro Sensor
Text Notes 1900 650  0    51   ~ 10
STM32F3 Circuitry
Text GLabel 1000 2450 0    60   Input ~ 0
RESET_n
$Comp
L R R4
U 1 1 5A8A7550
P 2600 5450
F 0 "R4" H 2650 5450 50  0000 L CNN
F 1 "4K7" V 2600 5450 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 2530 5450 50  0001 C CNN
F 3 "" H 2600 5450 50  0001 C CNN
	1    2600 5450
	0    1    1    0   
$EndComp
Text Notes 2200 5900 0    39   ~ 0
Don’t enable analog PSU\nuntil digital PSU is up
$Comp
L +3V3 #PWR35
U 1 1 5A8DE2CE
P 10750 5800
F 0 "#PWR35" H 10750 5650 50  0001 C CNN
F 1 "+3V3" H 10765 5973 50  0000 C CNN
F 2 "" H 10750 5800 50  0001 C CNN
F 3 "" H 10750 5800 50  0001 C CNN
	1    10750 5800
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X06 J4
U 1 1 5A8DE7FE
P 11000 6100
F 0 "J4" H 11078 6141 50  0000 L CNN
F 1 "COMS" V 11100 5850 50  0000 L CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06_Pitch2.00mm" H 11000 6100 50  0001 C CNN
F 3 "" H 11000 6100 50  0001 C CNN
	1    11000 6100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR26
U 1 1 5A8E0A54
P 5050 3100
F 0 "#PWR26" H 5050 2850 50  0001 C CNN
F 1 "GND" H 5055 2927 50  0001 C CNN
F 2 "" H 5050 3100 50  0001 C CNN
F 3 "" H 5050 3100 50  0001 C CNN
	1    5050 3100
	1    0    0    -1  
$EndComp
$Comp
L +3.3VA #PWR19
U 1 1 5A8E0CA8
P 5050 2600
F 0 "#PWR19" H 5050 2450 50  0001 C CNN
F 1 "+3.3VA" V 5050 2850 50  0000 C CNN
F 2 "" H 5050 2600 50  0001 C CNN
F 3 "" H 5050 2600 50  0001 C CNN
	1    5050 2600
	1    0    0    -1  
$EndComp
$Comp
L C C12
U 1 1 5A8E15BF
P 8150 1000
F 0 "C12" V 8100 1050 50  0000 L CNN
F 1 "0.1 uF" V 8100 750 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 8188 850 50  0001 C CNN
F 3 "" H 8150 1000 50  0001 C CNN
	1    8150 1000
	0    1    1    0   
$EndComp
$Comp
L GND #PWR4
U 1 1 5A8E231E
P 7900 1000
F 0 "#PWR4" H 7900 750 50  0001 C CNN
F 1 "GND" H 7905 827 50  0001 C CNN
F 2 "" H 7900 1000 50  0001 C CNN
F 3 "" H 7900 1000 50  0001 C CNN
	1    7900 1000
	0    1    -1   0   
$EndComp
Text Notes 7800 4450 0    60   ~ 0
bottom
NoConn ~ 7900 4650
NoConn ~ 8100 4650
$Comp
L TEST_1P J2
U 1 1 58E08F41
P 7900 4650
F 0 "J2" H 7958 4770 50  0000 L CNN
F 1 "Fiducial" V 7950 4400 50  0000 L CNN
F 2 "Fiducials:Fiducial_1mm_Dia_2.54mm_Outer_CopperBottom" H 8100 4650 50  0001 C CNN
F 3 "" H 8100 4650 50  0001 C CNN
	1    7900 4650
	1    0    0    -1  
$EndComp
$Comp
L TEST_1P J5
U 1 1 58E08EB6
P 8100 4650
F 0 "J5" H 8158 4770 50  0000 L CNN
F 1 "Fiducial" V 8150 4400 50  0000 L CNN
F 2 "Fiducials:Fiducial_1mm_Dia_2.54mm_Outer_CopperBottom" H 8300 4650 50  0001 C CNN
F 3 "" H 8300 4650 50  0001 C CNN
	1    8100 4650
	1    0    0    -1  
$EndComp
$Comp
L TACT_4.2MM SW1
U 1 1 5A8F14BC
P 1050 2650
F 0 "SW1" V 750 2600 45  0000 C CNN
F 1 "TACT_4.2MM" V 850 2750 45  0000 C CNN
F 2 "MF_Switches:MF_Switches-TACT4.2MM" H 940 2800 20  0001 L CNN
F 3 "" H 1050 2650 60  0001 C CNN
	1    1050 2650
	0    -1   -1   0   
$EndComp
$Comp
L R R3
U 1 1 58CAF82A
P 1350 2850
F 0 "R3" H 1400 2850 50  0000 L CNN
F 1 "4K7" V 1350 2850 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 1280 2850 50  0001 C CNN
F 3 "" H 1350 2850 50  0001 C CNN
	1    1350 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1300 1250 1400 1250
Wire Wire Line
	1350 1050 1400 1050
Wire Wire Line
	1400 1150 1350 1150
Wire Wire Line
	1250 2200 1400 2200
Wire Wire Line
	1400 2100 1250 2100
Wire Wire Line
	1250 2000 1250 2300
Wire Wire Line
	1400 2000 1250 2000
Connection ~ 1250 2100
Wire Wire Line
	4900 1700 4900 1650
Wire Wire Line
	900  5050 900  5200
Wire Wire Line
	1700 4950 1700 5100
Wire Wire Line
	1000 2450 1400 2450
Wire Wire Line
	1100 1500 1400 1500
Wire Wire Line
	700  1600 1400 1600
Connection ~ 1150 1500
Connection ~ 750  1600
Wire Wire Line
	750  1200 1150 1200
Wire Wire Line
	750  1300 750  1200
Connection ~ 950  1200
Wire Wire Line
	3200 1250 3250 1250
Wire Wire Line
	10700 4600 10800 4600
Wire Wire Line
	10800 4550 10800 4900
Wire Wire Line
	10800 4900 10700 4900
Connection ~ 10800 4600
Wire Wire Line
	10400 4600 10350 4600
Wire Wire Line
	10400 4900 10350 4900
Wire Wire Line
	10050 4600 10000 4600
Wire Wire Line
	10000 4900 10050 4900
Wire Wire Line
	3200 1150 3250 1150
Wire Wire Line
	3200 1550 3250 1550
Wire Wire Line
	2150 2950 2150 2850
Wire Wire Line
	9800 5850 9750 5850
Wire Wire Line
	9800 5950 9750 5950
Wire Wire Line
	9750 6050 9800 6050
Wire Wire Line
	9800 6250 9750 6250
Wire Wire Line
	9150 6250 9250 6250
Wire Wire Line
	9250 6050 9150 6050
Wire Wire Line
	9250 5950 9150 5950
Wire Wire Line
	9150 5850 9250 5850
Wire Wire Line
	5550 7150 5650 7150
Wire Wire Line
	5550 7250 6150 7250
Wire Wire Line
	5550 7350 5700 7350
Connection ~ 5650 7350
Connection ~ 6100 7250
Wire Wire Line
	6100 7200 6100 7250
Wire Wire Line
	5600 6850 6100 6850
Wire Wire Line
	6100 6850 6100 6900
Connection ~ 5800 6850
Wire Wire Line
	5650 7650 5650 7700
Wire Wire Line
	5650 7700 5850 7700
Wire Wire Line
	5850 7700 5850 7650
Wire Wire Line
	5150 7250 5100 7250
Wire Wire Line
	3100 7150 3250 7150
Connection ~ 3150 7150
Wire Wire Line
	3150 6800 3150 6850
Wire Wire Line
	4300 6800 4300 6850
Wire Wire Line
	4250 7150 4450 7150
Connection ~ 4300 7150
Wire Wire Line
	2650 7150 2700 7150
Wire Wire Line
	3800 7150 3850 7150
Connection ~ 5600 7150
Wire Wire Line
	3400 2150 3200 2150
Wire Wire Line
	3200 2050 3400 2050
Wire Wire Line
	1350 3050 1350 3000
Wire Wire Line
	1150 2550 1400 2550
Wire Wire Line
	4300 2950 4300 3100
Wire Wire Line
	4050 2950 4050 3100
Connection ~ 4300 3100
Wire Wire Line
	4300 2600 4300 2650
Wire Wire Line
	4050 2650 4050 2600
Connection ~ 4300 2600
Connection ~ 4400 7150
Connection ~ 3200 7150
Wire Wire Line
	10750 6150 10800 6150
Wire Wire Line
	10750 6250 10800 6250
Wire Wire Line
	4550 1050 4600 1050
Wire Wire Line
	1250 2300 1400 2300
Connection ~ 1250 2200
Wire Wire Line
	10750 6350 10800 6350
Wire Wire Line
	3200 2450 3400 2450
Wire Wire Line
	3400 2350 3200 2350
Wire Wire Line
	3200 2250 3400 2250
Wire Wire Line
	3400 1950 3200 1950
Wire Wire Line
	3200 1850 3400 1850
Wire Wire Line
	4550 950  4550 1050
Connection ~ 4550 1050
Wire Wire Line
	3200 1450 3750 1450
Wire Wire Line
	3200 1350 3700 1350
Wire Wire Line
	4550 2600 4550 2650
Wire Wire Line
	4550 3100 4550 2950
Wire Wire Line
	3200 1750 4000 1750
Wire Wire Line
	2450 2950 2450 2850
Wire Wire Line
	2550 2950 2550 2850
Wire Wire Line
	7450 5900 7550 5900
Connection ~ 7550 5900
Wire Wire Line
	7550 6300 7550 6250
Wire Wire Line
	8200 6300 8200 6250
Wire Wire Line
	8100 5900 8200 5900
Connection ~ 8200 5900
Wire Wire Line
	3750 1450 3750 1350
Wire Wire Line
	3750 1350 4600 1350
Wire Wire Line
	3700 1250 4600 1250
Wire Wire Line
	3700 1350 3700 1250
Wire Wire Line
	3200 1650 3250 1650
Wire Wire Line
	4300 1750 4300 1700
Wire Wire Line
	4300 1400 4300 1350
Connection ~ 4300 1350
Wire Wire Line
	4300 1200 4300 1250
Connection ~ 4300 1250
Wire Wire Line
	4300 900  4300 850 
Wire Wire Line
	2300 5050 2750 5050
Wire Wire Line
	3450 4950 3450 5100
Wire Wire Line
	8100 1250 8450 1250
Wire Wire Line
	8500 1050 8450 1050
Wire Wire Line
	8450 950  8450 1150
Wire Wire Line
	8500 1850 8500 1750
Wire Wire Line
	8500 1650 8450 1650
Wire Wire Line
	8400 2700 8500 2700
Wire Wire Line
	8500 2600 8450 2600
Wire Wire Line
	8450 2600 8450 2700
Connection ~ 8450 2700
Wire Wire Line
	10250 2650 10200 2650
Wire Wire Line
	10250 2950 10500 2950
Wire Wire Line
	10500 2950 10500 2800
Connection ~ 10400 2950
Wire Wire Line
	10500 2500 10500 2450
Wire Wire Line
	10500 2450 10200 2450
Wire Wire Line
	10200 2100 10300 2100
Wire Wire Line
	10300 2000 10200 2000
Wire Wire Line
	9300 2950 9300 3050
Wire Wire Line
	9100 3000 9500 3000
Wire Wire Line
	9500 3000 9500 2950
Wire Wire Line
	9400 2950 9400 3000
Connection ~ 9400 3000
Connection ~ 9300 3000
Wire Wire Line
	9200 3000 9200 2950
Wire Wire Line
	9100 3000 9100 2950
Connection ~ 9200 3000
Wire Wire Line
	8450 1250 8450 1450
Wire Wire Line
	8450 1450 8500 1450
Wire Wire Line
	8500 1350 8450 1350
Connection ~ 8450 1350
Wire Wire Line
	8100 1550 8100 1650
Wire Wire Line
	7950 1650 8300 1650
Wire Wire Line
	8300 1650 8300 1550
Connection ~ 8100 1650
Connection ~ 8300 1250
Connection ~ 8100 1250
Wire Wire Line
	8400 1750 8450 1750
Wire Wire Line
	8450 1750 8450 1650
Wire Wire Line
	8400 1850 8500 1850
Wire Wire Line
	8400 2050 8400 1950
Wire Wire Line
	8400 1950 8500 1950
Wire Wire Line
	8050 2150 8450 2150
Wire Wire Line
	8450 2150 8450 2050
Wire Wire Line
	8450 2050 8500 2050
Wire Wire Line
	8050 2150 8050 2050
Wire Wire Line
	8050 2050 8400 2050
Wire Wire Line
	8400 2350 8500 2350
Wire Wire Line
	8450 2350 8450 2250
Wire Wire Line
	8450 2250 8500 2250
Connection ~ 8450 2350
Wire Wire Line
	10200 1250 10300 1250
Wire Wire Line
	10200 1100 10300 1100
Wire Wire Line
	1350 1150 1350 1050
Wire Wire Line
	1200 1150 1300 1250
Wire Wire Line
	1200 1150 1200 1050
Wire Wire Line
	3050 5500 3050 5700
Wire Wire Line
	2300 5650 3450 5650
Wire Wire Line
	3450 5650 3450 5400
Connection ~ 3050 5650
Wire Wire Line
	2750 5300 2750 5450
Wire Wire Line
	2300 5650 2300 5400
Wire Wire Line
	3350 5050 3450 5050
Connection ~ 3450 5050
Wire Wire Line
	1350 5500 1350 5650
Wire Wire Line
	900  5550 1700 5550
Wire Wire Line
	1700 5550 1700 5400
Connection ~ 1350 5550
Wire Wire Line
	900  5550 900  5500
Wire Wire Line
	1050 5050 1050 5300
Wire Wire Line
	1050 5150 900  5150
Connection ~ 900  5150
Connection ~ 1050 5150
Wire Wire Line
	1650 5050 1700 5050
Connection ~ 1700 5050
Wire Wire Line
	2300 4950 2300 5100
Connection ~ 2300 5050
Wire Wire Line
	9150 5950 9150 6350
Connection ~ 9150 6050
Connection ~ 9150 6250
Wire Wire Line
	3250 1050 3200 1050
Wire Wire Line
	3200 2550 3400 2550
Wire Wire Line
	2050 2950 2050 2850
Wire Notes Line
	400  3700 5250 3700
Wire Notes Line
	5250 3700 5250 400 
Wire Wire Line
	5050 3100 5050 2950
Wire Wire Line
	5050 2600 5050 2650
Wire Notes Line
	400  5950 7000 5950
Wire Notes Line
	4150 5950 4150 3700
Wire Notes Line
	7000 4050 7000 6550
Wire Notes Line
	7000 5100 11300 5100
Wire Notes Line
	8900 6500 8900 5100
Wire Notes Line
	11300 3200 6950 3200
Wire Notes Line
	6950 3200 6950 400 
Wire Notes Line
	7000 4050 11300 4050
Wire Notes Line
	9150 4050 9150 5100
Wire Wire Line
	1100 2100 1100 2050
Wire Wire Line
	1100 2400 1100 2450
Connection ~ 1100 2450
Wire Notes Line
	2150 5950 2150 3700
Wire Wire Line
	7550 5900 7550 5950
Wire Wire Line
	8200 5900 8200 5950
Wire Wire Line
	2450 5450 2450 5400
Wire Notes Line
	2650 5750 2650 5500
Wire Wire Line
	10750 6050 10800 6050
Wire Wire Line
	10750 5950 10800 5950
Wire Wire Line
	10750 5800 10750 5850
Wire Wire Line
	10750 5850 10800 5850
Wire Wire Line
	4050 2600 4550 2600
Wire Wire Line
	4050 3100 4550 3100
Wire Wire Line
	8450 1150 8500 1150
Connection ~ 8450 1050
Wire Wire Line
	8450 1000 8300 1000
Connection ~ 8450 1000
Wire Wire Line
	7900 1000 8000 1000
Wire Wire Line
	950  2750 850  2750
Wire Wire Line
	850  2750 850  2650
$Comp
L +3V3 #PWR20
U 1 1 5A8F2020
P 850 2650
F 0 "#PWR20" H 850 2500 50  0001 C CNN
F 1 "+3V3" V 850 2850 50  0000 C CNN
F 2 "" H 850 2650 50  0001 C CNN
F 3 "" H 850 2650 50  0001 C CNN
	1    850  2650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1350 2700 1350 2550
Connection ~ 1350 2550
NoConn ~ 1150 2750
NoConn ~ 950  2550
$Comp
L Jumper JP1
U 1 1 5A8F352D
P 4700 1950
F 0 "JP1" H 4750 1750 50  0000 C CNN
F 1 "Jumper" H 4700 1850 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" H 4700 1950 50  0001 C CNN
F 3 "" H 4700 1950 50  0001 C CNN
	1    4700 1950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR13
U 1 1 5A8F382F
P 4400 2000
F 0 "#PWR13" H 4400 1750 50  0001 C CNN
F 1 "GND" H 4405 1827 50  0001 C CNN
F 2 "" H 4400 2000 50  0001 C CNN
F 3 "" H 4400 2000 50  0001 C CNN
	1    4400 2000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5000 1650 5000 1950
Wire Wire Line
	4400 2000 4400 1950
$Comp
L C C14
U 1 1 5A8F2522
P 4850 2800
F 0 "C14" H 4850 2900 50  0000 L CNN
F 1 "1 uF" V 4800 2600 39  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 4888 2650 50  0001 C CNN
F 3 "" H 4850 2800 50  0001 C CNN
	1    4850 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 2950 4850 3100
Wire Wire Line
	4850 3100 5050 3100
Wire Wire Line
	4850 2650 4850 2600
Wire Wire Line
	4850 2600 5050 2600
Connection ~ 5050 2600
Connection ~ 5050 3100
$EndSCHEMATC
