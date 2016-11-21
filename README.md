# DayShift

DayShift shifts circadian rhythm.  Fix your Jet Lag or become and early riser.

![animated][animated]

# How it works

Ultra-short 2ms flashes of bright light early in the morning can make your mind(circadian clock) think it's day out before it actually is.

This shifts the circadian clock.

So if you need to travel, you can shift your clock before you get there and significantly shorten the jetlag.

Uses:
- Fix JetLag
- Become early riser
- Counteract evening screen use

This is based on a recent Stanford research paper [https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4767340/](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4767340/) by Raymond P. Najjar and Jamie M. Zeitzer

I've learned about this on humanOS podcast from a [conversation between Dan Pardi and Dr. Jamie Zeitzer](http://blog.dansplan.com/a-new-product-to-significantly-reduce-jet-lag-interview-with-stanford-professor-jamie-zeitzer/) 

I'm about to travel 9 timezones in two weeks, and jetlag always takes away my vacation time I wanted to try this immediately. 
And since you can't buy this, I needed a quick fix.

Here are the instructions on how to build your own.

# How to build one

You'll need this:

- an Arduino. I used [Arduino 101](https://www.amazon.com/Intel-Arduino-single-Components-ATLASEDGE-1/dp/B0198HHR06) since I had one lying around, but you could you any Arduino. This one had an added benefit of having Bluetooth low energy, though. 
- a bright LED headlamp. I used [Rayovac Sportsman Virtually Indestructible 100 Lumen 3AAA Hi Performance Headlight (OTHPHL-BA)](https://www.amazon.com/Rayovac-Sportsman-Virtually-Indestructible-Performance/dp/B004Y6DMK6/ref=sr_1_5?ie=UTF8&qid=1479591395&sr=8-5&keywords=rayovac+headlamp). Others might work, but this is the one I had lying around. I turned out to be great for this project, because of the LED driver circuit it used.
- a soldering iron
- a wire
- 22-ohm resistor 
- 100-ohm resistor 

You can get all this on Amazon.

# My headlamp

The circuit in my headlamp is a basic NPN switch. The transistor connected to the LED had a 617 marking, a quick google search for `sot23 617` told me this is an [FMMT617](http://www.s-manuals.com/pdf/datasheet/f/m/fmmt617%2C_fmmt618%2C_fmmt619%2C_fmmt624%2C_fmmt625_zetex.pdf) NPN Transistor. A good design choice, because of it's high current, high gain, and fast switching.

![headlamp][headlamp]

# Wiring it up

![circuit][circuit]

Pin 12 on Arduino should drive the LED.

|Ardino | Headlamp|
|---|---|
|GND | - |
|5V | 22-ohm to +
|pin 12 | 100-ohm to B on FMMT617


Connect digital **pin 12** through a 100ohm resistor to BASE of FMMT617 and GND. That's it. You can power the headlamp from the 5V on Arduino instead from the batteries, but that's optional. (5V gives it an extra boost). 

I've also used a 22-ohm resistor from 5V on Arduino to the + on the headlamp to smooth the current surges on power up and to protect the circuit and the USB in case the code misbehaved.

# Arduino Sketch

The idea is to generate **2ms** blinks **every 10s**.
Which could be a simple as:
```c++
digitalWrite(12,  HIGH);    
delay(2);                         
digitalWrite(12,  LOW);    
delay(10000);
```

I've added start/end time configuration and BTLE support though. 

You can run it directly in the awesome new online Arduino Editor here:

[https://create.arduino.cc/editor/pkuhar/af1a83e4-7b82-48b8-8a3b-592b06766842/preview](https://create.arduino.cc/editor/pkuhar/af1a83e4-7b82-48b8-8a3b-592b06766842/preview)

Or open 'dayshift.ino' in Arduino IDE.

## Setting the current, start and end time

This is the tricky part if you don't use Arduino 101 with BTLE or can't use the app.
Since most Arduinos don't have a real-time clock, you'll need to hardcode the power on time in the sketch.

Look for this section at the top of the sketch and adjust as needed.

```c++
double initialTimeOfDay = 23;   // the time of powering up the board
double flashStartTime   = 4.5;  // when in the morning you want the flashing start
double flashEndTime     = 8.0;    // when do you want the flashing end
```

# Mobile App

The included Cordova App for iOS and Android(not tested) maker updating the start and end times easier.
You'll need Xcode to install it on iOS.

To install it on the device run:

    cordova run ios --device

If you get something like ` Unable to locate DeviceSupport directory.` open the project `platforms/ios/DayShift.xcodeproj` directly and install it from there.

# License

[CC BY-SA 3.0](https://creativecommons.org/licenses/by-sa/4.0/)


[animated]: dayshift-animated.gif
[circuit]: dayshift-circuit.jpeg
[headlamp]: headlamp.jpeg