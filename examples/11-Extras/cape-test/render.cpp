/*
 * render.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: parallels
 */


#include <Bela.h>
#include <cmath>

#define ANALOG_LOW	(2048.0 / 65536.0)
#define ANALOG_HIGH (50000.0 / 65536.0)

const int gDACPinOrder[] = {6, 4, 2, 0, 1, 3, 5, 7};

enum {
	kStateTestingAudioLeft = 0,
	kStateTestingAudioRight,
	kStateTestingAudioDone
};

uint64_t gLastErrorFrame = 0;
uint32_t gEnvelopeSampleCount = 0;
float gEnvelopeValueL = 0.5, gEnvelopeValueR = 0.5;
float gEnvelopeDecayRate = 0.9995;
int gEnvelopeLastChannel = 0;

float gPositivePeakLevels[2] = {0, 0};
float gNegativePeakLevels[2] = {0, 0};
float gPeakLevelDecayRate = 0.999;
const float gPeakLevelLowThreshold = 0.02;
const float gPeakLevelHighThreshold = 0.2;
const float gDCOffsetThreshold = 0.1;
int gAudioTestState = kStateTestingAudioLeft;
int gAudioTestStateSampleCount = 0;
int gAudioTestSuccessCounter = 0;
const int gAudioTestSuccessCounterThreshold = 64;
const int gAudioTestStateSampleThreshold = 16384;

// setup() is called once before the audio rendering starts.
// Use it to perform any initialisation and allocation which is dependent
// on the period size or sample rate.
//
// userData holds an opaque pointer to a data structure that was passed
// in from the call to initAudio().
//
// Return true on success; returning false halts the program.

bool setup(BelaContext *context, void *userData)
{
	return true;
}

// render() is called regularly at the highest priority by the audio engine.
// Input and output are given from the audio hardware and the other
// ADCs and DACs (if available). If only audio is available, numMatrixFrames
// will be 0.

