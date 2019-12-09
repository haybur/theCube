//////// Animations.h

int sparkleNumber;
int maxNumOfSparkles = 25;

void sparkles(int sparkStart, int sparkEnd, uint8_t freq, double multiplierAdjustment, int additionAdjustment, uint8_t hue, bool white) {
	//static uint8_t sparkleSat = (white) ? 255 : 0;
	static int sparkleSat;
	if (white) sparkleSat = 0;
	else sparkleSat = 255;

	sparkleNumber = spectrumValue[freq] + additionAdjustment;
	sparkleNumber = constrain(sparkleNumber, 0, 500);
	int brightness = map(sparkleNumber, 0, 400, 130, 255);
	//brightness = 255;
	sparkleNumber = map(sparkleNumber, 0, 500, 0, maxNumOfSparkles);
	sparkleNumber = sparkleNumber * multiplierAdjustment;

	for (int i = 0; i < sparkleNumber; i++) {
		int tempPixelNumber = random16(sparkStart, sparkEnd);

		if (white)
			leds[tempPixelNumber] = CHSV(0, 0, brightness);
		else
			leds[tempPixelNumber] = CHSV(hue, 255, brightness);
		//leds[tempPixelNumber] = ColorFromPalette(currentPalette, hue, 100, currentBlending);
	}
	//fadeValue = fadeValue + sparkleNumber * .5;
	//Serial.print(sparkleNumber);
	//Serial.print("\t");
	//Serial.println(fadeValue);
}

void fillVolumeBar(int freq, int bar, int hue) {
	static int tempHeight;
	static int barHeights[5];

	tempHeight = constrain(map(spectrumValue[freq], 20, 500, 1, 18), 1, 18);
	if (tempHeight > barHeights[bar])
		barHeights[bar] = tempHeight;
	else {
		if (barHeights[bar] > 1)
			barHeights[bar]--;
	}

	// BOTTOM UP BARS
	//for (int i = 0; i < height; i++) {
	//	if (i < barHeights[bar])
	//		setVoxel(4, bar, i, CHSV(hue, 255, 255));
	//	else
	//		setVoxel(4, bar, i, CHSV(hue, 0, 0));
	//}

	// TOP DOWN BARS
	for (int i = 0; i < height; i++) {
		if (i > 17 - barHeights[bar])
			setVoxel(4, bar, i, CHSV(hue, 255, 255));
		else
			setVoxel(4, bar, i, CHSV(hue, 0, 0));
	}
}

void volumeBarsIn3D() {
	EVERY_N_MILLIS(120) {
		// MOVE BARS BACKWARDS
		for (int i = 0; i < rows - 1; i++) {
			for (int j = 0; j < columns; j++) {
				for (int k = 0; k < height; k++) {
					setVoxel(i, j, k, leds[coords[i + 1][j][k]]);
				}
			}
		}
	}

	//fadeToBlackBy(leds, NUM_LEDS_PER_STRIP * NUM_STRIPS, 20);

	fillVolumeBar(2, 4, 0);
	fillVolumeBar(5, 3, 50);
	fillVolumeBar(8, 2, 96);
	fillVolumeBar(10, 1, 150);
	fillVolumeBar(12, 0, 210);
}


void volumeBall() {
	static int ballRadius;
	static int newRadius;

	//newRadius = constrain(volume, 20, 3000); // This isn't the new radius at all, just using it here cause it's redefined in next line
	//newRadius = map(volume, 20, 3000, 1, 13);

	newRadius = constrain(spectrumValue[2], 50, 500);
	newRadius = map(newRadius, 50, 500, 1, 10);
	if (newRadius > ballRadius)
		ballRadius = newRadius;

	EVERY_N_MILLIS(10) ballRadius--;

	constrain(ballRadius, 0, 25);
	
	drawBall(2, 2, 8, ballRadius, CHSV(0, 255, 255));
	fadeAll(160); 
	
}

void growingBalls(int freq, int index, int x, int y, int z, CRGB color) {
	//static double speed[3];
	//static elapsedMillis time[3];
	static int radii[3];
	static int temp;

	temp = constrain(spectrumValue[freq], 30, 500);
	temp = map(temp, 100, 500, 0, 15);
	if (temp > radii[index]) 
		radii[index] = temp;

	radii[index]--;

	constrain(radii[index], 0, 8);

	//drawShell(x, y, z, radii[index] + 3, 4, CHSV(100, 255, 255));
	drawBall(x, y, z, radii[index], color);
}

