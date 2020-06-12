# Harma Project

The Harma project first came about when I was talking to my friend and coworker Ben. We are both tinkerers and He used to work as a rework technician, so is quite good at fine pitch soldering. I however, have very shaky hands, so I shy away from anything smaller than an 0805 passive component.

That got me thinking, there are robotic assistants for surgeries, so why not for an over caffeinated tinkerer like myself?

Thus, this project was born. It consists of two major parts:

1. The thing the tinkerer holds on to to control the robot
2. The robotic arms holding:
   * The soldering iron
   * The solder
   * The wire or component

### The Name

Harma comes from the word Härma, which means Mimic in Swedish. A fitting name since the goal of this project is for you to control a robot that mimics your movements in a more precise and filtered way.

## Sub-Components

As stated above, this project consists of two major parts: The thing the user holds to control the robot and the robotic arms itself.

### [Pensel][Pensel Link]

Pensel, or brush/pencil in Swedish, is the device the user interfaces with to control the robotic arms. It is this sub-component that I will first tackle to make the whole system. The design as it stands is as follows:

There will be one unified design to control the iron, solder, and/or wire/components. As such, there must be a generic control methodology built into the Pensel. The Pensel will have:

1. A three position toggle switch for the user to indicate what mode the Pensel is operating in (Soldering iron, solder, or wire/component).
2. One push button to indicate that we are actively moving and want the robot to respond
3. Another push button to do the action supported by the mode. For example:
   * Feed solder
   * Feed wire
   * release component

Further Pensel documentation can be found [here][Pensel Link].

#### The Current design

* Processor
   * STM32F302K8U6
   * ARM® Cortex®-M4
   * 72MHz
   * CAN, I²C, IrDA, LIN, SPI, UART/USART, USB
   * DMA, I²S, POR, PWM, WDT
   * Hardware FPU
* Sensors
   * Combined Accelerometer and Magnetometer (LSM303DLHC) to detect orientation & movement
   * A button to trigger "active" mode
   * An auxiliary button to generate an action
   * A three position switch to indicate the active mode
   * A thumbwheel potentiometer to control sensitivity

### Robotic Arms (Working title)

This section has not been thought through yet. I am currently only focusing on the Pensel sub-component.


[Pensel Link]: https://www.holmesengineering.com/Harma/Pensel "Pensel Sub-Project Page"
