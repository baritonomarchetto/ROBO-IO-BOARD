# ROBO-IO-BOARD
A generic development platform for DIY Robots.

<img width="1024" height="768" alt="IMG_20260421_120154_risultato" src="https://github.com/user-attachments/assets/6fc377cc-b93a-40ad-89e9-3a5f2389e658" />

# Intro
Since I was a child, I have dreamed of having a robot friend to interact with. Something similar to modern “speech assistants,” but without subservience.

Not that I was low on human-friends, to be honest, but it was appealing to me the possibility to interact with those futuristic movie-stars I could watch on TV.

Yes, robots were dream-inspiring "beings" in the mid '80s, even if mostly fake: hybrid mechatronics with a human-driven soul. Not something anyone skilled in the art (but a child) could be tricked by back then!

Fast forward today, that naive idea of a robotic friend with a "soul" has never been closer thanks to progress and availability of AI.

This made me think that it could be the time to dust that old desire of mine out and realize something. Just to see how far I could get this time (yes, it's not my first try, but maybe not even yours :))

After some online research and "primitive" tinkering, I realized that an assistant capable of interacting with humans through speech (a so-called ‘speech-to-speech’ assistant), able to gather information about its environment autonomously, and capable of actuating hardware (motors, lights, etc.) accordingly was actually an option.

In this repo all the files I am developing for the project.

# Supplies

Here is the bill of materials for main PCB and rotary extension board.

**MAIN PCB**

1x Arduino MEGA

1x LED 3mm

12x 220 ohm resistors

1x 2 Kohm resistor

2x 4.7 Kohm resistors

2x 10 Kohm trimmers

2x 100 nF ceramic capacitors

1x 100 uF electrolitic capacitor

1x 220 uF electrolitic capacitor

1x 470 uF electrolitic capacitor

1x 47 uH inductor (SMD)

2x 40 pin, single line pin headers (male)

12x 6 pin, dual line pin headers (male)

4x screw connectors (W237-102)

4x screw connectors (W237-103)

1X IDC connector (8x2)

1x DC power jack (DC barrel)

**Extension Board (rotary)**

2x rotary encoders (EC11)

2x BF3 tact switches (Omron)

1X IDC connector (8x2)

1x 10uF electrolitic capacitor

5x 100 nF ceramic capacitors

8x 10 Kohm resistors

Please notice that there's a SMD component (inductor) in the main board. Even if SMD, it's big and you will have no issues soldering.

Main PCB was designed for compatibility with a IP56 box with dimensions 190 x 140 x 70.

Double line pinheaders could be replaced with 2X single line if you prefer, but dual line are way easier to keep in place when soldering, so...

You are also in the need for the usual tools: a soldering station, some cm of Pb-free solder wire, some time for you :)

# General Approach

When I took this desire of mine back from the drawer, I planted some stakes deep in the ground:

- give it a mind - not an old school mechatronic, based on action-reaction triggering, but something more in line with my original desires (see introduction);
- have it capable of interacting in a speech to speech way;
- give it feedback about it's environment;
- give it the possibility to physically trigger external hardware.
- 
That "hardware" being a light, a fan, a set of motors, etc.

To the aforementioned, adds that I am forced to work on the cheap being, oh well... a poor guy. But I also am one that likes developing things anyone could use if readily instructed.

Additional boundary conditions for the whole project then are:

- use of readily available components;
- use of cheap devices;
- use of simplest possible solutions.
- 
We do all this for fun, after all! :)

**Microcontroller Board**

Needless to say, Arduino is a great place where to start a project like this. We can easily foresee a high number of I/O, flexibility in the handling of different input devices and communication protocols.

Its robust design and vast collection of libraries made MEGA a reasonable first candidate.

Other pros like it's 5V tollerance, high number of GPIOs and availability turned it to be the best candidate.

# Main Board

Most of the preliminary work here was the determination of which hardware I wanted to support and how many peripherals. PCB layout followed, even if in the end it was a recursive process.

<img width="1024" height="768" alt="IMG_20260421_104733_risultato" src="https://github.com/user-attachments/assets/b87ac149-d530-431f-bc59-73a5770a9d7b" />

**Main Features**

- up to 16 PWM servo motors
- up to 4 stepper motors
- up to 2 DC motors
- up to 12 sensors (I2C excluded)
- I2C ready
- Serial (USB/UART) ready
- customizable (and detachable) local controls
  
Please be aware that the PCB doesn't host motors driver boards! You cannot control directly motors other than servos. The PCB is intended to manage all digital informations from- and to- external drivers, the single exeption being servo motors.

In the following some more details about supported peripherals and related PCB design.

**PWM Servos**
Servo motors are different from all the other kind of motors. In particular, they have integrated driver boards and simple pinout, making them very handy to use.

On the other end, integrated drivers calls for power handling, especially when there's a PCB in the middle.

Even if up to 16 servos can be hosted, we are anyway limited in the number of servos we can control simultaneously because of trace's thickness used to juice servo loads.

PCB servo traces are 75 mil in thickness with a 15 mil tolerance. Before busses, current goes through a two-sides poligon with vias (via stitching) to increase the overall current.

Considering a 1oz thickness of copper, we can (conservatively) handle 3A of current per bus without significant heat generation. Just to be more clear, this means that you can handle big servo motors like DS3220 one at a time. They drain less than 3A in normal operation, but 3A if stalled.

The PCB supports up to 16 PWM servos, divided in 4 groups (or BUS) of 4. This is to limit the possibility that too much current goes through a single servo trace.

Even if I am not a fan of Dupont connectors, I adopted them because PWM servos are almost always sold with these and we want to make our life easy for now ;)

