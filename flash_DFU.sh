#!/bin/bash

dfu-programmer atmega32u4 erase 
dfu-programmer atmega32u4 flash $1 
dfu-programmer atmega32u4 reset 