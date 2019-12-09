/*
    Name:       theCUBE.ino
    Created:	8/25/2019 3:03:40 PM
    Author:     DESKTOP-UB92N3P\dehab
*/

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#include <Arduino.h>
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>
#include <Statistic.h>

#define knobPin1 A1
#define knobPin2 A8
#define buttonPin1 3
#define buttonPin2 4
#define buttonPin3 12
int knobReading1, knobReading2;
bool buttonReading1, buttonReading2, buttonReading3;

#define COLOR_ORDER GRB                                       // It's GRB for WS2812 and BGR for APA102.
#define NUM_LEDS 979 // Because for now, I really do only have 979
#define NUM_LEDS_PER_STRIP 979
#define NUM_STRIPS 8
CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

#define NUM_LEDS_EL 7128
#define NUM_LEDS_PER_STRIP_EL 900
#define ACT_NUM_LEDS_PER_STRIP_EL 891
CRGB ledsE[NUM_STRIPS * NUM_LEDS_PER_STRIP_EL];
const int rowsE = 9;
const int columnsE = 22;
const int heightE = 36;
int elektraCoords[9][22][36];

#include "DDAudio.h"

// Cube Dimensions
const int rows = 5;
const int columns = 5;
const int height = 18;
int coords[rows][columns][height];

uint8_t x;
uint8_t y;
uint8_t z;
CRGB color;
uint8_t hue;
float hue2;

int xx;
int mode = 4;

#include "DrawingBB.h"
#include "MusicAnimationsBB.h"
#include "ESPcomm.h"

#define buttonPin 33

void setup()
{
	Serial.begin(9600);
	Serial1.begin(115200);
	delay(500);
	Serial.println("Starting setup");
	mapCube();
	mapElektra();
	audioSetup();
	buttonSetup();

	//LEDS.addLeds<WS2813, 34, GRB>(leds, NUM_LEDS);
	//LEDS.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
	LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS);
    //FastLED.addLeds<NUM_STRIPS, WS2813, 34, GRB>(leds, NUM_LEDS_PER_STRIP);
  
	LEDS.clear(true);
	delay(2000);
	LEDS.show();

	LEDS.setBrightness(60);
	testElektraMapping();
	//Serial.println("fuck");
	Serial.println("setup complete!");

	pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
	//leds[0] = CHSV(0, 0, 0);

	talkToESP();

	fillStats();
	musicAnalytics(false);

	//if (digitalRead(buttonPin) == 0) {
	//	mode++;
	//	delay(2000);
	//}
	if (mode == 6) mode = 0;
	
	if (silence) ambientCube();
	else {
		switch (mode) {
		case 0:
			sparklesPattern();
			break;
		case 1:
			volumeBarsIn3D();
			break;
		case 2:
			volumeBall();
			break;
		case 3:
			twoBalls();
			break;
		case 4:
			movingLevels();
			break;
		case 5:
			solidColor();
			break;
		}
	}

	//hue++;
	//fill_rainbow(leds, 979, hue, 10);


	// Transfer output 1 to all outputs
	for (int i = 0; i < NUM_STRIPS; i++) {
		for (int j = 0; j < NUM_LEDS; j++)
			leds[(979 * i) + j] = leds[j];
	}

	//volumeBallElektra();
	//drawBallElektra(5, 5, 10, 20, CHSV(0, 255, 255));
	//drawShell(5, 5, 10, 20, 2, CHSV(0, 255, 255));
	//drawShellElektra(5, 10, 20, 20, 2, CHSV(0, 255, 255));
	//Serial.println(millis());

	//inputTesting();

	//sparklesPattern();
	//volumeBarsIn3D();
	//allBalls();
	//volumeBall(); 
	//twoBalls();
	//movingLevels();

	//printSpectrum();
	//Serial.println(LEDS.getFPS());                              // Display frames per second on the serial monitor.
	FastLED.show();
}

void solidColor() {
	for (int i = 0; i < NUM_LEDS_PER_STRIP * NUM_STRIPS; i++)
		leds[i] = CHSV(solidHue, solidSat, solidVal);
}

