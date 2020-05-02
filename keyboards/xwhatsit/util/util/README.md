# Building

## Option 1: Building on local system

1) Install qt5 development packages
2) Install hidapi development packages
3) Optionally: make sure your preferred backend is selected in util.pro:
   ```
   LIBS += -lhidapi-libusb
   #LIBS += -lhidapi-hidraw
   ```
4) ```qmake util.pro```
5) ```make```
6) Optionally: add the following rules to ```/etc/udev/rules.d/my_preferred_filename.rules```
   ```
   SUBSYSTEMS=="usb", ATTRS{idVendor}=="0481", ATTRS{idProduct}=="0002", MODE:="0666"
   ```
   And then restart udev
7) ```./util``` Or ```sudo ./util``` (if you have not set up udev)

## Option 2: Build with docker

In case you have trouble with dependencies.

1) ```docker build -t utilbuilder .```
2) ```docker run -it --rm  -e DISPLAY=$DISPLAY -v $HOME/.Xauthority:/root/.Xauthority --net=host -v `pwd`/../../:/xwhatsit -v /dev:/dev --privileged -w /xwhatsit/util/util utilbuilder /bin/bash```
   a) ```qmake util.pro```
   b) ```make clean && make -j```
   c) ```exit```
3) ```docker run -it --rm  -e DISPLAY=$DISPLAY -v $HOME/.Xauthority:/root/.Xauthority --net=host -v `pwd`/../../:/xwhatsit -v /dev:/dev --privileged -w /xwhatsit/util/util utilbuilder ./util```

(You can also try running directly on the local machine, if you have Qt and hidapi libraries installed)
