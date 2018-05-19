# Pensel

Pensel, or brush/pencil in Swedish, is the device the user interfaces with to control
the robotic arms. It is this sub-component that I will first tackle to make the whole
system. The design as it stands is as follows:

There will be one unified design to control the iron, solder, and/or wire/components.
As such, there must be a generic control methodology built into the Pensel. The
Pensel will have as follows:

1. A three position toggle switch for the user to indicate what mode the Pensel is
operating in (Soldering iron, solder, or wire/component).
2. One push button to indicate that we are actively moving and want the robot to respond
3. Another push button to do the action supported by the mode. For example:
   * Feed solder
   * Feed wire
   * release component

## Hardware Basics

* STM32F302K8U6
   * ARM® Cortex®-M4
   * 72MHz
   * I²C, UART/USART, USB, SPI, DMA, PWM, WDT
* Sensors
   * One LSM303DLHC with accelerometer and magnetometer for orientation and movement detection
   * A button to trigger "active" mode
   * An auxiliary button to generate an action
   * A three position switch to indicate the active mode
   * A thumbwheel potentiometer to control sensitivity


## Software Design

Below is an overview of all of the modules in the system.

![alt text][Pensel Software Diagram]

Below is an overview of the control and call structure of the system.

![alt text][Pensel Control Diagram]

[Pensel Software Diagram]: https://raw.githubusercontent.com/TDHolmes/Harma/master/Pensel/documentation/pictures/Pensel_SoftwareDiagram.png "Overall Pensel software diagram"
[Pensel Control Diagram]: https://raw.githubusercontent.com/TDHolmes/Harma/master/Pensel/documentation/pictures/Pensel_ControlHierarchy.png "Pensel control flow"