void twoBalls() {
	color = CHSV(0, 255, 255);
	growingBalls(2, 0, 2, 2, 0, color);
	color = CHSV(210, 255, 255);
	growingBalls(12, 1, 2, 2, 18, color);

	maxNumOfSparkles = 25;
	sparkles(0, NUM_LEDS, 8, 1, -50, 0, true);

	fadeAll(170);
}

void allBalls() {
	color = CHSV(0, 255, 255);
	growingBalls(2, 0, 4, 4, 0, color);
	color = CHSV(96, 255, 255);
	growingBalls(midBeatBin, 1, 0, 0, 0, color);
	color = CHSV(160, 255, 255);
	growingBalls(highBeatBin, 2, 4, 0, 17, color);

	fadeAll(200);
}

void shrinkingShells(int freq, int index, int x, int y, int z, CRGB color) {
	static double speed[3];
	static elapsedMillis time[3];
	static int radii[3];

	if (beatDetected[freq] == 2) {
		radii[index] = 15;
		time[index] = 0;
		speed[index] -= .5 * speed[index];
	}

	if (time[index] > speed[index]) {
		if (radii[index] < 30) {
			radii[index]--;
			time[index] = 0;
		}
	}
	drawShell(x, y, z, radii[index], 1, color);
	speed[index] = constrain(speed[index] + .1 + .02*speed[index], 20, 60);
}

void shrinkingShellsToBeat(int lowFreq, int midFreq, int highFreq) {
	color = CHSV(0, 255, 255);
	shrinkingShells(lowBeatBin, 0, 2, 2, 8, color);
	color = CHSV(100, 255, 255);
	shrinkingShells(midBeatBin, 1, 2, 2, 8, color);
	color = CHSV(200, 255, 255);
	shrinkingShells(highBeatBin, 2, 2, 2, 8, color);
}

void sparklesPattern() {
	maxNumOfSparkles = 100;
	//brightness = 255;
	sparkles(0, NUM_LEDS, 1, 1, -50, 0, false);
	sparkles(0, NUM_LEDS, 2, 1, -50, 15, false);
	sparkles(0, NUM_LEDS, 4, 1, -50, 64, false);
	sparkles(0, NUM_LEDS, 6, 1, -50, 96, false);
	sparkles(0, NUM_LEDS, 9, 1, -50, 160, false);
	sparkles(0, NUM_LEDS, 12, 1, -50, 200, false);

	int fade = (fadeVals[lowAveBin] + fadeVals[midAveBin] + fadeVals[highAveBin]) / 3;
	int noFadeFading = map(fade, 0, 255, 1, 50) * -1 + 255;
	//Serial.println(noFadeFading);
	fadeAll(noFadeFading);
}

void movingLevels() {
	const int numOfLevels = 5;
	static int lowLevels[numOfLevels];
	static int highLevels[numOfLevels];
	static int tempIndex;

	if (beatDetected[2] == 2) {
		for (int i = 0; i < numOfLevels; i++) {
			if (lowLevels[i] > 18) {
				tempIndex = i;
			}
		}
		lowLevels[tempIndex] = 0;
	}
	if (beatDetected[12] == 2) {
		for (int i = 0; i < numOfLevels; i++) {
			if (highLevels[i] > 18) {
				tempIndex = i;
			}
		}
		highLevels[tempIndex] = 0;
	}

	EVERY_N_MILLIS(20) {
		for (int i = 0; i < numOfLevels; i++) {
			if (lowLevels[i] < 19) lowLevels[i]++;
			if (highLevels[i] < 19) highLevels[i]++;
		}
	}

	for (int i = 0; i < numOfLevels; i++) {
		if (lowLevels[i] < 19) {
			for (int j = 0; j < rows; j+=2) {
				for (int k = 0; k < columns; k++) {
					setVoxel(j, k, lowLevels[i], CHSV(0, 255, 255));
				}
			}
		}
		if (highLevels[i] < 19) {
			for (int j = 1; j < rows; j+=2) {
				for (int k = 0; k < columns; k++) {
					setVoxel(j, k, 18 - highLevels[i], CHSV(196, 255, 255));
				}
			}
		}
	}

	maxNumOfSparkles = 25;
	sparkles(0, NUM_LEDS, 8, 1, -50, 0, true);

	fadeAll(175);
}

