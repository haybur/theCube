#pragma once
#include <Statistic.h>

int mixAmount;

void audioSetup() {
	//if (digitalRead(switchPin) == 1) {
	//Serial.print("Brightness Pot is at ");
	//Serial.println(analogRead(brightnessPot));
	//if (analogRead(brightnessPot) > 500) {
	//	myInput = AUDIO_INPUT_LINEIN;
	//	Serial.println("Input is Aux");
	//}
	//else {
	//	myInput = AUDIO_INPUT_MIC;
	//	Serial.println("Input is Mic");
	//}
	// AUDIO STUFF
	AudioMemory(12);
	audioShield.enable();
	audioShield.inputSelect(myInput);
	//audioShield.lineInLevel(10); // This should normalize the volume levels of different audio inputs, but it also makes the quieter parts of songs louder. No Bueno.

	// configure the mixer to equally add left & right
	mixer1.gain(0, 0.5);
	mixer1.gain(1, 0.5);
}

// Clears the running mean and st dev. Used mostly between songs when silence is detected.
void clearStats() {
	for (int i = 0; i < 14; i++) {
		freq[i].clear();
		beatPeak[i] = 0;
		binScore[i] = 0;
		lowBeatBin = 2;
		midBeatBin = 8;
		highBeatBin = 12;
	}
}

void fillStats() {
	for (int i = 0; i < 14; i++)
		spectrumValueOld[i] = spectrumValue[i];

	spectrumValue[0] = fft1024.read(0) * 1000;
	spectrumValue[1] = fft1024.read(1) * 1000;
	spectrumValue[2] = fft1024.read(2, 3) * 1000;
	spectrumValue[3] = fft1024.read(4, 6) * 1000;
	spectrumValue[4] = fft1024.read(7, 10) * 1000;
	spectrumValue[5] = fft1024.read(11, 15) * 1000;
	spectrumValue[6] = fft1024.read(16, 22) * 1000;
	spectrumValue[7] = fft1024.read(23, 32) * 1000;
	spectrumValue[8] = fft1024.read(33, 46) * 1000;
	spectrumValue[9] = fft1024.read(47, 66) * 1000;
	spectrumValue[10] = fft1024.read(67, 93) * 1000;
	spectrumValue[11] = fft1024.read(94, 131) * 1000;
	spectrumValue[12] = fft1024.read(132, 184) * 1000;
	spectrumValue[13] = fft1024.read(185, 257) * 1000;

	volume = 0;
	// Adjust the standard deviation and average of each array value as the song goes
	for (int i = 0; i < 14; i++) {
		freq[i].add(spectrumValue[i]);
		average[i] = freq[i].average();
		stDev[i] = freq[i].pop_stdev();
		//variance[i] = pow(stDev[i], 2);
		volume = volume + spectrumValue[i];

		if (spectrumValue[i] > average[i])
			fadeVals[i] += (spectrumValue[i] - average[i]) / 2;
		else
			fadeVals[i] -= (average[i] - spectrumValue[i]) / 2;

		fadeVals[i] = constrain(fadeVals[i], 0, 255);
		//Serial.print(fadeVals[i]);
		//.print("\t");
	}
	//Serial.println();

	uint8_t silenceCounter = 0;
	// If over 13 of the 15 frequencies are silent, 
	// reset the statistics because it's likely the song ended
	for (int i = 0; i < 14; i++) {
		if (spectrumValue[i] < 6) { // 3 is an arbitrary number but it means it is basically silent
			silenceCounter++;
		}
		else {
			break;
		}
	}
	if (silenceCounter > 11) {
		//Serial.println("stats have been cleared");
		songDuration = 0;
		clearStats();
	}
	else
		silenceDuration = 0;


	// If song is over 300000ms, 5 minutes, clear the stats. DJs have songs blend together which would never clear the stats
	if (songDuration > 300000) {
		songDuration = 0;
		clearStats();
	}

	if (silenceDuration > 5000) {
		silence = true;
		lowBeats = false;
		midBeats = false;
		highBeats = false;
		songDuration = 0;
	}
	else 
		silence = false;
}