void render(BelaContext *context, void *userData)
{
	static float phase = 0.0;
	static int sampleCounter = 0;
	static int invertChannel = 0;
	float frequency = 0;

	// Play a sine wave on the audio output
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		
		// Peak detection on the audio inputs, with offset to catch
		// DC errors
		for(int ch = 0; ch < 2; ch++) {
			if(context->audioIn[2*n + ch] > gPositivePeakLevels[ch])
				gPositivePeakLevels[ch] = context->audioIn[2*n + ch];
			gPositivePeakLevels[ch] += 0.1;
			gPositivePeakLevels[ch] *= gPeakLevelDecayRate;
			gPositivePeakLevels[ch] -= 0.1;
			if(context->audioIn[2*n + ch] < gNegativePeakLevels[ch])
				gNegativePeakLevels[ch] = context->audioIn[2*n + ch];
			gNegativePeakLevels[ch] -= 0.1;			
			gNegativePeakLevels[ch] *= gPeakLevelDecayRate;
			gNegativePeakLevels[ch] += 0.1;
		}
		
		if(gAudioTestState == kStateTestingAudioLeft) {
			context->audioOut[2*n] = 0.2 * sinf(phase);
			context->audioOut[2*n + 1] = 0;		
			
			frequency = 3000.0;
			phase += 2.0 * M_PI * frequency / 44100.0;
			if(phase >= 2.0 * M_PI)
				phase -= 2.0 * M_PI;
			
			gAudioTestStateSampleCount++;
			if(gAudioTestStateSampleCount >= gAudioTestStateSampleThreshold) {
				// Check if we have the expected input: signal on the left but not
				// on the right. Also check that there is not too much DC offset on the
				// inactive signal
				if((gPositivePeakLevels[0] - gNegativePeakLevels[0]) >= gPeakLevelHighThreshold 
					&& (gPositivePeakLevels[1] -  gNegativePeakLevels[1]) <= gPeakLevelLowThreshold &&
					fabsf(gPositivePeakLevels[1]) < gDCOffsetThreshold &&
					fabsf(gNegativePeakLevels[1]) < gDCOffsetThreshold) {
					// Successful test: increment counter
					gAudioTestSuccessCounter++;
					if(gAudioTestSuccessCounter >= gAudioTestSuccessCounterThreshold) {
						gAudioTestState = kStateTestingAudioRight;
						gAudioTestStateSampleCount = 0;
						gAudioTestSuccessCounter = 0;
					}

				}
				else {
					if(!((context->audioFramesElapsed + n) % 22050)) {
						// Debugging print messages
						if((gPositivePeakLevels[0] - gNegativePeakLevels[0]) < gPeakLevelHighThreshold)
							rt_printf("Left Audio In FAIL: insufficient signal: %f\n", 
										gPositivePeakLevels[0] - gNegativePeakLevels[0]);
						else if(gPositivePeakLevels[1] -  gNegativePeakLevels[1] > gPeakLevelLowThreshold)
							rt_printf("Right Audio In FAIL: signal present when it should not be: %f\n",
										gPositivePeakLevels[1] -  gNegativePeakLevels[1]);
						else if(fabsf(gPositivePeakLevels[1]) >= gDCOffsetThreshold ||
								fabsf(gNegativePeakLevels[1]) >= gDCOffsetThreshold)
							rt_printf("Right Audio In FAIL: DC offset: (%f, %f)\n",
										gPositivePeakLevels[1], gNegativePeakLevels[1]);						
					}
					gAudioTestSuccessCounter--;
					if(gAudioTestSuccessCounter <= 0)
						gAudioTestSuccessCounter = 0;
				}
			}
		}
		else if(gAudioTestState == kStateTestingAudioRight) {
			context->audioOut[2*n] = 0;
			context->audioOut[2*n + 1] = 0.2 * sinf(phase);
			
			frequency = 3000.0;
			phase += 2.0 * M_PI * frequency / 44100.0;
			if(phase >= 2.0 * M_PI)
				phase -= 2.0 * M_PI;
			
			gAudioTestStateSampleCount++;
			if(gAudioTestStateSampleCount >= gAudioTestStateSampleThreshold) {
				// Check if we have the expected input: signal on the left but not
				// on the right
				if((gPositivePeakLevels[1] - gNegativePeakLevels[1]) >= gPeakLevelHighThreshold 
					&& (gPositivePeakLevels[0] -  gNegativePeakLevels[0]) <= gPeakLevelLowThreshold &&
					fabsf(gPositivePeakLevels[0]) < gDCOffsetThreshold &&
					fabsf(gNegativePeakLevels[0]) < gDCOffsetThreshold) {
					// Successful test: increment counter
					gAudioTestSuccessCounter++;
					if(gAudioTestSuccessCounter >= gAudioTestSuccessCounterThreshold) {
						gAudioTestSuccessCounter = 0;							
						gAudioTestStateSampleCount = 0;
						gAudioTestState = kStateTestingAudioDone;
					}
				}
				else {
					if(!((context->audioFramesElapsed + n) % 22050)) {
						// Debugging print messages
						if((gPositivePeakLevels[1] - gNegativePeakLevels[1]) < gPeakLevelHighThreshold)
							rt_printf("Right Audio In FAIL: insufficient signal: %f\n", 
										gPositivePeakLevels[1] - gNegativePeakLevels[1]);
						else if(gPositivePeakLevels[0] -  gNegativePeakLevels[0] > gPeakLevelLowThreshold)
							rt_printf("Left Audio In FAIL: signal present when it should not be: %f\n",
										gPositivePeakLevels[0] -  gNegativePeakLevels[0]);
						else if(fabsf(gPositivePeakLevels[0]) >= gDCOffsetThreshold ||
								fabsf(gNegativePeakLevels[0]) >= gDCOffsetThreshold)
							rt_printf("Left Audio In FAIL: DC offset: (%f, %f)\n",
										gPositivePeakLevels[0], gNegativePeakLevels[0]);						
					}
					gAudioTestSuccessCounter--;
					if(gAudioTestSuccessCounter <= 0)
						gAudioTestSuccessCounter = 0;
				}
			}			
		}
		else {
			// Audio input testing finished. Play tones depending on status of
			// analog testing
			context->audioOut[2*n] = gEnvelopeValueL * sinf(phase);
			context->audioOut[2*n + 1] = gEnvelopeValueR * sinf(phase);

			// If one second has gone by with no error, play one sound, else
			// play another
			if(context->audioFramesElapsed + n - gLastErrorFrame > 44100) {
				gEnvelopeValueL *= gEnvelopeDecayRate;
				gEnvelopeValueR *= gEnvelopeDecayRate;
				gEnvelopeSampleCount++;
				if(gEnvelopeSampleCount > 22050) {
					if(gEnvelopeLastChannel == 0)
						gEnvelopeValueR = 0.5;
					else
						gEnvelopeValueL = 0.5;
					gEnvelopeLastChannel = !gEnvelopeLastChannel;
					gEnvelopeSampleCount = 0;
				}
				frequency = 880.0;
			}
			else {
				gEnvelopeValueL = gEnvelopeValueR = 0.5;
				gEnvelopeLastChannel = 0;
				frequency = 220.0;
			}

			phase += 2.0 * M_PI * frequency / 44100.0;
			if(phase >= 2.0 * M_PI)
				phase -= 2.0 * M_PI;
		}
	}

	for(unsigned int n = 0; n < context->analogFrames; n++) {
		// Change outputs every 512 samples
		if(sampleCounter < 512) {
			for(int k = 0; k < 8; k++) {
				if(k == invertChannel)
					context->analogOut[n*8 + gDACPinOrder[k]] = ANALOG_HIGH;
				else
					context->analogOut[n*8 + gDACPinOrder[k]] = 0;
			}
		}
		else {
			for(int k = 0; k < 8; k++) {
				if(k == invertChannel)
					context->analogOut[n*8 + gDACPinOrder[k]] = 0;
				else
					context->analogOut[n*8 + gDACPinOrder[k]] = ANALOG_HIGH;
			}
		}

		// Read after 256 samples: input should be low
		if(sampleCounter == 256) {
			for(int k = 0; k < 8; k++) {
				if(k == invertChannel) {
					if(context->analogIn[n*8 + k] < ANALOG_HIGH) {
						rt_printf("FAIL [output %d, input %d] -- output HIGH input %f (inverted)\n", gDACPinOrder[k], k, context->analogIn[n*8 + k]);
						gLastErrorFrame = context->audioFramesElapsed + n;
					}
				}
				else {
					if(context->analogIn[n*8 + k] > ANALOG_LOW) {
						rt_printf("FAIL [output %d, input %d] -- output LOW --> input %f\n", gDACPinOrder[k], k, context->analogIn[n*8 + k]);
						gLastErrorFrame = context->audioFramesElapsed + n;
					}
				}
			}
		}
		else if(sampleCounter == 768) {
			for(int k = 0; k < 8; k++) {
				if(k == invertChannel) {
					if(context->analogIn[n*8 + k] > ANALOG_LOW) {
						rt_printf("FAIL [output %d, input %d] -- output LOW input %f (inverted)\n", gDACPinOrder[k], k, context->analogIn[n*8 + k]);
						gLastErrorFrame = context->audioFramesElapsed + n;
					}
				}
				else {
					if(context->analogIn[n*8 + k] < ANALOG_HIGH) {
						rt_printf("FAIL [output %d, input %d] -- output HIGH input %f\n", gDACPinOrder[k], k, context->analogIn[n*8 + k]);
						gLastErrorFrame = context->audioFramesElapsed + n;
					}
				}
			}
		}

		if(++sampleCounter >= 1024) {
			sampleCounter = 0;
			invertChannel++;
			if(invertChannel >= 8)
				invertChannel = 0;
		}
	}
}

// cleanup() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in setup().

void cleanup(BelaContext *context, void *userData)
{

}