void setVoxel(uint8_t x, uint8_t y, uint8_t z, CRGB color) {
	if (x >= 0 && y >= 0 && z >= 0 && x < rows && y < columns && z < height) {
		int ledValue = coords[x][y][z];
		leds[ledValue] = color;
		leds[ledValue + 2 * z + 1] = color; // Sets the voxel on the back
	}
}

void setVoxelAdd(uint8_t x, uint8_t y, uint8_t z, CRGB color) {
	if (x >= 0 && y >= 0 && z >= 0 && x < rows && y < columns && z < height) {
		int ledValue = coords[x][y][z];
		leds[ledValue] += color;
		leds[ledValue + 2 * z + 1] += color; // Sets the voxel on the back
	}
}

int returnLED(int x, int y, int z) {
	int ledValue;
	if (x >= 0 && y >= 0 && z >= 0 && x < rows && y < columns && z < height)
		ledValue = coords[x][y][z];
	return ledValue;
}

void drawShell(int x, int y, int z, float r, int thickness, CRGB color) {
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			for (int k = 0; k < 17; k++)
				if (abs(sqrt(5 * pow(i - x, 2) + 5 * pow(j - y, 2) + pow(k - z, 2)) - r) < thickness)
					setVoxel(i, j, k, color);
}

void drawBall(int x, int y, int z, int r, CRGB color) {
	static int sdx, edx; // sdx = startdx, edx = enddx
	static int sdy, edy;
	static int sdz, edz;

	// Check to make sure radius does not go outside the number of ROWS
	if (5*x - r < 0) sdx = 0;
	else sdx = -r;
	if (5*x + r > rows) edx = rows;
	else edx = r;
	// Check to make sure radius does not go outside the number of COLUMNS
	if (5*y - r < 0) sdy = 0;
	else sdy = -r;
	if (5*y + r > columns) edy = columns;
	else edy = r;
	// Check to make sure radius does not go outside the HEIGHT
	if (z - r < 0) sdz = 0;
	else sdz = -r;
	if (z + r > height) edz = height;
	else edz = r;

	for (int dx = sdx; dx <= edx; dx++)
		for (int dy = sdy; dy <= edy; dy++)
			for (int dz = sdz; dz <= edz; dz++)
				if (sqrt(5 * dx * dx + 5 * dy * dy + dz * dz) <= r)
					setVoxelAdd(x + dx, y + dy, z + dz, color);
					//setVoxel(x + dx, y + dy, z + dz, color);

	// OLD WAY, SLOW AT LARGE RADII
	//for (int dx = -r; dx <= r; dx++)
	//	for (int dy = -r; dy <= r; dy++)
	//		for (int dz = -r; dz <= r; dz++)
	//			if (sqrt(5 * dx * dx + 5 * dy * dy + dz * dz) <= r)
	//				setVoxel(x + dx, y + dy, z + dz, color);
}

void mapCube() {
	static int index = 1; // "index" is the # of LED on the strip. Initiating at 1 skips the unused first LED
	for (int i = 0; i < rows; i++) {
		if (i > 0) index = index + 5; // Skips 4 LEDS between each row

		for (int j = 0; j < columns; j++) {
			if (j > 0) // Skips the first column
				index = index + 4; // Skips 3 LEDs between each column

			for (int k = height - 1; k >= 0; k--) {
				if (i == 1 || i == 3)  // The rows snake, so this accounts for 1 and 3 going the opposite direction
					coords[i][4 - j][k] = index;
				else
					coords[i][j][k] = index;

				if (k > 0) index++; // 
				if (k == 0) index = index + 18; // This accounts for all the 18 LEDs going back up with the same coordinates
			}
		}
	}
}

void drawShellElektra(int x, int y, int z, float r, int thickness, CRGB color) {
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 22; j++)
			for (int k = 0; k < 36; k++)
				if (abs(sqrt(14 * pow(i - x, 2) + 14 * pow(j - y, 2) + pow(k - z, 2)) - r) < thickness)
					setVoxel(i, j, k, color);
}

