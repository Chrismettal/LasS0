# LasS0 <!-- omit in toc -->

![BoardRender](img/Nice.png)

# WARNING <!-- omit in toc -->

The current version of the PCB is tested working, but uses an BJT input stage which takes around 6mA to drive. The device was tested with a FET input stage and optional pullups/pulldowns on the input which does not require such a high driving current and therefor performs better with TCRT5000 modules. The PCB files are currently being updated with the new input stage.

# Contents <!-- omit in toc -->

- [Buying](#buying)
- [What is this?](#what-is-this)
- [Usage](#usage)
- [Firmware](#firmware)
- [Performance](#performance)
    - [0-5V DC pulses, 50Hz, 5ms pulse length](#0-5v-dc-pulses-50hz-5ms-pulse-length)
    - [Rise time 33µs, Fall time 1µs](#rise-time-33µs-fall-time-1µs)
    - [Switch on delay 66µs](#switch-on-delay-66µs)
    - [Switch off delay 5µs](#switch-off-delay-5µs)
    - [TCRCT5000 with "normal" non inverted configuration](#tcrct5000-with-normal-non-inverted-configuration)
    - [TCRCT5000 with "inverted" configuration](#tcrct5000-with-inverted-configuration)
- [3D printing](#3d-printing)

# Buying

Keep an eye out on the Tindie store, where leftover boards will be sold!

<a href="https://www.tindie.com/stores/binary-6/?ref=offsite_badges&utm_source=sellers_Chrismettal&utm_medium=badges&utm_campaign=badge_medium"><img src="https://d2ss6ovg47m0r5.cloudfront.net/badges/tindie-mediums.png" alt="I sell on Tindie" width="150" height="78"></a>

# What is this?

This is a logic level impulse to S0 converter/interface board. It is used to convert 5v logic level impulses to an S0 compatible format to use sensors like the TCRT5000 reflective optical sensor in existing smart home systems, that usually use 12v or 24v levels for digital inputs. This can be used to read out old school electrical meters as well as modern smart meters that only offer optical IR impulses or a spinning disc to "display" the current power usage.

![BoardReal](img/Board.jpg)

![BoardRealKit](img/BoardKit.jpg)

There are 'two versions' of the board included. LasS0_FET uses a FET input stage. The included gerbers are NOT tested and probably won't be. The initial NPN input stage version however was ordered and tested working.
The other, more recent version uses an AVR microcontroller as the input stage, specifically providing pulse extension and pulse division between the input and the S0 output. This was neccessary for me since my electricity meter outputs 10,000 imp/kWh with only a pulselength of about 7ms, which my smart home controller could not read. It required pulses of at least 15ms to reliably detect every pulse. The AVR counts a set number of input pulses and outputs a fixed output pulse once enough inputs have been counted. In my case I divide 10:1 and output a 30ms pulse after every 10 input pulses, changing the total resolution of my specific meter to 1,000 imp/kwh.

The AVR board additionaly breaks out all unused portpins to pinheaders as well as the AO screw terminal so you could theoretically read out the analog value of a TCRT5000 as well.

# Usage

The device needs to be powered through `J1` with 12-24v.

One of the upper jumpers needs to be set to choose between `Direct` and `Inverted` which would invert the output fully. If no jumper is set you won't get any output. If both jumpers are set you are going to have a bad time.

A 5v logic level impulse input such as the output of a TCRT5000 can be connected to `J3/J4` at `D0`, which is then output to `J2` as an `S0` compatible open collector signal through the optocoupler.

If you are connecting the S0 output to certain devices that require high logic level input like 12v and can't directly deal with open collector S0 outputs, you might choose to set the lower two jumpers to put a pullup to `VCC` on `S0+` and connect `GND` to `S0-`. Combined with the `Inverted` output you will get your input pulses converted to VCC output pulses. (This is required for Loxone inputs for example, which is what I am using this for).

# Firmware

To translate any type of input pulse to any type of output pulse an Attiny24a is used. The firmware is done in C but built in PlatformIO. The project can be found in the `Code/Platformio` folder.

If you don't wanna compile yourself, there are several prebuild binaries for different configurations of input and output pulses. Find them in the `/Builds` directory.
This directory contains  .elf and .hex files for every environment.

The naming structure for the environments is as follows:

`tinyW_XXX_Y_Z`

`W` = Divider ratio
`X` = Output pulse length
`Y` = `r` for rising edge input, `f` for falling edge input
`Z` = `i` for inverted output, `n` for non-inverted output

So:
`tiny10_30_n_r` generates a `30 ms` `positive` output pulse for every `10` `rising edges` on the input
`tiny5_250_i_f` generates a `250ms` `negative` output pulse for every `5` `falling edges` on the input

The firmware is uploaded via `ICSP`.

# Performance

![MeasurementMethod](img/CRO.jpg)

The device was tested, on a 23 V supply with 5 V test pulses from a function gen as well as the TCRT5000.

The version that was tested needs around 6mA of current on the DO input to drive the input stage. My TCRT5000 module is only able to deliver about 1mA somehow so the channel 1 voltage with the TCRT5000 drops significantly but the output is still fully driven. Currently ordering some new TCRT5000 to see why the comparator can only drive such low currents. At the same time I will switch out the S8050 transistors to AO3400A MOSFETs to greatly raise the input impedance of DO. 

The following measurements / demonstrations were done with the S8050 input stage, PCB revision 38b7723, with the S0 pullup jumpers set:

### 0-5V DC pulses, 50Hz, 5ms pulse length

![5msPulse](img/5msPulse.png)

### Rise time 33µs, Fall time 1µs

![RiseFall](img/RiseFall.png)

### Switch on delay 66µs

![DelayRise](img/DelayRise.png)

### Switch off delay 5µs

![DelayFall](img/DelayFall.png)

### TCRCT5000 with "normal" non inverted configuration

![Normal](img/TCRT5000Normal.png)

### TCRCT5000 with "inverted" configuration

![Inverted](img/TCRT5000Inverted.png)

# 3D printing

Considering this is meant to go inside your electrical cabinet it is best printed in PC or ABS and not PLA.

There is a 3D printable DIN rail case for the LasS0 PCB itself, as well as a readhead to attach a TCRT5000 module to EMH eHZ readers (which is what I am using this whole project for).

The case incorporates DIN rail clamps, remixed from a cable clamp by mdkendall, [check it out here!](https://www.thingiverse.com/thing:2613804).

![3DCase](img/3D.png)

![3DHead](img/3Dhead.png)