//void beatTiming(int i) {
//		if (beatDetected[i] == 2) { // if beat has been detected
//			if (beatTimer[i] < 200) // and it isn't 200 ms from the last one
//				beatDetected[i] = 1; // but if it is, cancel the beat
//			else {
//				beatInterval[i] = beatTimer[i];  // a beat has been detected. Get the time since last beat.
//				// could use absolute value below
//				if ((beatIntervalOld[i] - beatInterval[i]) < 30 && (beatIntervalOld[i] - beatInterval[i]) > -30) // If the time between the new interval and old interval is less than 30 ms 
//					constantBeatCounter[i]++;
//
//				beatTimer[i] = 0;
//				beatIntervalOld[i] = beatInterval[i];
//			}
//		}
//		if (constantBeatCounter[i] > 0 && beatTimer[i] > beatInterval[i] + 50) constantBeatCounter[i] = 0; // clears beat counter when more time than the beat has passed
//
//		// These print statements will print the constant beat counter of each bin
//		//if (i == numFFTBins -1) Serial.println(constantBeatCounter[i]);
//		//else {
//		//	Serial.print(constantBeatCounter[i]);
//		//	Serial.print("\t");
//		//}
//}

void beatDetection(int i) {
	// 0 = no beat detected
	// 1 = beat hasn't dropped / reset yet
	// 2 = beat detected
	// Beat is detected here. Must be greater than the average+(2.3*st.dev) and greater than 100 which is pretty quiet
		//if (spectrumValue[i] > average[i] + 2.3 * stDev[i] && spectrumValue[i] > 100) {
	if (spectrumValue[i] > average[i] + 2.3 * stDev[i] && spectrumValue[i] > 100) {
		if (beatDetected[i] == 2)
			beatDetected[i] = 1;
		if (beatDetected[i] == 0)
			beatDetected[i] = 2;
		// if it's == 1, it stays 1.
	}
	// This is where 1's get reset to 0. If beat is not detected in that frequency, set it's status to 0.
	else {
		if (beatDetected[i] == 1) {
			// else, don't be ready for the next beat until the frequency has hit the running average.
			// This prevents multiple beats being triggered when they aren't well defined
			if (spectrumValue[i] <= average[i])
				beatDetected[i] = 0;
		}
	}
}

void printDetectedBeats(int i) {
		// These print statements will print the beat status of each bin
		if (i == numFFTBins - 1) Serial.println(beatDetected[i]);
		else {
			Serial.print(beatDetected[i]);
			Serial.print("\t");
		}
}

void scoreBins(int i) {
	// If there's a constant beat, increase score by 2
	if (constantBeatCounter[i] > 5)
		binScore[i] += 2;

	// If there's a beat detected, increase score by 1
	if (beatDetected[i] == 2)
		binScore[i] += 1;

	// If the highest scoring bin has it's constant beat stop, lower that score
	if (binScore[i] > 300 && constantBeatCounter[i] < 2)
		binScore[i] -= 2;

	// These print statements will print the score of each bin
	//if (i == numFFTBins - 1)
	//	Serial.println(binScores[i]);
	//else {
	//	Serial.print(binScores[i]);
	//	Serial.print("\t");
	//}

}

