////// drawing.h

//void propagateCircle(CHSV color, int startx, int starty) {
//	for (int i = 0; i < NUM_LEDS; i++) {
//		int xdist = ledx[i] - startx;
//		int ydist = ledy[i] - starty;
//		int dist = sqrt(pow(xdist, 2) + pow(ydist, 2));
//		if (abs(dist - radius) < 1)
//			leds[i] = color;
//		//if (pow(ledx[i] - startx, 2) + pow(ledy[i] - starty, 2) - pow(radius, 2) > -1 && pow(ledx[i] - startx, 2) + pow(ledy[i] - starty, 2) - pow(radius, 2) < 1) leds[i] = color;
//	}
//}

void fadeAll(int fadeVar) {
	for (int i = 0; i < NUM_LEDS; i++) leds[i].nscale8(fadeVar);
}

//// This function blends leds2 in to leds
//void blend2(bool location, int mixAmount) {
//	switch (location) {
//	case 0:
//		for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
//			nblend(leds[800 + i], leds2[800 + i], mixAmount);
//		}
//		break;
//	case 1:
//		for (int i = 0; i < 800; i++) {
//			nblend(leds[i], leds2[i], mixAmount);
//		}
//		break;
//	}
//}
//
//void blend3(bool location, int mixAmount) {
//	switch (location) {
//	case 0:
//		for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
//			nblend(leds[800 + i], leds3[800 + i], mixAmount);
//		}
//		break;
//	case 1:
//		for (int i = 0; i < 800; i++) {
//			nblend(leds[i], leds3[i], mixAmount);
//		}
//		break;
//	}
//}
//
//// This function will blend the non-beat patterns together with the beat patterns, depending on what the music is doing
//void blendingPatterns(bool location, int mixAmount) { // location is 0 = ducenti, 1 = pillars
//	// mixAmount is fed in to this function and is a number determined elsewhere that represents what the song is doing.
//	// The value will drift to 0 naturally. Music with no beat pulls the value negative and when a beat is present, it adds to it.
//	static int tempMix;
//	switch (location) {
//		case 0:
//			if (mixAmount >= 0) {
//				blend3(location, mixAmount);
//				blend2(location, (mixAmount - 255) * -1);
//				//blend2(location, qsub8(mixAmount, 255));
//				//Serial.println("it's working?");
//			}
//			else {
//				tempMix = mixAmount + 255;
//				blend2(location, tempMix);
//				blend3(location, qsub8(tempMix, 255));
//			}
//			break;
//		case 1:
//			if (mixAmount >= 0) {
//				blend3(location, mixAmount);
//				blend2(location, (mixAmount - 255) * -1);
//				//blend2(location, qsub8(mixAmount, 255));
//				//Serial.println("it's working?");
//			}
//			else {
//				tempMix = mixAmount + 255;
//				blend2(location, tempMix);
//				blend3(location, qsub8(tempMix, 255));
//			}
//		}
//
//	//for (int i = 800; i < 1000; i++) 
//	//	nblend(leds[i], leds3[i], 160);
//
//}
//
//void fadeRange(int fadeVar, int start, int stop) {
//	for (int i = start; i < stop; i++) {
//		holder[i].v = qsub8(holder[i].v, fadeVar);
//	}
//}
//
//void fadeRange2(int fadeVar, int start, int stop) {
//	for (int i = start; i < stop; i++)
//		leds2[i].nscale8(fadeVar);
//}
//
//void fadeRange3(int fadeVar, int start, int stop) {
//	for (int i = start; i < stop; i++)
//		leds3[i].nscale8(fadeVar);
//}
//
//void fadeAll(int fadevar) {
//	for (int i = 0; i < NUM_LEDS; i++)
//		//dim8_raw(fadevar);
//		for (int j = 0; j < fadevar; j++)
//			leds[i]--;
//
//		//fadeToBlackBy(fadeVar);
//		//leds[i].nscale8(fadevar);
//}
////
////// the purpose of this function is to:
////// - pick a stretch of leds that you want the sparkles to show up in. between "sparkstart" and "sparkend"
////// - pick a fft bin that you want the sparkles to react to, "freq"
////// - the "multiplieradjustment" and "additionadjustment" variables change how many sparkles appear
//////   - "additionadjustment" gets rid of low volume noise creating sparkles. if the fft bin value is 20, aa is -50, then no sparkles show
////// - "hue" chooses the color of the sparkles
////// - if you want sparkles to be white, as in a color that cannot be choosen with a hue, put true for that argument
////int maxnumofsparkles = 25;
////void addsparkles(int sparkstart, int sparkend, uint8_t freq, double multiplieradjustment, int additionadjustment, int hue, bool white) {
////	// if sparkles are being added to react to music, do this:
////
////	// if the freq argument is a valid fft bin, 0 - 15
////	if (freq > 0 && freq < 15) {
////		sparklenumber = spectrumvalue[freq] + additionadjustment;
////		sparklenumber = constrain(sparklenumber, 0, 500);
////		sparklenumber = map(sparklenumber, 0, 500, 0, maxnumofsparkles);
////		sparklenumber = sparklenumber * multiplieradjustment;
////
////		for (int i = 0; i < sparklenumber; i++) {
////			//leds[getrandompixel(sparkstart, sparkend)] = sparklecolor;
////			int pos = getrandompixel(sparkstart, sparkend);
////			testvar = map(ledx[pos] - ledy[pos], -31, 31, 0, 255);
////			leds[pos] = colorfrompalette(currentpalette, testvar+hue1, brightness, linearblend);
////			if (white) leds[pos] = chsv(0, 0, brightness);
////			fadevalue = fadevalue + sparklenumber*.5;
////		}
////	}
////	// else, if sparkles are being added as an ambient effect, do this:
////	else {
////		every_n_milliseconds(additionadjustment) {
////			int pos = getrandompixel(sparkstart, sparkend);
////			leds[pos] = chsv(hue, 255, brightness);
////			if (white) leds[pos] = chsv(0, 0, brightness);
////		}
////	}
////}
////
////// uses coordmode in sweeps() in animations.h and does the math to propogate the line 
////void linearsweepmath(int coordmode) {
////	if (coordmode == 3 || coordmode == 4) { // this one is rotated 90 degrees so the math works.
////		for (int i = 0; i < num_leds; i++) {
////			if (slope * ledy[i] + yint - ledx[i] < 1 && slope *ledy[i] + yint - ledx[i] > -1) leds[i] = color;
////		}
////	}
////	else {
////		for (int i = 0; i < num_leds; i++) {
////			if (slope * ledx[i] + yint - ledy[i] < 1 && slope *ledx[i] + yint - ledy[i] > -1) leds[i] = color;
////		}
////	}
////}
////
////void propagatecircle(chsv color, int startx, int starty) {
////	for (int i = 0; i < num_leds; i++) {
////		if (pow(ledx[i] - startx, 2) + pow(ledy[i] - starty, 2) - pow(radius, 2) > -10 && pow(ledx[i] - startx, 2) + pow(ledy[i] - starty, 2) - pow(radius, 2) < 10) leds[i] = color;
////	}
////}
////
////void lightside(int start, int stop, bool fade, int h, int s, int v) {
////	if (stop > start) {
////		for (int i = start; i < stop; i++) {
////			if (fade)
////				leds[i] = chsv(h, s, 255 - ((i - start) * 200 / (stop - start)));
////			else
////				leds[i] = chsv(h, s, v);
////		}
////	}
////	else { // start > stop, if you want it to fade backwards
////		for (int i = start; i > stop; i--) {
////			if (fade)
////				leds[i] = chsv(h, s, 255 + ((i - start) * 200 / (start - stop)));
////			else
////				leds[i] = chsv(h, s, v);
////		}
////	}
////}
////
////void lightside(int start, int stop, bool fade, chsv color) {
////	if (stop > start) {
////		for (int i = start; i < stop; i++) {
////			if (fade)
////				leds[i] = chsv(color.h, color.s, 255 - ((i - start) * 200 / (stop - start)));
////			else
////				leds[i] = color;
////		}
////	}
////	else { // start > stop, if you want it to fade backwards
////		for (int i = start; i > stop; i--) {
////			if (fade)
////				leds[i] = chsv(color.h, color.s, 255 + ((i - start) * 200 / (start - stop)));
////			else
////				leds[i] = color;
////		}
////	}
////}
////
////void travelingdotbigsquaredesigns(int mode, int option) {
////	switch (mode) {
////	case 1:
////		for (int i = 0; i < cor1; i++) {                    // creates white lights on corners of the big square. cor1 is used as that's how long the big side is
////			lightside(cor0, cor1, 1, color);
////			lightside(cor28, cor27, 1, color);
////			lightside(cor14, cor15, 1, color);
////			lightside(cor14, cor13, 1, color);
////			lightside(cor2, cor3, 1, color);
////			lightside(cor2, cor1, 1, color);
////			lightside(cor22, cor21, 1, color);
////			lightside(cor22, cor23, 1, color);
////		}
////		break;
////	case 2:
////			if (option == 0) {
////				lightside(cor1, cor0, 1, color);
////				lightside(cor13, cor14, 1, color);
////				lightside(cor2, cor3, 1, color);
////				lightside(cor2, cor1, 1, color);
////			}
////			if (option == 1) {
////				lightside(cor27, cor28, 1, color);
////				lightside(cor15, cor14, 1, color);
////				lightside(cor22, cor21, 1, color);
////				lightside(cor22, cor23, 1, color);
////			}
////		break;
////	}
////}
////
////void travelingtopdot(double &topdotspeed) {
////	if (topdottime > topdotspeed) {
////		for (int i = 0; i < 2; i++) {
////			////////////////// this section makes the top dot rotate clockwise
////		//	if (topdotzones[i] == 1) {
////		//		topdots[i]--;
////		//		if (topdots[i] <= cor15 - 1 && topdots[i] >= cor13) { // i believe the cor21 is useless
////		//			topdots[i] = cor25 - 1;
////		//			topdotzones[i] = 2;
////		//		}
////		//	}
////		//	if (topdotzones[i] == 2) {
////		//		topdots[i]--;
////		//		if (topdots[i] < cor23) {
////		//			topdots[i] = cor19 - 1;
////		//			topdotzones[i] = 1;
////		//		}
////		//	}
////
////		///////////// makes top down rotate counter clockwise
////			if (topdotzones[i] == 1) {
////				topdots[i]++;
////				if (topdots[i] >= cor19 && topdots[i] <= cor21) { // i believe the cor21 is useless
////					topdots[i] = cor23;
////					topdotzones[i] = 2;
////				}
////			}
////			if (topdotzones[i] == 2) {
////				topdots[i]++;
////				if (topdots[i] >= cor25) {
////					topdots[i] = cor15;
////					topdotzones[i] = 1;
////				}
////			}
////			leds[topdots[i]] = crgb::white;
////			topdottime = 0;
////		}
////	}
////}
////
////void travelingbottomdot(double &bottomdotspeed) {
////	if (bottomdottime > bottomdotspeed) {
////		for (int i = 0; i < 2; i++) {
////			bottomdots[i]++;
////			if (bottomdots[i] >= cor13) bottomdots[i] = cor7;
////			leds[bottomdots[i]] = crgb::white;
////			bottomdottime = 0;
////		}
////	}
////}
////
////void bouncingdotsmallsquares(int botsquarecolor, int topsquarecolor) {
////	if (topsmallsqvar < (cor24 - cor23 + 3)) {
////		leds[cor23 + topsmallsqvar - 1] = chsv(topsquarecolor, 255, 255);
////		leds[cor25 - topsmallsqvar - 1] = chsv(topsquarecolor, 255, 255);
////		leds[cor21 + topsmallsqvar] = chsv(topsquarecolor, 255, 255);
////		leds[cor23 - topsmallsqvar - 3] = chsv(topsquarecolor, 255, 255); // "-3" is because the bend around the wood wastes a few leds
////	}
////	if (topsmallsqvar < (cor19 - cor17 + 1)) {
////		leds[cor19 - topsmallsqvar - 1] = chsv(topsquarecolor, 255, 255);
////		leds[cor15 + topsmallsqvar] = chsv(topsquarecolor, 255, 255);
////	}
////	if (botsmallsqvar < (cor2 - cor1)) {
////		leds[cor9 - botsmallsqvar - 2] = chsv(botsquarecolor, 255, 255);
////		leds[cor7 + botsmallsqvar] = chsv(botsquarecolor, 255, 255);
////		leds[cor1 + botsmallsqvar] = chsv(botsquarecolor, 255, 255);
////		leds[cor3 - botsmallsqvar - 5] = chsv(botsquarecolor, 255, 255);
////	}
////	if (botsmallsqvar < (cor11 - cor9 + 1)) {
////		leds[cor9 + botsmallsqvar] = chsv(botsquarecolor, 255, 255);
////		leds[cor13 - botsmallsqvar - 4] = chsv(botsquarecolor, 255, 255);
////	}
////}