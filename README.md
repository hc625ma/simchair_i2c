# simchair_i2c

This is software for a new simchair mkIII I2C controller, based on Arduino Leonardo. 
It uses an epic Leonardo Joystick library by MHeironimus: https://github.com/MHeironimus/ArduinoJoystickLibrary 
and the I2C bus to communicate with peripherals.

There are several cool things about this approach:

    we can use device-specific configuration
    we can use external I2C ADCs
    we can use filtering when needed without slowing the main controller down
    we can extend our controller more than we’ll ever need
    unified interface for everything, buttons or axes
    Leo board allows us to use a nice Joystick library by MHeironimus, which simplifies things like ten times; 
    no more pain with USB descriptors

Note that you will need either an arduino pro mini or an ADS1115 for each peripheral (visit http://hc625ma.org for build manuals).
All simchair compatible controllers can be upgraded to work with this software and the new controller, see corresponding peripherals pages on hc625ma.org or thingiverse.


I don’t think there’s any point in using an old analog version anymore, as there are now controllers with buttons (an old UNO – based analog controller only supported 6 axes as it had to do some massive filtering because of a noisy 10 bit built-in ADC).

Master controller sketch is updated frequently, please update it regularly.

You can find STL files and sources, as well as new peripherals dev snapshots on http://hc625ma.org (preferred)
http://hc625ma.org/files/simchair3/

or thingiverse:
https://www.thingiverse.com/thing:2919692
Search for "simchair" to list all compatible stuff

Writing of documentation for the project takes as much effort as creating new models. If you have questions, please don't hesitate to ask, I'll do my best to help. Stay tuned for new updates!
