# simchair_i2c

![Simchair MKIII](http://hc625ma.org/wp-content/uploads/2018/10/05310003.jpg)

This software is for new simchair mkIII I2C controllers (master and peripherals). The master is based on Arduino Leonardo. 
It uses an epic Leonardo Joystick library by MHeironimus: https://github.com/MHeironimus/ArduinoJoystickLibrary 
and the I2C bus to communicate with peripherals, which use Arduino Pro mini or Adafruit ADS1115 ADC boards.

There are several cool things about this approach:

    we can use device-specific configuration
    we can use external I2C ADCs
    we can use filtering when needed without slowing the main controller down
    we can extend our controller more than we’ll ever need
    unified interface for everything, buttons or axes
    Leonardo board allows us to use a nice Joystick library by MHeironimus, which simplifies things like ten times; 
    no more pain with USB descriptors

Note that you will need either an arduino pro mini (sometimes 2 of them) or an ADS1115 for each peripheral (visit http://hc625ma.org for build manuals).
All simchair compatible controllers can be upgraded to work with this software and the new controller, see corresponding peripherals pages on http://hc625ma.org or thingiverse.


I don’t think there’s any point in using an old analog version (axes only) anymore, as this one is superior in every way.

Master controller sketch is updated frequently, please update it accordingly.

You can find STL files and sources, as well as new peripherals dev snapshots on http://hc625ma.org (preferred)
http://hc625ma.org/files/simchair3/

or thingiverse:
https://www.thingiverse.com/thing:2919692

Search for "simchair" to list all compatible stuff.

Writing of documentation for the project takes as much effort as creating new models. If you have questions, please don't hesitate to ask, I'll do my best to help. Stay tuned for new updates!