void musicAnalytics(bool autoSwitch) {
	
	// All the deep analytics happen from the functions in this for loop
	for (int i = 0; i < 14; i++) {
		beatDetection(i);
		//beatTiming(i);
		//printDetectedBeats(i);
		scoreBins(i);
	}
	
	// This for loop checks the lowest 4 bins, mid 4 bins, high 4 bins
	for (int i = 0; i < 4; i++) {
		// Find the beats in the low, mid, and high ranges
		if (binScore[i] > binScore[lowBeatBin])
			lowBeatBin = i;
		if (binScore[i + 5] > binScore[midBeatBin]) // binScore[4] is not measured
			midBeatBin = i + 5;
		if (binScore[i + 9] > binScore[highBeatBin]) 
			highBeatBin = i + 9;

		// Find highest averages in the low, mid, and high ranges
		if (average[i] > average[lowAveBin])
			lowAveBin = i;
		if (average[i + 5] > average[midAveBin])
			midAveBin = i + 5;
		if (average[i + 9] > average[highAveBin])
			highAveBin = i + 9;
	}

	// Prints all the beat bins
	//Serial.print(lowBeatBin);
	//Serial.print("\t");
	//Serial.print(binScore[lowBeatBin]);
	//Serial.print("\t");
	//Serial.print(midBeatBin);
	//Serial.print("\t");
	//Serial.print(binScore[midBeatBin]);
	//Serial.print("\t");
	//Serial.print(highBeatBin);
	//Serial.print("\t");
	//Serial.println(binScore[highBeatBin]);

	static int mixAmountInfluencer;

	//if (beatDetected[2] == 2 || beatDetected[12] == 2)
	if (beatDetected[lowBeatBin] == 2 || constantBeatCounter[lowBeatBin] > 8)
		mixAmountInfluencer += 5;

	EVERY_N_MILLIS(150)
		mixAmountInfluencer -= 1;

	mixAmountInfluencer = constrain(mixAmountInfluencer, -10, 10);
	mixAmount = constrain(mixAmount + mixAmountInfluencer, 0, 255);
	//Serial.print(mixAmountInfluencer);
	//Serial.print("\t");
	//Serial.println(mixAmount);


	// If there is beat detected in 1 of these 3 frequencies, then increment their counter
	if (beatDetected[2] == 2) lowBeatCounter++;
	if (beatDetected[8] == 2) midBeatCounter++;
	if (beatDetected[12] == 2) highBeatCounter++;

	if (musicState == 0) { // If there is silence and any of the counters go from 0 to 1, a music reactive mode starts
		if (lowBeatCounter > 0 || midBeatCounter > 0 || highBeatCounter > 0) musicState = 3;
	}

	EVERY_N_BSECONDS(10) {
		musicStateOld = musicState;

		lowBeats = (lowBeatCounter > 5) ? true : false;
		midBeats = (midBeatCounter > 5) ? true : false;
		highBeats = (highBeatCounter > 5) ? true : false;

		// Checks to see if there is a constant beat in the low frequency
		if (constantBeatCounter[2] > 8) {
			constBeats = true;
		}
		else constBeats = false;

		// loop() {} uses music states to choose which function list to pull a function from
		//if (autoSwitch) {
		//	if (constBeats) {
		//		musicState = 5;
		//		nextMusicWithConstBeatPattern();
		//	}
		//	// If low and high beats present
		//	else if (lowBeats && highBeats) {
		//		musicState = 3;
		//		nextSpectrumPattern();
		//	}
		//	// If only low beats present
		//	else if (lowBeats) {
		//		musicState = 2;
		//		nextLowBeatPattern();
		//	}
		//	// If silent
		//	else if (silence) {
		//		musicState = 0;
		//		nextAmbPattern();
		//	}
		//	// If sound but no beats
		//	else if (!silence && !lowBeats && !midBeats && !highBeats) {
		//		musicState = 1;
		//		nextMusicWithNoBeatPattern();
		//	}
		//	// If just middle or just high beats present
		//	else {
		//		musicState = 1;
		//		nextMusicWithNoBeatPattern();
		//	}

		//	switch (musicStateOld) {
		//	case 0:
		//		nextAmbPattern();
		//		break;
		//	case 1:
		//		nextMusicWithNoBeatPattern();
		//		break;
		//	case 2:
		//		while (lowBeatCounter > 10 && (currentLowBeatPatternNumber == 1 || currentLowBeatPatternNumber == 3)) {
		//			nextLowBeatPattern();
		//		}
		//		break;
		//	case 3:
		//		nextSpectrumPattern();
		//		break;
		//	}

		//	// Reset counters
		//	lowBeatCounter = 0;
		//	midBeatCounter = 0;
		//	highBeatCounter = 0;

		//	initiate = true;
		//}
	}
}

