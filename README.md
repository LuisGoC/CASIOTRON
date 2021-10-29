# CASIOTRON

Casiotron is a real-time Clock Calendar Application Project in C language for embedded systems using a 32 bit microcontroller of STM Family and HAL library provided by ST.
___
## Table of Contents

1. [About the Project](#about-the-project)
1. [Getting Started](#getting-started)
    1. [Dependencies](#dependencies)
    1. [Getting the Source](#getting-the-source)
    1. [Installation](#installation)
    1. [Building](#building)
    1. [Usage](#usage)
1. [Further Reading](#further-reading)
1. [Authors](#authors)
1. [Acknowledgments](#acknowledgments)
___
## About the Project

This is a project I made to develop my skills programming microcontrollers. 
It allows you to display the date, time and temperature on an LCD. Furthermore, you can set alarms of time and temperature through a computer terminal using AT commands (e.g. AT+TIME=12,30,00).

In this project I used several common peripherals of the microcontroller (SPI, USART, I2C, RTC, Timers, GPIOs, WWDG, NVIC).
I'm also created some generic drivers to control the usage of the LCD, temperature sensor and data queues for communication between processes. 

The project structure is divided in four different processes.
1. _Serial process_. 
    This process takes care of all the serial port terminal communication and AT cmd tasks.
2. _Clock process_.
    This process takes care of all the clock tasks.
3. _WWDG process_. 
    A simple process with the task of reset the watchdog counter.
4. _Heartbeat process_.
    A simple process with the task of toggle a LED each certain time.

Serial and clock processes were design using state machines to run in concurrent execution.

These are the state machine diagrams of those processes.
![Serial Process Diagram](https://github.com/LuisGoC/CASIOTRON/blob/main/Serial.drawio.png)

![Clock Process Diagram](https://github.com/LuisGoC/CASIOTRON/blob/main/Clock.drawio.png)

Hardware requirements:

- Nucleo STM32F070RB Board. 
- LCD 16x2 with SPI Interface ([NHD-C0216CZ-NSW](http://www.newhavendisplay.com/specs/NHD-C0216CZ-NSW-BBW-3V3.pdf)).
- Temperature Sensor with I2C Interface ([MCP9808](https://ww1.microchip.com/downloads/en/DeviceDoc/25095A.pdf)).


**[Back to top](#table-of-contents)**

## Getting started
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Dependencies
In order to build, debug or use this project you'll need the following dependencies to be install in your computer.

- GCC compiler.
- GDB debugger.
- OpenOCD.
- Make
- Git.
- VS Code (or another code editor).
- PuTTY (or another serial port terminal).

**[Back to top](#table-of-contents)**

### Getting the Source
This project is [hosted on GitHub](https://github.com/LuisGoC/CASIOTRON). You can clone this project directly using this command:

```
git clone https://github.com/LuisGoC/CASIOTRON.git
```
**[Back to top](#table-of-contents)**

### Installation
For download and install Visual Studio Code for Linux (Manjaro) you can click [here](https://snapcraft.io/install/code/manjaro).
You can also install GCC, the GNU compiler in the official [website](https://gcc.gnu.org/). Or using the following Linux command:
```
pacman -Sy gcc
```
For Git installation on Linux:
```
sudo pacman -S git
```
Putty Serial Port Terminal on Linux:
```
sudo pacman -S putty
```
Make installation on Linux:
```
sudo pacman -S make
```
For more help installing the dependencies you can visit this friend's blog [website](https://diefx.gitlab.io/).

**[Back to top](#table-of-contents)**

### Building
A makefile is included in this project to automate the compilation process and make easier some other features. You can use the following make commands through the VS Code terminal.
To build the project:
```
make all
```
To delete the built files:
```
make clean
```
To flash the code to the microcontroller:
```
make flash
```
To connect the interface and target micro to the debug host:
```
make open
```
To debug with GDB:
```
make debug
```
Note: flash and open commands are specific for the STM32F070RB microcontroller. 

**[Back to top](#table-of-contents)**

### Usage
Once you installed everything, clone and build the project you should make the hardware connections of the LCD and Temperature Sensor to the NUCLEO-F070RB Board.

The following illustration describes how to make the connections.

![Casiotron Schematic Diagram](https://github.com/LuisGoC/CASIOTRON/blob/main/Serial.drawio.png)

The table below shows the AT commands allowed to set time, date or alarms in the project via serial port terminal:

>**AT+TIME=hh,mm,ss** _//Set the time (hour, minutes, seconds)._
>**AT+DATE=dd,mm,yy** _//Set the date (day, month, year)._
>**AT+ALARM=hh,mm** _//Set the clock alarm (hour, minutes)._
>**AT+TEMP=lt,ut** _//Set the temperature alarm (lt-lower temperature, ut-upper temperature)._
>**AT+HEARTBEAT=ms** _//Set the time in milliseconds to toggle the LED (50 <= ms <= 1000)._

NOTE: Serial Port Terminal must be set at 115200 baud rate speed, 8 data bits, 1 stop bit, none parity, flow control off using UART protocol.

**[Back to top](#table-of-contents)**

## Authors
* **[Luis Gonzalez](https://github.com/LuisGoC)**

## Acknowledgments
I'd like to express my sincerely thanks of gratitude to my assessor Diego Perez to gave me the opportunity, knowledge and time to get better in something I really like as it is the area of the embedded systems programming and the help he gave me to realize this project. 
I appreciate your guidance.
