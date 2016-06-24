/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

The platform for ultra-low latency audio and sensor processing

http://bela.io

A project of the Augmented Instruments Laboratory within the
Centre for Digital Music at Queen Mary University of London.
http://www.eecs.qmul.ac.uk/~andrewm

(c) 2016 Augmented Instruments Laboratory: Andrew McPherson,
	Astrid Bin, Liam Donovan, Christian Heinrichs, Robert Jack,
	Giulio Moro, Laurel Pardue, Victor Zappi. All rights reserved.

The Bela software is distributed under the GNU Lesser General Public License
(LGPL 3.0), available here: https://www.gnu.org/licenses/lgpl-3.0.txt
*/


#include <Bela.h>
#include <cmath>
#include <rtdk.h>
#include <stdlib.h>


bool setup(BelaContext *context, void *userData)
{
	pinMode(context, 0, P8_08, INPUT);
	pinMode(context, 0, P8_07, OUTPUT);
	return true;
}


void render(BelaContext *context, void *userData)
{
	for(unsigned int n=0; n<context->digitalFrames; n++){
		int status=digitalRead(context, 0, P8_08); //read the value of the button
		digitalWriteOnce(context, n, P8_07, status); //write the status to the LED
		float out = 0.1 * status * rand() / (float)RAND_MAX * 2 - 1; //generate some noise, gated by the button
		for(unsigned int j = 0; j < context->audioOutChannels; j++){
			audioWrite(context, n, j, out); //write the audio output
		}
	}
}


void cleanup(BelaContext *context, void *userData)
{
	// Nothing to do here
}


/**
\example digital-input/render.cpp

Switching an LED on and off
---------------------------

This example brings together digital input and digital output. The program will read 
a button and turn the LED on and off according to the state of the button.

- connect an LED in series with a 470ohm resistor between P8_07 and ground.
- connect a 1k resistor to P9_03 (+3.3V),
- connect the other end of the resistor to both a button and P8_08
- connect the other end of the button to ground.

You will notice that the LED will normally stay on and will turn off as long as 
the button is pressed. This is due to the fact that the LED is set to the same 
value read at input P8_08. When the button is not pressed, P8_08 is `HIGH` and so 
P8_07 is set to `HIGH` as well, so that the LED conducts and emits light. When 
the button is pressed, P8_08 goes `LOW` and P8_07 is set to `LOW`, turning off the LED.

Note that there are two ways of specifying the digital pin: using the GPIO label (e.g. `P8_07`), or using the digital IO index (e.g. 0)

As an exercise try and change the code so that the LED only turns on when 
the button is pressed.
*/