//
//void circles() {
//	if (beatDetected[2] == 2) {
//		radius = 0;
//		time = 0;
//		speed -= .50*speed;
//	}
//
//	if (time > speed) {
//		if (radius < 40 + 2)
//			radius = radius + .5;
//		time = 0;
//	}
//
//	speed = constrain(speed + .12, 0, 40);  // old is .1 and adding .15
//	color = CHSV(map(speed, 0, 40, 0, 210) * -1 + 210, random8(150, 255), 255);
//	propagateCircle(color, 0, 0);
//
//	int fade = map(fadeVals[lowBeatBin], 0, 255, 1, 10) * -1 + 255;
//
//	pillarSparkles(0, 132, 11, .4, -50, 0, true);
//	fadeAll(253);
//}
//
//
//
//void volumeBars(int freq, int pillar, bool white) { // pillar is 0-3.
//	int preBarLength = map(spectrumValue[freq], 0, 600, 0, 32);
//	preBarLength = constrain(preBarLength, 0, 31);
//	static int barLength[4];
//	static int topDot[4];
//
//	if (preBarLength > barLength[pillar]) {
//		barLength[pillar] = preBarLength;
//		if (barLength[pillar] > topDot[pillar])
//			topDot[pillar] = barLength[pillar] + 1;
//	}
//	else {
//		barLength[pillar]--;
//	}
//
//	if (pillar == 0) {
//		for (int i = 0; i < barLength[pillar]; i++) {
//			//leds[i] = CHSV(i * 7, 255, 255);
//			leds[i] = CHSV(199, 255, 255);
//		}
//		leds[topDot[pillar]] = CHSV(0, 0, 130);
//	}
//	else if (pillar == 1) {
//		for (int i = 0; i < barLength[pillar]; i++) {
//			//leds[65 - i] = CHSV(i * 7, 255, 255);
//			leds[65 - i] = CHSV(120, 255, 255);
//		}
//		leds[65 - topDot[pillar]] = CHSV(0, 0, 130);
//	}
//	else if (pillar == 2) {
//		for (int i = 0; i < barLength[pillar]; i++) {
//			//leds[66 + i] = CHSV(i * 7, 255, 255);
//			leds[66 + i] = CHSV(45, 255, 255);
//		}
//		leds[66 + topDot[pillar]] = CHSV(0, 0, 130);
//	}
//	else {
//		for (int i = 0; i < barLength[pillar]; i++) {
//			//leds[132 - i] = CHSV(i * 7, 255, 255);
//			leds[132 - i] = CHSV(0, 255, 255);
//		}
//		leds[131 - topDot[pillar]] = CHSV(0, 0, 130);
//	}
//
//	Serial.println(topDot[1]);
//
//	for (int i = 0; i < barLength[pillar]; i++) {
//		//leds[(pillar * 200) + i] = ColorFromPalette(currentPalette, i * 2.5, 100, currentBlending);
//		//leds[((pillar) * 200 + 78) - i] = CHSV(i * 3, 255, 150);
//		//leds3[((pillar) * 200 + 78) - i] = ColorFromPalette(currentPalette, hue, 100, currentBlending);
//		//leds3[((pillar) * 200) + i] = ColorFromPalette(currentPalette, i * 2.5, 100, currentBlending);
//	}
//
//	EVERY_N_MILLIS(40) {
//		for (int i = 0; i < 4; i++) {
//			if (topDot[i] >= 1)
//				topDot[i]--;
//		}
//	}
//		//topDot--;
//	//leds[(pillar * 200) + topDot[pillar]] = CHSV(0, 0, 150);
//}
//
//void volumeBarMode() {
//	volumeBars(12, 0, false);
//	volumeBars(10, 1, false);
//	volumeBars(4, 2, false);
//	volumeBars(2, 3, false);
//
//	fadeAll(250);
//}

