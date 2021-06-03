# LasS0 <!-- omit in toc -->

![Board](img/Nice.png)

# Contents <!-- omit in toc -->

- [What is this?](#what-is-this)
- [Usage](#usage)
- [3D printing](#3d-printing)

# What is this?

This is a logic level impulse to S0 converter/interface board. It is used to convert 5v logic level impulses to an S0 compatible format to use sensors like the TCRT5000 reflective optical sensor in existing smart home systems, that usually use 12v or 24v levels for digital inputs. This can be used to read out old school electrical meters as well as modern smart meters that only offer optical IR impulses or a spinning disc to "display" the current power usage.

# Usage

The device needs to be powered through `J1` with 12-24v.

One of the upper jumpers needs to be set to choose between `Direct` and `Inverted` which would invert the output fully. If no jumper is set you won't get any output. If both jumpers are set you are going to have a bad time.

A 5v logic level impulse input such as the output of a TCRT5000 can be connected to `J3/J4` at `D0`, which is then output to `J2` as an `S0` compatible open collector signal through the optocoupler.

If you are connecting the S0 output to certain devices that require high logic level input like 12v and can't directly deal with open collector S0 outputs, you might choose to set the lower two jumpers to put a pullup to `VCC` on `S0+` and connect `GND` to `S0-`. Combined with the `Inverted` output you will get your input pulses converted to VCC output pulses. (This is required for Loxone inputs for example, which is what I am using this for)

# 3D printing

Considering this is meant to go inside your electrical cabinet it is best printed in PC or ABS and not PLA.