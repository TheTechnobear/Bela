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
#include "SampleData.h"

SampleData gSampleData;	// User defined structure to get complex data from main
int gReadPtr;			// Position of last read sample from file

// Task for handling the update of the frequencies using the matrix
AuxiliaryTask gTriggerSamplesTask;

bool initialise_trigger();
void trigger_samples();

bool setup(BelaContext *context, void *userData)
{

	// Check that we have the same number of inputs and outputs.
	if(context->audioInChannels != context->audioOutChannels ||
			context->analogInChannels != context-> analogOutChannels){
		printf("Error: for this project, you need the same number of input and output channels.\n");
		return false;
	}

	// Retrieve a parameter passed in from the initAudio() call
	gSampleData = *(SampleData *)userData;

	gReadPtr = -1;

	// Initialise auxiliary tasks
	if(!initialise_trigger())
		return false;

	return true;
}

void render(BelaContext *context, void *userData)
{
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		float out = 0;

		// If triggered...
		if(gReadPtr != -1)
			out += gSampleData.samples[gReadPtr++];	// ...read each sample...

		if(gReadPtr >= gSampleData.sampleLen)
			gReadPtr = -1;

		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++)
			context->audioOut[n * context->audioOutChannels + channel] = out;	// ...and put it in both left and right channel
	}

	// Request that the lower-priority task run at next opportunity
	Bela_scheduleAuxiliaryTask(gTriggerSamplesTask);
}

// Initialise the auxiliary task
// and print info

bool initialise_trigger()
{
	if((gTriggerSamplesTask = Bela_createAuxiliaryTask(&trigger_samples, 50, "bela-trigger-samples")) == 0)
		return false;

	rt_printf("Press 'a' to trigger sample, 's' to stop\n");
	rt_printf("Press 'q' to quit\n");

	return true;
}

// This is a lower-priority call to periodically read keyboard input
// and trigger samples. By placing it at a lower priority,
// it has minimal effect on the audio performance but it will take longer to
// complete if the system is under heavy audio load.

void trigger_samples()
{
	// This is not a real-time task!
	// Cos getchar is a system call, not handled by Xenomai.
	// This task will be automatically down graded.

	char keyStroke = '.';

	keyStroke =	getchar();
	while(getchar()!='\n'); // to read the first stroke

	switch (keyStroke)
	{
		case 'a':
			gReadPtr = 0;
			break;
		case 's':
			gReadPtr = -1;
			break;
		case 'q':
			gShouldStop = true;
			break;
		default:
			break;
	}
}


void cleanup(BelaContext *context, void *userData)
{
	delete[] gSampleData.samples;
}


/**
\example samples/render.cpp

Playback WAV files
------------------

This sketch shows how to playback audio samples from a buffer.

An audio file is loaded into a buffer `SampleData` as `gSampleData`. This is 
accessed with a read pointer that is incremented at audio rate within the render 
function: `out += gSampleData.samples[gReadPtr++]`.

Note that the read pointer is stopped from incrementing past the length of the 
`gSampleData`. This is achieved by comparing the read pointer value against the 
sample length which we can access as follows: `gSampleData.sampleLen`.

The sample is triggered by keyboard input: (a) starts sample playback, (s) 
stops sample playback. The triggering is treated as a lower priority task than 
the audio. You can see this at the bottom of the render function: 
`Bela_scheduleAuxiliaryTask(gTriggerSamplesTask)`;
*/
