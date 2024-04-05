# DPM86xx

DPM86xx is an Arduino library for controlling DPM86xx series power converters (DPM8605, DPM8608, DPM8616 and DPM8624).
The library uses Serial communication for sending and receiving commands using [this protocol](https://www.manualslib.com/uploads/action/downloadUploaded/id/52592).

This project has been forked from <https://github.com/Lotiq/DPM8600> and has undergone a major overhaul, in particular the evaluation of return values has been greatly improved.

## Table of Contents

- [General Information](#general-information)
- [Serial protocol via UART](#serial-protocol-via-uart)
- [Setup](#setup)
- [How to start](#how-to-start)

## General Information

- This library only works for **TTL** version of the converter, not RS-485.
- Serial communication happens over 5V so use a level converter if your arduino runs at 3.3V.
- DPM86xx take about 0.6s to turn on an adjust the settings, so turning on the power and immediately getting the readings will occasionally produce erroneous measurements.
- Current resolution is 3 decimal places, while voltage is 2 decimal places.
- The implementation analyses and validates the responses from the DPM86xx so that the user is notified of any possible faulty communication.

## Serial protocol via UART

### Examples of read and write sequences

Here som examples of read and write request and response protocol with DPM8624 from JUNTEK manufacturer.

These are listed here, as the description of the protocol contains some errors.

#### Read maximal voltage

```shell
REQ: :01r00=0,
RES: :01r00=6000.
```

#### Read maximal current

```shell
REQ: :01r01=0,
RES: :01r01=24000.
```

#### Switch off the PSU

```shell
REQ: :01w12=0,
RES: :01ok
```

#### Switch on the PSU

```shell
REQ: :01w12=1,
RES: :01ok
```

#### Write voltage and current

Write set voltage value to 53.28 V and set current value to 1.000 A:

```shell
REQ: :01w20=5328,1000,
RES: :01ok
```

#### Read measured voltage

Here is read value 53.39 V:

```shell
REQ: :01r30=0,
RES: :01r30=5339.
```

#### Read measured current

Here is read value 0.000 A:

```shell
REQ: :01r31=0,
RES: :01r31=0.
```

#### Reading temperature

Read temperature, here is read value 20 °C:

```shell
REQ: :01r33=0,
RES: :01r33=20.
```

## Setup

![DPM86xx Layout](images/layout.jpg)

As can be seen on the image on above, there several main component to working with DPM86xx power converters. First, In+ In- indicates where the input needs to be connected (10-75V). Then Out+ Out- is for the output. Finally communication is done through ports 1-4:

1 - Power

2 - Data out (->)

3 - Data in (<-)

4 - Ground

If using Arduino with the same 5V serial (e.g Uno), you only need to connect ports 2-4. However, if your Arduino is 3.3V (e.g MKR1010) then you have to use a [level converter](https://www.sparkfun.com/products/12009) to step between different voltages, and to power that converter you would have to use port 1 as well.

## How to start

```cpp
DPM86xx clPsuG;

clPsuG.init(Serial2, 1);
```

Creates a new DPM86xx object with a different address (1-99). The address can be changed manually on the power converter.

e.g. clPsuG.init(Serial2, 2); - communicates with converter with address = 02

e.g. clPsuG.init(Serial2); - communicates with converter with address = 01

An example of implementation can be found in [.\examples\psu_init.cpp](.\examples\psu_init.cpp)