void setVoxelElektra(uint8_t x, uint8_t y, uint8_t z, CRGB color) {
	if (x >= 0 && y >= 0 && z >= 0 && x < rowsE && y < columnsE && z < heightE) {
		int ledValue = elektraCoords[x][y][z];
		leds[ledValue] = color;
		//leds[ledValue + 2 * z + 1] = color; // Sets the voxel on the back
	}
}

void drawBallElektra(int x, int y, int z, int r, CRGB color) {
	static int sdx, edx; // sdx = startdx, edx = enddx
	static int sdy, edy;
	static int sdz, edz;

	// Check to make sure radius does not go outside the number of ROWS
	if (14*x - r < 0) sdx = 0;
	else sdx = -r;
	if (14 * x + r > rowsE) edx = rowsE;
	else edx = r;
	// Check to make sure radius does not go outside the number of COLUMNS
	if (14 * y - r < 0) sdy = 0;
	else sdy = -r;
	if (14 * y + r > columnsE) edy = columnsE;
	else edy = r;
	// Check to make sure radius does not go outside the HEIGHT
	if (z - r < 0) sdz = 0;
	else sdz = -r;
	if (z + r > heightE) edz = heightE;
	else edz = r;

	for (int dx = sdx; dx <= edx; dx++)
		for (int dy = sdy; dy <= edy; dy++)
			for (int dz = sdz; dz <= edz; dz++)
				if (sqrt(5 * dx * dx + 5 * dy * dy + dz * dz) <= r)
					setVoxel(x + dx, y + dy, z + dz, color);

	// OLD WAY, TAKES TOO LONG FOR LARGE RADII
	//for (int dx = -r; dx <= r; dx++)
	//	for (int dy = -r; dy <= r; dy++)
	//		for (int dz = -r; dz <= r; dz++)
	//			if (sqrt(5 * dx * dx + 5 * dy * dy + dz * dz) <= r)
	//				setVoxel(x + dx, y + dy, z + dz, color);
}

void volumeBallElektra() {
		static int ballRadius;
		static int newRadius;

		//newRadius = constrain(volume, 20, 3000); // This isn't the new radius at all, just using it here cause it's redefined in next line
		//newRadius = map(volume, 20, 3000, 1, 13);

		newRadius = constrain(spectrumValue[2], 50, 500);
		newRadius = map(newRadius, 50, 500, 1, 20);
		if (newRadius > ballRadius)
			ballRadius = newRadius;

		EVERY_N_MILLIS(10) ballRadius--;

		constrain(ballRadius, 0, 25);

		drawBallElektra(2, 2, 10, ballRadius, CHSV(0, 255, 255));
		fadeAll(160);
		Serial.println(ballRadius);
}

