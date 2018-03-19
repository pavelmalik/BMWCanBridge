# BMWCanBridge
#### Retrofiting/swapping night vision and other modules to modern bmws

Did you damage the very expensive night vision camera on your bmw? Or are you, like me, blind as a bat when it's dark out and would like to have a night vision camera? It turns out with an arduino and some C++ you can retrofit the system to most modern bmws:

###### Bit of technical info
The night vision module, along with others that connect to k-bus (CIC, NBT navigation etc), is coded directly to the vin number of the original car and refuses to work if it receives another vin. Specifically, all these modules look for 7 byte long 0x380 frames that encode the last 7 digits of the vin as ascii hex values. 

###### Arduino to the rescue
The good folks at CopperHill Technologies make a [dual channel can bus shield](http://copperhilltech.com/arduino-based-ecu-development-board-with-dual-can-bus-interface/) for the Arduino Due that allows you to [intercept and change the vin frames](https://en.wikipedia.org/wiki/Man-in-the-middle_attack) to match whatever the modules are expecting. 

>One caveat for the night vision application: *both the camera and the module* have to come from the same donor. There is an extra layer of security between the two and the communication doesn't happen over k-can bus.



###### Wiring and set
To wire up the arduino, tap any k-can wires (yellow/black twisted pair on e60 cars, there's one right under the glovebox) and run that into CAN1 port on the module, *without* any terminating resistance. Then run another set of can high/can low wires from the module you'd like to install (eg the night vision controller) to CAN2 port and add 120ohm resistor from each wire to ground. The K-can lines on bmws use low speed / fault tolerant can-bus spec, so termination does not happen across the two can lines. To power the arduino, you can use something like the amplifier 12v pin on the CCC nav unit (pin 13) that goes hot whenever you unlock the car.

![Wiring diagram](https://raw.githubusercontent.com/pavelmalik/BMWCanBridge/master/attachment.jpeg)

Once it's all wired up, grab the BMWCanBridge sketch code from github, edit the values of DONOR_VIN array with your donor vin and upload the sketch to your Arduino Due. The new module will be happy and, more importantly, all factory diagnostics will see it as if it were OEM.


###### Night vision parts list


* 66549164992 Night vision controller
* 66549167255 Night vision camera, hopefully with the bracket
* 61316946321 Headlight switch with night vision button (and hud)
* 61138373632 Connector for the camera
* 61130005199 6x pins for the camera connector
* 61138364666 18 pol connector for the night vision controller (AMP 1-963616-1) - tons of bmws use them
* 61138364662 Cap thingy with lever for the connector above 


And a handy factory [retrofit guide](https://www.bimmerfest.com/forums/attachment.php?attachmentid=181022&d=1239472676)
