# ClarionC-Blue
Clarion C-Bus interface made with arduino to spoof CD changer. Targeted use is for bluetooth, could also be simplified and used for AUX IN. The end goal is to have a single package Bluetooth module that plugs in to the CD Changer cable in the trunk and provides plug and play for bluetooth audio from a mobile phone or music device.

### Support

Testing and initial application being done in a 1995 Saab 900 S with stock head unit and existing CD changer wiring. Will work with any new generation Saab 900 from 1994 to 1998 (Excluding 1994 convertible).

Theory indicates that this unit, when assembled, will work with any Clarion made stereo using the 13 pin DIN connector. I do not have the ability to test with any other units. The only unit available to me, and the only unit I target, is the factory Clarion unit installed in my Saab 900.

This device is not planned at this time to support bluetooth hands free. It will only support A2DP (stereo audio) and AVRCP (playback controls).

The first prototype of the device will display mock track time on display, and persistent CD 1 TRACK 1, but it will not display track information. That is in the wish list, but the main goal here first is to establish communications between the device and the head unit.

### Documentation
http://embedded-bg.com/clarion_c-bus.txt

This document was incredibly insightful in regards to the protocols Clarion is using to communicate between the head and changer.

### Components
Component list will be published when a prototype has been built.
