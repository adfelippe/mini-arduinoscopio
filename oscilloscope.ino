/*
  Basic oscilloscope
  using Arduino
  and a graphical LCD
*/

// include the library header
#include <glcd.h>
// include the Fonts
#include <fonts/allFonts.h>

#define  SAMPLE_RATE  100
#define  VCC          5
const int vIn = A0;
//Arduino has a 10-bit ADC
const float adc_precision = 0.00488;
//Factor to adjust the Y-axis points on the screen
const float screen_adjust_factor = 0.0943;
float rms_voltage;
float max_voltage;
double voltage[128];

//Prototypes
void drawChart(void);
void showVoltages(void);
void drawWave(void);
void clearChart(void);

//Arduino setup
void setup() 
{
	//Initialise GLCD
	GLCD.Init();
	// Select the font for the default text area
	GLCD.SelectFont(System5x7);
	//Clear the screen
	GLCD.ClearScreen();
}

//Arduino loop
void loop() 
{
	//Draw the X and Y chart lines
	drawChart();
	//Show voltages on the top right corner of the screen
	showVoltages();
	//Draw wave
	drawWave();
	//Clear the chat every half second
	//if ((millis()/500) == 0)
	delay(1000);
	clearChart();
}


void drawChart(void) 
{
	//Draw line on the Y-axis
	GLCD.DrawLine(0, 0, 0, GLCD.Bottom-9);
	//Draw line on the X-axis
	GLCD.DrawLine(0, GLCD.Bottom-9, GLCD.Width-1, GLCD.Bottom-9);
}

//Acquires and calculates voltages
//The voltage array will store a sample of a measurement
//to be displayed on the screen at the right position
void acquireVoltages(void) 
{
	max_voltage = 0;
	//Sample analog input
	for (int i = 0; i < 128; i++) {		
		for (int j = 0; j < SAMPLE_RATE; j++) {
			voltage[i] += analogRead(vIn);
		}
		voltage[i] = voltage[i]/SAMPLE_RATE;
		//Copy the max voltage into 
		if (voltage[i] > max_voltage)
		  max_voltage = voltage[i];
	}
	//Calculate average
	rms_voltage = 0;
	for (int i = 0; i < 128; i++) {
		rms_voltage += voltage[i];
	}
	rms_voltage = rms_voltage/128;
}

void showVoltages(void) 
{
	acquireVoltages();
	//Move cursor to the bottom line
	GLCD.CursorTo(0, 7);
	GLCD.print("Vrms:");
	GLCD.print(rms_voltage*adc_precision);
	GLCD.CursorTo(11, 7);
	GLCD.print("Vmax:");
	GLCD.print(max_voltage*adc_precision);
}

//Draw the wave on the screen	
void drawWave(void) 
{
	int y_axis_dot[128];
	int y_current_position[128];
	//Draw the dots on screen based on the 
	//measured voltages
	for (int i = 1; i < 128; i++) {
		//Get the exact Y-axis position to set the dot
		//Subtracted from 53 because it's inverted on the screen
		y_current_position[i] = 53 - (voltage[i]*adc_precision/screen_adjust_factor);
		//If the Y-axis dot has moved, clear the previous one
		if (y_current_position[i] != y_axis_dot[i])
		  GLCD.SetDot(i, y_axis_dot[i], WHITE);
		//Then set the new one
		y_axis_dot[i] = y_current_position[i];
		GLCD.SetDot(i, y_axis_dot[i], BLACK);
	}
}

//Clear the area inside the chart
void clearChart(void) 
{
	for (int y = 0; y <= 52; y++) {
		for (int x = 1; x <= 127; x++) {
			GLCD.SetDot(x, y, WHITE);
		}
	}
}
		