//
//void audioSetup() {
//	if (digitalRead(switchPin) == 1) {
//		myInput = AUDIO_INPUT_LINEIN;
//		Serial.println("Input is Aux");
//	}
//	else {
//		myInput = AUDIO_INPUT_MIC;
//		Serial.println("Input is Mic");
//	}
//	// AUDIO STUFF
//	AudioMemory(12);
//	audioShield.enable();
//	audioShield.inputSelect(myInput);
//
//	// configure the mixer to equally add left & right
//	mixer1.gain(0, 0.5);
//	mixer1.gain(1, 0.5);
//}
//
//// Clears the running mean and st dev. Used mostly between songs when silence is detected.
//void clearStats() {
//	for (int i = 0; i < 14; i++) {
//		freq[i].clear();
//		beatPeak[i] = 0;
//	}
//}
//
//void fillStats() {
//	spectrumValue[0] = fft1024.read(0) * 1000;
//	spectrumValue[1] = fft1024.read(1) * 1000;
//	spectrumValue[2] = fft1024.read(2, 3) * 1000;
//	spectrumValue[3] = fft1024.read(4, 6) * 1000;
//	spectrumValue[4] = fft1024.read(7, 10) * 1000;
//	spectrumValue[5] = fft1024.read(11, 15) * 1000;
//	spectrumValue[6] = fft1024.read(16, 22) * 1000;
//	spectrumValue[7] = fft1024.read(23, 32) * 1000;
//	spectrumValue[8] = fft1024.read(33, 46) * 1000;
//	spectrumValue[9] = fft1024.read(47, 66) * 1000;
//	spectrumValue[10] = fft1024.read(67, 93) * 1000;
//	spectrumValue[11] = fft1024.read(94, 131) * 1000;
//	spectrumValue[12] = fft1024.read(132, 184) * 1000;
//	spectrumValue[13] = fft1024.read(185, 257) * 1000;
//
//	volume = 0;
//	uint8_t silenceCounter = 0;
//	// Adjust the standard deviation and average of each array value as the song goes
//	for (int i = 0; i < 14; i++) {
//		freq[i].add(spectrumValue[i]);
//		average[i] = freq[i].average();
//		stDev[i] = freq[i].pop_stdev();
//		volume = volume + spectrumValue[i];
//
//		if (spectrumValue[i] < 8) // 8 is an arbitrary number but it means it is basically silent
//			silenceCounter++;
//		else 
//			break;
//	}
//
//	if (silenceCounter > 11) {
//		//Serial.println("stats have been cleared");
//		songDuration = 0;
//		clearStats();
//	}
//	else 
//		silenceDuration = 0;
//
//	// If song is over 300000ms, 5 minutes, clear the stats. DJs have songs blend together which would never clear the stats
//	if (songDuration > 300000) {
//		songDuration = 0;
//		clearStats();
//	}
//
//	// If there's 5 seconds of silence, go in to ambient mode
//	if (silenceDuration > 5000) {
//		silence = true;
//		lowBeats = false;
//		midBeats = false;
//		highBeats = false;
//		songDuration = 0;
//	}
//	else silence = false;
//}
//
//void beatTiming(int i) {
//		if (beatDetected[i] == 2) { // if beat has been detected
//			if (beatTimer[i] < 400) // and it isn't 200 ms from the last one
//				beatDetected[i] = 1; // but if it is, cancel the beat
//			else {
//				beatInterval[i] = beatTimer[i];  // a beat has been detected. Get the time since last beat.
//				// could use absolute value below
//				if ((beatIntervalOld[i] - beatInterval[i]) < 30 && (beatIntervalOld[i] - beatInterval[i]) > -30) // If the time between the new interval and old interval is less than 30 ms 
//					constantBeatCounter[i]++;
//				//else 
//				//	constantBeatCounter[i] = 0;
//
//				beatTimer[i] = 0;
//				beatIntervalOld[i] = beatInterval[i];
//			}
//		}
//		if (constantBeatCounter[i] > 0 && beatTimer[i] > beatInterval[i] + 50) constantBeatCounter[i] = 0; // clears beat counter when more time than the beat has passed
//
//		// These print statements will print the constant beat counter of each bin
//		//if (i == numFFTBins -1) Serial.println(constantBeatCounter[i]);
//		//else {
//		//	Serial.print(constantBeatCounter[i]);
//		//	Serial.print("\t");
//		//}
//}
//
//int fftBinWithBeat;
//bool constBeat = false;
////bool beatHitMean[numFFTBins] = { false };
//
////int beatCriteria(int j) {
////	if (spectrumValue[j])
////}
//
//void beatDetection(bool autoSwitch) {
//	// 0 = no beat detected
//	// 1 = beat hasn't dropped / reset yet
//	// 2 = beat detected
//	for (int i = 0; i < 14; i++) {
//
//		// This is first because beats were going from 0 to 1 when it was at the end of this for loop
//		beatTiming(i);
//
//		// Beat is detected here. Must be greater than the average+(2.3*st.dev) and greater than 100 which is pretty quiet
//		if (spectrumValue[i] > average[i] + 2.3 * stDev[i] && spectrumValue[i] > 100) {
//			if (beatDetected[i] == 2)
//				beatDetected[i] = 1;
//			if (beatDetected[i] == 0)
//				beatDetected[i] = 2;
//			// if it's == 1, it stays 1.
//		}
//		// This is where 1's get reset to 0. If beat is not detected in that frequency, set it's status to 0.
//		else {
//			if (beatDetected[i] == 1) {
//				// else, don't be ready for the next beat until the frequency has hit the running average.
//				// This prevents multiple beats being triggered when they aren't well defined
//				if (spectrumValue[i] <= average[i])
//					beatDetected[i] = 0;
//			}
//		}
//		//beatTiming(i);
//	}
//
//	Serial.print(beatDetected[0]);
//	Serial.print("\t");
//	Serial.print(beatDetected[1]);
//	Serial.print("\t");
//	Serial.print(beatDetected[2]);
//	Serial.print("\t");
//	Serial.println(beatDetected[3]);
//
//	uint8_t highestAveLow = 0;
//	uint8_t highestAveMid = 0;
//
//	uint8_t constBeatIndex[3] = { 0 };
//	uint8_t highestAveIndex[3] = { 0 };
//
//	for (int i = 0; i < numFFTBins; i++) {
//		if (i <= 5) {
//			if (constantBeatCounter[i] > 9 && constantBeatCounter[i] > constantBeatCounter[constBeatIndex[0]])
//				constBeatIndex[0]= i;
//			if (average[i] > average[highestAveIndex[0]])
//				highestAveIndex[0] = i;
//		}
//		else if (i < 5 && i <= 10) {
//			if (constantBeatCounter[i] > 9 && constantBeatCounter[i] > constantBeatCounter[constBeatIndex[1]])
//				constBeatIndex[1] = i;
//			if (average[i] > average[highestAveIndex[1]])
//				highestAveIndex[1] = i;
//		}
//		else {
//			if (constantBeatCounter[i] > 9 && constantBeatCounter[i] > constantBeatCounter[constBeatIndex[2]])
//				constBeatIndex[2] = i;
//			if (average[i] > average[highestAveIndex[2]])
//				highestAveIndex[2] = i;
//		}
//	}
//
//	//for (int i = 0; i < numFFTBins; i++) {
//	//	Serial.print(constantBeatCounter[i]);
//	//	Serial.print("\t");
//	//}
//	//Serial.print(constBeatIndex[0]);
//	//Serial.print("\t");
//	//Serial.print(constBeatIndex[1]);
//	//Serial.print("\t");
//	//Serial.println(constBeatIndex[2]);
//
//	// If there is beat detected in 1 of these 3 frequencies, then increment their counter
//	if (beatDetected[2] == 2) lowBeatCounter++;
//	if (beatDetected[8] == 2) midBeatCounter++;
//	if (beatDetected[12] == 2) highBeatCounter++;
//
//	if (musicState == 0) { // If there is silence and any of the counters go from 0 to 1, a music reactive mode starts
//		if (lowBeatCounter > 0 || midBeatCounter > 0 || highBeatCounter > 0) musicState = 3;
//	}
//
//	EVERY_N_BSECONDS(10) {
//		musicStateOld = musicState;
//
//		lowBeats = (lowBeatCounter > 5) ? true : false;
//		midBeats = (midBeatCounter > 5) ? true : false;
//		highBeats = (highBeatCounter > 5) ? true : false;
//
//		// Checks to see if there is a constant beat in the low frequency
//		if (constantBeatCounter[2] > 8) {
//			constBeats = true;
//		}
//		else constBeats = false;
//
//		// loop() {} uses music states to choose which function list to pull a function from
//		if (autoSwitch) {
//			if (constBeats) {
//				musicState = 5;
//				nextMusicWithConstBeatPattern();
//			}
//			// If low and high beats present
//			else if (lowBeats && highBeats) {
//				musicState = 3;
//				nextSpectrumPattern();
//			}
//			// If only low beats present
//			else if (lowBeats) {
//				musicState = 2;
//				nextLowBeatPattern();
//			}
//			// If silent
//			else if (silence) {
//				musicState = 0;
//				nextAmbPattern();
//			}
//			// If sound but no beats
//			else if (!silence && !lowBeats && !midBeats && !highBeats) {
//				musicState = 1;
//				nextMusicWithNoBeatPattern();
//			}
//			// If just middle or just high beats present
//			else {
//				musicState = 1;
//				nextMusicWithNoBeatPattern();
//			}
//
//			switch (musicStateOld) {
//			case 0:
//				nextAmbPattern();
//				break;
//			case 1:
//				nextMusicWithNoBeatPattern();
//				break;
//			case 2:
//				while (lowBeatCounter > 10 && (currentLowBeatPatternNumber == 1 || currentLowBeatPatternNumber == 3)) {
//					nextLowBeatPattern();
//				}
//				break;
//			case 3:
//				nextSpectrumPattern();
//				break;
//			}
//
//			// Reset counters
//			lowBeatCounter = 0;
//			midBeatCounter = 0;
//			highBeatCounter = 0;
//
//			initiate = true;
//		}
//	}
//}