Any pin on the PCB is labeled with the GPIO number it is attached to.

Please notice that there are at least 2 different kind of servos out there: PWM servos and serial BUS servos. Advanced robots would use serial BUS servos instead of PWM servos. These are controlled through serial, so they don't ask for special care on PCB side.

**Stepper and DC Motors**

Stepper motors and DC motors are very different technically (see dedicated Step), but similar from a mere wiring point of view.

One important similitude, which makes them different from servo motors, is the need for a dedicated driver board. A driver board receives logic signals from the microcontroller (being in the form of PWM or simple hi/lo state change) and actuates motors accordingly.

This means that you do not send the actual motor power to the microcontroller board, but to the driver board. This, in turn, means that you don't have to manage high currents (which in turn means "heat dissipation") through the I/O board, being the current handled at driver board level.

The robot i/o board of this instructables supports a total of 4 stepper motors and 2 DC motors (or 6 stepper if you prefer).

Please notice that I enabled a single enable signal for all steppers. These signals could be wired to a shared-but-dedicated line for automatic or manual safety system.

**Sensors**

To have a feedback about it's environment, any robot needs sensors. The simplest being a push button, a whole lot of more sofisticated devices that perfect fit robotics are put in our hands by arduino.

These sensors are most often cheap and give valuable informations. They call for limited current drain, but clean signals to work appropriately.

Apart for the actual function, sensors mainly differ in the type of i/o lines they handle. Most often they call for one digital line, but sometimes analog or even two pins are necessary (refer to attached table).

I then organized pins such that one could use three (gnd/vcc/signal) for the most cases, but even four (gnd/vcc/signal1/signal2) if necessary.

GPIOS reserved for sensors have a 220 ohm series resistance, in order to reduce interference when long cables are used. These are optional and can be replaced with jumpers if sensors are limited in number and cables lenght limited to some tens of cm.

**Power Supply**

There's a single supply voltage on the PCB: +5V.

This is used to move servos, but also to juice sensors. This means that:

- the power supply must be adeguate to the whole load to drive.
- servos must be rated for 5V
- 
About the first point, servos are the only power hungry components and need special care. Being that there's a limit in current the PCB itself can handle, it is mandatory to keep total servo current to less than 3A.

The arduino is juiced by the USB line. This is in the need for serial communication to the PC anyway. USB and 5V lines are isolated from each other.
Being that servos can make the 5V line noisy, the power going to sensors is LC filtered.

**Local Control**

When debugging robot's hardware it comes handy to take direct control over outputs like servos and motors.
A 16 pins IDE connector hosting a generous amount of analog and digital pins is there to connect dedicated extension boards (see dedicated Step).

# Extension Board

Main PCB is designed to host custom extension boards for different tasks.

I designed a single extension board, for now, hosting two rotary encoders and two additional push buttons.

<img width="1024" height="768" alt="IMG_20260421_104727_risultato" src="https://github.com/user-attachments/assets/df638b65-0e78-4c45-8647-7aa22d7fde46" />

The extension board IDC connector pinout is directly serigrafated on the PCB.

This connector pinout hosts a number of 4 digital and 8 analog pins in order to (hopefully) handle any board configuration one could think of.

I preferred to use two rotaries in this first extension board layout instead of i.e. potentiometers because I already had in mind a possible first example code: moving a 6-DOF robotic arm.

# The Code

Shared code has all pins definitions declared and some of them are already initialized.

In example, the two built-in potentiometers are declared as input and can already be used like you would in any other arduino sketch. The same is true for the MEGA built-in LED.

All motors typologies have their own GPIOs array declared and set up in order to make it easier to access them without the need to remember the actual pin number.

Servo busses are organized in an array and the usual servo library gives you instant access to those motors.

Actually, the code I shared works as follows:

- one extension board's rotary moves a servomotor in each directions.
- the other one selects the servo motor focus.
- 
At first my idea was the development of a two-analog sticks extension board to move more servos at once, but this was not compatible with the PCB current handling limits. Moving a single servo at a time makes it impossible to exceed the supported current and then damage the PCB, so it was the obvious solution.

# And Now?

Now that we have a platform to connect all our motors and sensors, we can start to manage the possible following phases of the project.

The idea is to give an AI control over this environment and let it do whatever it wants with the informations coming from it and wired devices (am I the only one here finding this fascinating? :) ).

So we can likely divide the following in two (three, actually):

- realization of a speech-to-speech assistant
- realization of a robotic arm, and...
- Linking these two toghether!

# Acknowledgments

<img width="1024" height="768" alt="IMG_20260420_165702_risultato" src="https://github.com/user-attachments/assets/47c7c589-290a-41db-bba3-65d893a9869d" />

Many thanks goes to [JLCPCB](https://jlcpcb.com/IAT) for sponsoring PCBs manufacturing for this project. This project would never have seen the light without their material help.

They also sponsored the manufacturing of some preliminary PCB for the very next project.
Many many thanks.

JLCPCB is a high-tech manufacturer specialized in the production of high-reliable and cost-effective PCBs. They offer a flexible PCB assembly service with a huge library of more than 630.000 components in stock at today. This project made use of the service and everything went smooth and clean.

3D printing is part of their portfolio of services so one could create a full finished product, all in one place (note to self: start learning how to create 3D parts!).

What about [nano-coated stencils](https://jlcpcb.com/resources/nano-coated-stencil) for your SMD projects? You can take advantage of a coupon and test it at reduced price in these days.

By registering at JLCPCB site via [THIS LINK](https://jlcpcb.com/IAT) (affiliated link) you will receive a series of coupons for your orders. Registering costs nothing, so it could be the right opportunity to give their service a due try ;)