int pixelIndex;
int rowIndex;
int colIndex;
void mapElektra() {
	pixelIndex = 0;
	static int output = 0;

	//////// LED OUTPUT 11111111111111111 ///////
	Serial.println(pixelIndex);
	rowIndex = 1;
	colIndex = 2;
	for (int i = 0; i < 3; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex--;
	}
	Serial.println(pixelIndex);
	rowIndex = 0;
	colIndex = 0;
	for (int i = 0; i < 22; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex++;
	}
	//////// LED OUTPUT 22222222222222222222 ///////
	Serial.println(pixelIndex);
	rowIndex = 1;
	colIndex = 3;
	for (int i = 0; i < 19; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex++;
	}
	Serial.println(pixelIndex);
	rowIndex = 2;
	colIndex = 21;
	for (int i = 0; i < 6; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex++;
	}
	//////// LED OUTPUT 3333333333333333333 ///////
	Serial.println(pixelIndex);
	rowIndex = 3;
	colIndex = 8;
	for (int i = 0; i < 8; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex--;
	}
	rowIndex = 2;
	colIndex = 0;
	for (int i = 0; i < 17; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex++;
	}
	//////// LED OUTPUT 4444444444444444444 ///////
	Serial.println(pixelIndex);
	rowIndex = 3;
	colIndex = 9;
	for (int i = 0; i < 13; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex++;
	}
	rowIndex = 2;
	colIndex = 21;
	for (int i = 0; i < 12; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex--;
	}
	//////// LED OUTPUT 55555555555555555 ///////
	Serial.println(pixelIndex);
	rowIndex = 5;
	colIndex = 14;
	for (int i = 0; i < 15; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex--;
	}
	rowIndex = 4;
	colIndex = 0;
	for (int i = 0; i < 10; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex++;
	}
	//////// LED OUTPUT 66666666666666 ///////
	Serial.println(pixelIndex);
	rowIndex = 5;
	colIndex = 15;
	for (int i = 0; i < 7; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex++;
	}
	rowIndex = 6;
	colIndex = 21;
	for (int i = 0; i < 18; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex--;
	}
	//////// LED OUTPUT 77777777777777777 ///////
	Serial.println(pixelIndex);
	rowIndex = 7;
	colIndex = 20;
	for (int i = 0; i < 21; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex--;
	}
	rowIndex = 6;
	colIndex = 0;
	for (int i = 0; i < 4; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex++;
	}
	//////// LED OUTPUT 8888888888888 ///////
	Serial.println(pixelIndex);
	rowIndex = 7;
	colIndex = 21;
	for (int i = 0; i < 1; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex++;
	}
	rowIndex = 8;
	colIndex = 21;
	for (int i = 0; i < 22; i++) {
		downUpMap(rowIndex, colIndex);
		colIndex--;
	}
}

void downUpMap(int row, int column) {
	for (int i = 0; i < heightE; i++) {  // Change 36 to ElektraHeight
		if (i < 18)
			elektraCoords[row][column][abs(34 - (i * 2))] = pixelIndex; // If the highest pixel is on the back side, change 17 to 18
		else
			elektraCoords[row][column][abs(34 - ((i - 1) * 2) - 1)] = pixelIndex;
		pixelIndex++;
	}
}

void testElektraMapping() {

	//for (int i = 15; i < 20; i++) {
	//	Serial.println(abs(35 - (i * 2)));
	//}

	//for (int i = 0; i < 36; i++)
	//	Serial.println(elektraCoords[7][20][i]);
	//for (int i = 0; i < 36; i++)
	//	Serial.println(elektraCoords[7][21][i]);
	//for (int i = 0; i < 36; i++)
	//	Serial.println(elektraCoords[8][0][i]);

	//Serial.println(elektraCoords[1][2][0]);
	//Serial.println(elektraCoords[1][2][1]);
	//Serial.println(elektraCoords[1][2][2]);
	//Serial.println(elektraCoords[1][2][3]);

	int ii = 1;
	int jj = 2;
	//for (int i = jj; i > 0; i--) {
	//	for (int k = 36; k > 0; k--)
	//		Serial.println(elektraCoords[ii][jj][k]);
	//}

	//for (int i = 0; i < ; i++) {
	//	for (int j = 0; j < 22; j++) {
	//		Serial.println(elektraCoords[i][j][18]);
	//	}
	//}
}

void ambientCube() {
	//fill_rainbow(leds, 979, 0, 1);
	
}

void buttonSetup() {
	pinMode(buttonPin1, INPUT_PULLUP);
	pinMode(buttonPin2, INPUT_PULLUP);
	pinMode(buttonPin3, INPUT_PULLUP);
}

void inputTesting() {
	int temp;

	temp = analogRead(knobPin1);
	Serial.print("knob 1: ");
	Serial.print(temp);
	Serial.print("\t");
	temp = analogRead(knobPin2);
	Serial.print("knob 2: ");
	Serial.print(temp);
	Serial.print("\t");
	temp = digitalRead(buttonPin1);
	Serial.print("button 1: ");
	Serial.print(temp);
	Serial.print("\t");
	temp = digitalRead(buttonPin2);
	Serial.print("button 2: ");
	Serial.print(temp);
	Serial.print("\t");
	temp = digitalRead(buttonPin3);
	Serial.print("button 3: ");
	Serial.print(temp);
	Serial.print("\t");

	Serial.println();
}