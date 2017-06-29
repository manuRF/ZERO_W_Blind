# ZERO_W_Blind
Rasberry pi ZERO_W gpio control. Via IR remote (using wiring pi and lirc). Via Android socket client to connect to Raspberry pi gpio Android server. The raspberry has an IR diode on PIN GPIO04. Receiving RF signals fron a RF remote (Euskaltel brand, but you can use your own remote with proper lirc.conf file) you can control one motorized blind conected via GPIO-SSRs.

You need to install and setup lirc and wiringpi (google about that if you need). In /boot/config.txt add this line at the end for the lirc IR imput: dtoverlay=lirc-rpi:gpio_in_pin=4.

Added two PushButtons to manual control the motorized blind using Interrupts. Added an optical counter for position monitoring.