void printNumber(float n) {
	if (n >= 0.004) {
		Serial.print(n, 3);
		Serial.print(" ");
	}
	else {
		//  Serial.print("   -  "); // don't print "0.00"
		Serial.print("   0  "); // print 0 for excel purposes
	}
}

void printSpectrum() {
	////// UNCOMMENT THEse LINES TO PRINT THE FFT WHILE PLAYING A SONG
	if (fft1024.available()) {
		// each time new FFT data is available
		// print to the Arduino Serial Monitor
		//Serial.print("FFT: ");
		printNumber(fft1024.read(0) * 1000);
		printNumber(fft1024.read(1) * 1000);
		printNumber(fft1024.read(2, 3) * 1000);
		printNumber(fft1024.read(4, 6) * 1000);
		printNumber(fft1024.read(7, 10) * 1000);
		printNumber(fft1024.read(11, 15) * 1000);
		printNumber(fft1024.read(16, 22) * 1000);
		printNumber(fft1024.read(23, 32) * 1000);
		printNumber(fft1024.read(33, 46) * 1000);
		printNumber(fft1024.read(47, 66) * 1000);
		printNumber(fft1024.read(67, 93) * 1000);
		printNumber(fft1024.read(94, 131) * 1000);
		printNumber(fft1024.read(132, 184) * 1000);
		printNumber(fft1024.read(185, 257) * 1000);
		printNumber(fft1024.read(258, 359) * 1000);
		printNumber(fft1024.read(360, 511) * 1000);
		Serial.println();
	}
}

void audioAnalyticsDiag() {

}