//
//void explosions(int freq, int begin, int end) {                                                                 // Display ripples triggered by peaks.
//	static const int maxsteps = 25;                                               // Maximum number of steps.
//	static uint8_t colour;                                                        // Ripple colour is based on samples.
//	static uint16_t center = 0;                                                   // Center of current ripple.
//	static int8_t step = -1;                                                      // Phase of the ripple as it spreads out.
//
//	if (beatDetected[freq] == 2)
//		step = -1;														// Trigger a new ripple if we have a peak
//
//	//fadeToBlackBy(leds, begin, end, 7);                                            // Fade the strand, where 1 = slow, 255 = fast
//	//fadeRange(7, begin, end);
//
//	switch (step) {
//
//	case -1:                                                                    // Initialize ripple variables. By Andrew Tuline.
//		center = random16(begin, end);
//		colour = (freq - 2) * 17;
//		step = 0;
//		break;
//
//	case 0:																		// Display the first pixel of the ripple.
//		holder[center] = CHSV(colour, 125, 255);
//		step++;
//		break;
//
//	case maxsteps:                                                              // At the end of the ripples.
//	  // step = -1;
//		break;
//
//	default:                                                                    // Middle of the ripples.
//		holder[center + step] = CHSV(colour + step * 2, 255, 255 - step * 6);
//		holder[center - step] = CHSV(colour + step * 2, 255, 255 - step * 6);
//		step++;                                                                  // Next step.
//		break;
//	} // switch step
//
//	fadeRange(3, 0, 78);
//} // ripple()
//
//void segPillarSparkling() {
//	maxNumOfSparkles = 25;
//	brightness = 255;
//	pillarSparkles(0, 78, 1, .2, -50, 0, false);
//	pillarSparkles(0, 78, 2, .15, -50, 32, false);
//
//	pillarSparkles(200, 278, 3, .10, -50, 50, false);
//	pillarSparkles(200, 278, 4, .10, -50, 60, false);
//	pillarSparkles(200, 278, 6, .10, -50, 70, false);
//
//	pillarSparkles(400, 478, 8, .15, -50, 100, false);
//	pillarSparkles(400, 478, 9, .15, -50, 128, false);
//
//	pillarSparkles(600, 678, 11, .3, -50, 160, false);
//	pillarSparkles(600, 678, 12, .3, -50, 196, false);
//
//	int fade = (fadeVals[lowAveBin] + fadeVals[midAveBin] + fadeVals[highAveBin]) / 3;
//	int noFadeFading = map(fade, 0, 255, 1, 10) * -1 + 255;
//
//	fadeRange2(noFadeFading, 0, 800);
//}
//
//void sparklesNoFade() {
//	maxNumOfSparkles = 25;
//	brightness = 255;
//	pillarSparkles(0, 800, 1, .4, -50, 0, false);
//	pillarSparkles(0, 800, 2, .3, -50, 10, false);
//	pillarSparkles(0, 800, 6, .3, -50, 80, false);
//	pillarSparkles(0, 800, 9, .3, -50, 120, false);
//	pillarSparkles(0, 800, 12, .3, -50, 160, false);
//
//	int fade = (fadeVals[lowAveBin] + fadeVals[midAveBin] + fadeVals[highAveBin]) / 3;
//	int noFadeFading = map(fade, 0, 255, 0, 10) * -1 + 255;
//
//	fadeRange2(noFadeFading, 0, 800);
//}
//
//void layeredPillarSparkles() {
//	maxNumOfSparkles = 25;
//	//brightness = 255;
//	EVERY_N_MILLIS(50)
//		hue++;
//
//	for (int i = 0; i < 4; i++) {
//		//pillarSparkles(48 + (78 * i), 78 + (78 * i), 2, .15, -50, 0 + hue, false);
//		//pillarSparkles(22 + (78 * i), 52 + (78 * i), 8, .15, -50, 96 + hue, false);
//		//pillarSparkles(0 + (78 * i), 30 + (78 * i), 12, .15, -50, 160 + hue, false);
//		pillarSparkles(48 + (NUM_LEDS_PER_STRIP * i), 78 + (NUM_LEDS_PER_STRIP * i), 2, .15, -50, 0 + hue, false);
//		pillarSparkles(22 + (NUM_LEDS_PER_STRIP * i), 52 + (NUM_LEDS_PER_STRIP * i), 8, .15, -50, 96 + hue, false);
//		pillarSparkles(0 + (NUM_LEDS_PER_STRIP * i), 30 + (NUM_LEDS_PER_STRIP * i), 12, .15, -50, 160 + hue, false);
//	}
//
//	// Fading happens here
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 26; j++) {
//			leds2[(j + 0 ) + (200 * i)].nscale8(map(fadeVals[2], 0, 255, 0, 10) * -1 + 254);
//			leds2[(j + 26) + (200 * i)].nscale8(map(fadeVals[8], 0, 255, 0, 10) * -1 + 254);
//			leds2[(j + 52) + (200 * i)].nscale8(map(fadeVals[12], 0, 255, 0, 10) * -1 + 254);
//		}
//	}
//}
//
//int topDot;
//
//void volumeBars(int freq, int pillar, bool white) { // pillar is 0-3.
//	int preBarLength = map(spectrumValue[freq], 0, 600, 0, 78);
//	preBarLength = constrain(preBarLength, 0, 78);
//	static int barLength[4];
//
//	if (preBarLength > barLength[pillar]) {
//		barLength[pillar] = preBarLength;
//		if (barLength[pillar] > topDot)
//			topDot = barLength[pillar] + 1;
//	}
//	else {
//		barLength[pillar]--;
//	}
//		
//	for (int i = 0; i < barLength[pillar]; i++) {
//		if (white)
//			leds3[(pillar * 200) + i] = CHSV(0, 0, 150);
//		else 
//			leds3[(pillar * 200) + i] = ColorFromPalette(currentPalette, i * 2.5, 100, currentBlending);
//		//leds[((pillar) * 200 + 78) - i] = CHSV(i * 3, 255, 150);
//		//leds3[((pillar) * 200 + 78) - i] = ColorFromPalette(currentPalette, hue, 100, currentBlending);
//		//leds3[((pillar) * 200) + i] = ColorFromPalette(currentPalette, i * 2.5, 100, currentBlending);
//	}
//	if (white)
//		leds3[(pillar * 200) + topDot] = CHSV(topDot * 3, 255, 250);
//	else
//		leds3[(pillar * 200) + topDot] = CHSV(0, 0, 150);
//}
//
//void volumeBarMode(int freq, bool white) {
//	volumeBars(freq, 0, white);
//	volumeBars(freq, 3, white);
//	volumeBars(freq, 1, white);
//	volumeBars(freq, 2, white);
//
//	EVERY_N_MILLIS(40)
//		topDot--;
//
//	fadeToBlackBy(leds3, 800, 4);
//}
//
//void fallingColorCreation(int freq) {
//	static int fallingBrightness;
//	static int threshold = 100;
//	if (spectrumValue[freq] > average[freq] + stDev[freq] && spectrumValue[freq] > threshold) {
//		fallingBrightness = map(constrain(spectrumValue[freq], 0, average[freq] + 3 * stDev[freq]), 0, average[freq] + stDev[freq], 0, 255);
//		if (freq == 2) fallingBrightness += 50;
//		leds3[0] += ColorFromPalette(currentPalette, map(freq, 2, 12, 0, 180), fallingBrightness, currentBlending);
//	}
//}
//
//void fallingMusic() {
//
//	leds3[0] = 0;
//	fallingColorCreation(2);
//	fallingColorCreation(4);
//	fallingColorCreation(6);
//	fallingColorCreation(8);
//	fallingColorCreation(10);
//	fallingColorCreation(12);
//
//	// Copy pillar to the other 3
//	for (int i = 1; i < 4; i++) {
//		for (int j = 0; j < 78; j++) {
//			leds3[(i * 200) + j] = leds3[j];
//		}
//	}
//
//	EVERY_N_MILLIS(9) {
//		for (int i = 78; i > 0; i--) {
//			leds3[i] = leds3[i - 1];
//			leds3[i].subtractFromRGB(3);
//		}
//	}
//	//blend3(1, 255);
//}
//
//void outsideMusicNoteSparkles() {
//	pillarSparkles(800, 958, lowBeatBin, .2, -50, 0, true);
//	pillarSparkles(800, 958, midBeatBin, .2, -50, 0, true);
//	pillarSparkles(800, 958, highBeatBin, .2, -50, 0, true);
//	pillarSparkles(800, 958, lowBeatBin, .2, -50, 210, false);
//	pillarSparkles(800, 958, midBeatBin, .2, -50, 210, false);
//	pillarSparkles(800, 958, highBeatBin, .2, -50, 210, false);
//
//	int sparkleFading = (fadeVals[midBeatBin] + fadeVals[midBeatBin] + fadeVals[midBeatBin]) / 3;
//	for (int i = 800; i < 958; i++) {
//		leds2[i].fadeToBlackBy(map(sparkleFading, 0, 255, 1, 15));
//	}
//	blend2(0, 255);
//}
//
//void volumeBarModeWhite() {
//	volumeBarMode(lowBeatBin, 1);
//}
//
//void volumeBarModeColors() {
//	volumeBarMode(lowBeatBin, 0);
//}
//
//void radialToBeatColors() {
//	radialToBeat(0, 1, 160);
//	highFreqDucentiSparkles();
//}
//
//void radialToBeatWhite() {
//	radialToBeat(1, 0, 200);
//}
//
//void mirrorBallSparkles() {
//	pillarSparkles(800, 1000, lowBeatBin, .2, -50, 0, true);
//	pillarSparkles(800, 1000, midBeatBin, .2, -50, 0, true);
//	pillarSparkles(800, 1000, highBeatBin, .2, -50, 0, true);
//	int mirrorBallFading = (fadeVals[midBeatBin] + fadeVals[midBeatBin] + fadeVals[midBeatBin]) / 3;
//	for (int i = 800; i < 1000; i++) {
//		leds2[i].fadeToBlackBy(map(mirrorBallFading, 0, 255, 1, 15));
//	}
//}
//
//// highFreqDucentiSparkles();
//
