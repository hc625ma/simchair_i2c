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
    because of Leo board, we can now use a nice Joystick library, which simplifies things like ten times; 
    no more pain with USB descriptors

Note that you will need either an arduino pro mini or an ADS1115 for each peripheral (see thingiverse and http://hc625ma.org).
All simchair compatible controllers can be upgraded to work with this software and the new controller, see corresponding peripherals pages on thingiverse.


I don’t think there’s any point in using an old analog version anymore, as there are now controllers with buttons (an old UNO – based analog controller only supported 6 axes as it had to do some massive filtering because of a noisy 10 bit built-in ADC).

You can find STL files and sources on thingiverse:
https://www.thingiverse.com/thing:2919692
Look for "simchair" to list all compatible stuff

Manuals, build videos etc. are in the process of creation, stay tuned!
