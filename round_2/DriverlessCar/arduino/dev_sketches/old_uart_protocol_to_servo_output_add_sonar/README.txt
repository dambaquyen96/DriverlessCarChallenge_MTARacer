Arduino drives a 50Hz PWM signal whose on-time is specified by a Serial input. The signal is 
intended to control an ESC.

The Serial input follows the old UART protocol for FPT Driverless Car, which is described below:

Format: D###

  - D     Direction. 'f' for forward. 'b' for backward
  - ###   Relative desired engine speed / drive strength. The Arduino should translate this number
          into a PWM on-time.

Output pin, the upper and lower bound of the PWM on-time are specified in configuration macros.