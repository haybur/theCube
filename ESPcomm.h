
// Variables for solid color
uint8_t solidHue = 0;
uint8_t solidSat = 255;
uint8_t solidVal = 0;

const byte numChars = 10;
char receivedChars[10];
char tempChars[3];

// variables to hold parsed data
char category[1] = { 0 };
int instruction = 0;

bool newCommand = false;

void doThingsWithCommands() {
	if (receivedChars[0] == 'o')
		mode++;
	else if (receivedChars[0] == 's') {
		mode = 5;
		for (int i = 0; i < 3; i++)
			tempChars[i] = receivedChars[i + 1];
		solidHue = atoi(tempChars);
		Serial.println(solidHue);
	}
	else if (receivedChars[0] == 't') {
		for (int i = 0; i < 3; i++)
			tempChars[i] = receivedChars[i + 1];
		solidSat = atoi(tempChars);
	}
	else if (receivedChars[0] == 'b') {
		for (int i = 0; i < 3; i++)
			tempChars[i] = receivedChars[i + 1];
		solidVal = atoi(tempChars);
	}
	else {}
}

void recWithStartEndMarkers() {
	static boolean recInProgress = false;
	static byte ndx = 0;
	char startMarker = '<';
	char endMarker = '>';
	char rc;

	while (Serial1.available() && newCommand == false) {
		rc = Serial1.read();
		//Serial.println("data being read");
		if (recInProgress == true) {
			if (rc != endMarker) {
				receivedChars[ndx] = rc;
				ndx++;
				if (ndx >= numChars)
					ndx = numChars - 1;
			}
			else {
				receivedChars[ndx] = '\0';
				recInProgress = false;
				ndx = 0;
				newCommand = true;
			}
		}
		else if (rc == startMarker)
			recInProgress = true;
	}
}

void parseData() {
	if (newCommand == true) {
		for (int i = 0; i < sizeof(receivedChars); i++) {
			//    for (int i = 0; i < 5; i++) {
			Serial.print(receivedChars[i]);
			//Serial.print("  ");
		}
		Serial.println();
		//    Serial.print(receivedChars[0]);
		//    Serial.print("  ");
		//    Serial.print(receivedChars[1]);
		//    Serial.print("  ");  
		//    Serial.print(receivedChars[2]);
		//    Serial.print("  ");
		//    Serial.print(receivedChars[3]);
		//    Serial.print("  ");  
		//    Serial.print(receivedChars[4]);
		//    Serial.println("  ");
		   // Serial.println(receivedChars);

		doThingsWithCommands();

		newCommand = false;
	}
}

void talkToESP() {
	recWithStartEndMarkers();
	parseData();
}