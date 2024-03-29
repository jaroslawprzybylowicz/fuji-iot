# fuji-iot

## What is this about?

I have Fuji Electric AC units at home that I wanted to become integrated with my smart home network. Unfortunatelly, those are old-school devices with IR and wired remotes and no WiFi. 

## This repository & interfacing

This package contains a simple daemon that will convert serial interface of the AC unit into RPC service. In order to use it, you will need a device capable of handling UART @ 500bps (author of the code used Raspberry Pi Zero). Release package contains pre-compiled ARM package suitable to use with Raspbian.

## Hardware 

Hardware directory contains schematics and PCB design of a Raspberry PI Zero hat that interfaces directly with the AC Unit main board. It effectively replaces UTY-TWBXF module.
Hat converts 400V DC available on the mainboard into 5V DC necessary to power up Raspberry PI and converts Tx/Rx logic. PCB dimensions make Raspberry Pi Zero + Hat fit the AC unit internal space so that entire device is WiFi enabled without any extra cables.

## Other platforms

If you want to run this on ESP32 or Arduino, there is [another library](https://github.com/unreality/FujiHeatPump) based on this work ported by [unreality](https://github.com/unreality) into smaller package and available here:
https://github.com/unreality/FujiHeatPump 

## Compatibility

This software was tested against RSG09LUCA and RSG12LUCA and should interface well with Fuji Electric/Fujitsu Air conditioners that are using UTY-RNNUM, UTY-RNNXM  or similar controllers.
