
/*
   Arduino Uno Mini LED Cube
   by Hari Wiguna, 2022
*/

/*
   Wiring:
   LEDs are wired with common Anode (+) per layer.
   4x4 Cathode (-) vertical pillars control each LED for all the layers.

   All the LEDs are refreshed by MsTimer2 every 3ms:
   1. For each layer, we ground (-) the cathodes of the LEDs we want to light up on that layer.
   2. We then, provide (+) for that layer.
   3. After n miliseconds, we turn off that layer, setup (-) for next layer, and then provide (+) for that layer.
   4. Repeat forever.
*/

#include <MsTimer2.h>
#include "Font.h"

volatile bool cube[4][4][4] ; // This array is transferred to the actual LED cube by MsTimer2 every 3 ms

volatile byte layerPins[] = { 9, 8, 7, 4 }; // Arduino pins controlling layer/level/tier (Z axis)

// USB connector is FRONT side of cube
//volatile byte pillarPins[][4] = { // Arduino pins controlling the 4x4 horizontal layer (X-Y axes)
//    { 11, 10, 12, 13},
//  { 6, 5, A1, A0},
//  { 3,2,A3, A2},
//  { 1,0,A5, A4}
//};

// USB connector is LEFT side of cube
volatile byte pillarPins[][4] = { // Arduino pins controlling the 4x4 horizontal layer (X-Y axes)
  { 13,A0,A2,A4},
  { 12,A1,A3,A5},
  { 10,5,2,0},
  { 11,6,3,1},
};


int dly = 100; // 0=No Delay, Higher=slower.
bool trail = false; // true:once lit, that led stays lit. false:turn off the previous led before lighting up the next one.

const bool hasTrail = true;
const bool noTrail = false;

const bool on = true;
const bool off = false;

const byte alongZ = 10;
const byte alongY = 11;
const byte alongX = 12;

#define DEBUG false

void setup()
{
    if (DEBUG) Serial.begin(9600);
    // Setup layer pins as INPUTs! We will set each to OUTPUT one at a time in the refresh routine.
    for (byte i = 0; i < sizeof(layerPins); i++)
        pinMode(layerPins[i], OUTPUT);

    // Setup pillar pins as output
    for (byte y = 0; y < 4; y++)
    {
        for (byte x = 0; x < 4; x++)
        {
            byte pillarPin = pillarPins[y][x];
            pinMode(pillarPin, OUTPUT);
        }
    }


    MsTimer2::set(3, Refresh); // interval in ms
    MsTimer2::start();

    All(on);
    delay(5000);
}

void loop()
{
    All(off);
    RaiseAndLowerCube(100, 1000); // (Interval, Pause)
    //Scan(on, noTrail, 100); // Slowly Scan every pixel
    Scan(on, hasTrail, 100); // Go through all pixels again, but this time leave it on.
    Blinks(500, 3);
    Sweeps();
    JumpingWater();
    Spinners();
    Circles(alongY, 150); // (alongWhichAxis, interval)
    CorkScrews(60, 500); // (interval, pause)
    ScrollMessage("BE KIND", 250);
    delay(1000);
}

void Refresh()
{
    static byte layer = 0;

    // Turn off previous layer
    //pinMode(layerPins[layer], INPUT); // common anode for this layer is now floating instead of HI/LO
    digitalWrite(layerPins[layer], HIGH);

    // Setup next layer
    layer++;
    if (layer > 3) layer = 0;

    // Setup layer pattern for this layer
    for (byte x = 0; x < 4; x++)
    {
        for (byte y = 0; y < 4; y++)
        {
            byte pillarPin = pillarPins[y][x];
            digitalWrite(pillarPin, cube[x][y][layer] ? 1 : 0);
        }
    }
    // Turn on this layer
    //pinMode(layerPins[layer], OUTPUT);
    digitalWrite(layerPins[layer], LOW); //TODO: flip this when we add layer transistor
}


//=== Primitives ===

//-- Dots --
void DrawDot(byte x, byte y, byte z, bool isOn)
{
    cube[x][y][z] = isOn;
}

//-- Lines --
void DrawLineUp(byte x, byte y)
{
    for (byte z = 0; z < 4; z++)
    {
        cube[x][y][z] = true;
        delay(dly);
        if (!trail) cube[x][y][z] = false;
    }
}

void DrawLineDown(byte x, byte y)
{
    for (byte zz = 0; zz < 4; zz++)
    {
        byte z = 3 - zz;
        cube[x][y][z] = true;
        delay(dly);
        if (!trail) cube[x][y][z] = false;
    }
}

void DrawLineThruXZ(byte x, byte z, bool isOn)
{
    for (byte y = 0; y < 4; y++)
        cube[x][y][z] = isOn;
}

void DrawLineThruXY(byte x, byte y, bool isOn)
{
    for (byte z = 0; z < 4; z++)
        cube[x][y][z] = isOn;
}

void DrawLineThruYZ(byte y, byte z, bool isOn)
{
    for (byte x = 0; x < 4; x++)
        cube[x][y][z] = isOn;
}

//-- Plane at X --
void DrawPlaneAtX(byte x, bool isOn)
{
    for (byte z = 0; z < 4; z++)
        for (byte y = 0; y < 4; y++)
            cube[x][y][z] = isOn;
}

void DrawPlaneToRight(byte startFrom)
{
    for (byte x = startFrom; x < 4; x++)
    {
        DrawPlaneAtX(x, true);
        delay(dly);
        if (!trail) DrawPlaneAtX(x, false);
    }
}

void DrawPlaneToLeft(byte startFrom)
{
    for (int x = startFrom; x >= 0; x--)
    {
        DrawPlaneAtX(x, true);
        delay(dly);
        if (!trail) DrawPlaneAtX(x, false);
    }
}

//-- Plane at Y --
void DrawPlaneAtY(byte y, bool isOn)
{
    for (byte z = 0; z < 4; z++)
        for (byte x = 0; x < 4; x++)
            cube[x][y][z] = isOn;
}

void DrawPlaneToBack(byte startFrom)
{
    for (byte y = startFrom; y < 4; y++)
    {
        DrawPlaneAtY(y, true);
        delay(dly);
        if (!trail) DrawPlaneAtY(y, false);
    }
}

void DrawPlaneToFront(byte startFrom)
{
    for (int y = startFrom; y >= 0; y--)
    {
        DrawPlaneAtY(y, true);
        delay(dly);
        if (!trail) DrawPlaneAtY(y, false);
    }
}

//-- Plane at Z --
void DrawPlaneAtZ(byte z, bool isOn)
{
    for (byte y = 0; y < 4; y++)
        for (byte x = 0; x < 4; x++)
            cube[x][y][z] = isOn;
}

void DrawPlaneToTop(byte startFrom)
{
    for (byte z = startFrom; z < 4; z++)
    {
        DrawPlaneAtZ(z, true);
        delay(dly);
        if (!trail) DrawPlaneAtZ(z, false);
    }
}

void DrawPlanetoBottom(byte startFrom)
{
    for (int z = startFrom; z >= 0; z--)
    {
        DrawPlaneAtZ(z, true);
        delay(dly);
        if (!trail) DrawPlaneAtZ(z, false);
    }
}


//== Effects ==
void Scan(bool isOn, bool leaveTrail, int interval)
{
    for (byte z = 0; z < 4; z++)
    {
        for (byte y = 0; y < 4; y++)
        {
            for (byte x = 0; x < 4; x++)
            {
                cube[x][y][z] = isOn;
                if (interval != 0)
                {
                    delay(interval);
                    if (!leaveTrail) cube[x][y][z] = false;
                }
            }
        }
    }
}

void All(bool isOn)
{
    Scan(isOn, noTrail, 0);
}

void Sweeps()
{
    dly = 150;
    trail = false;

    //-- Front-Back-Front --
    DrawPlaneToBack(0);
    DrawPlaneToFront(2);
    DrawPlaneToBack(0);
    DrawPlaneToFront(2);
    delay(dly);

    //-- Left-Right-Left --
    DrawPlaneToRight(0);
    DrawPlaneToLeft(2); // test to here
    DrawPlaneToRight(0);
    DrawPlaneToLeft(2);
    delay(dly);

    //-- Up-Down-Up --
    DrawPlaneToTop(0);
    DrawPlanetoBottom(2);
    DrawPlaneToTop(0);
    DrawPlanetoBottom(2);
    delay(dly);
}


void JumpingWater()
{
    dly = 0; trail = true;
    int interval = 100;
    byte hop[][2] = {
        { 0,0},{ 0,1},{ 0,2},
        { 1,3},
        { 2,3},
        { 3,2},{ 3,1},{ 3,0},
    };

    // PatternUp(byte* pattern, int patternLen, int tailLen, bool wrapAround, byte plane, byte atDepth, int interval)
    byte depth = 255;
    PatternUp(*hop, sizeof(hop) / 2, 3, false, alongY, depth, interval);
    PatternDown(*hop, sizeof(hop) / 2, 3, false, alongY, depth, interval);

    PatternUp(*hop, sizeof(hop) / 2, 3, false, alongX, depth, interval);
    PatternDown(*hop, sizeof(hop) / 2, 3, false, alongX, depth, interval);
}

void Spinner(byte alongWhichAxis, int interval)
{
    byte spin[][2] = {
        { 0,1},{ 1,1}, { 2,2},{ 3,2}, // h1
        { 0,2},{ 1,2},{ 2,1},{ 3,1}, // h2
        { 0,3},{ 1,2},{ 2,1},{ 3,0}, //diagonal 1
        { 1,2},{ 1,2},{ 2,1},{ 2,0}, //v1
        { 2,3},{ 2,2},{ 1,1},{ 1,0}, //v2
        { 3,3},{ 2,2},{ 1,1},{ 0,0}, // diagonal 2
    };

    byte patternLen = sizeof(spin);
    byte numFrames = 6; // h1, h2, diag1,  v1, v2, diag2
    byte frameLen = patternLen / 2 / numFrames;
    for (int n = 0; n < 2; n++) // How many times to play this animation?
        for (int f = 0; f < numFrames; f++)
            AnimateRect(*spin, frameLen, alongWhichAxis, f, interval);
}

void Spinners()
{
    Spinner(alongZ, 150); delay(150);
    Spinner(alongY, 150); delay(150);
    Spinner(alongX, 150); delay(150);
}


void CorkScrew(byte alongWhichAxis, int interval)
{
    byte corkScrew[][3] = {
        { 1,3,0},{ 2,3,0}, // Circle is on XY plane, Z upward
        { 3,2,0},{ 3,1,0},
        { 2,0,0},{ 1,0,0},
        { 0,1,0},{ 0,2,0},

        { 1,3,1},{ 2,3,1},
        { 3,2,1},{ 3,1,1},
        { 2,0,1},{ 1,0,1},
        { 0,1,1},{ 0,2,1},
        
        { 1,3,2},{ 2,3,2},
        { 3,2,2},{ 3,1,2},
        { 2,0,2},{ 1,0,2},
        { 0,1,2},{ 0,2,2},
        
        { 1,3,3},{ 2,3,3},
        { 3,2,3},{ 3,1,3},
        { 2,0,3},{ 1,0,3},
        { 0,1,3},{ 0,2,3},
    };

    Pattern3DUp(*corkScrew, sizeof(corkScrew) / 3, 6, false, alongWhichAxis, interval);
    Pattern3DDown(*corkScrew, sizeof(corkScrew) / 3, 6, false, alongWhichAxis, interval);
}

void CorkScrews(int interval, int pause)
{
    CorkScrew(alongZ, interval); delay(pause); // Circle is on XY plane,Z upward
    CorkScrew(alongY, interval); delay(pause);
    CorkScrew(alongX, interval); delay(pause);
}


void Circles(byte alongWhichAxis, int interval)
{
    byte circle[][2] = {
        { 1,3},{ 2,3}, // Circle is on XY plane, Z upward
        { 3,2},{ 3,1},
        { 2,0},{ 1,0},
        { 0,1},{ 0,2}
    };

    for (int n = 0; n < 3; n++)
        PatternUp(*circle, sizeof(circle) / 2, 6, true, alongWhichAxis, 255, interval);
    All(off);
}

void DrawLineThruPlane(byte c, byte r, byte alongWhichAxis, boolean isOn)
{
    //Serial.println(  String(c) + "," + String(r) + ": " + (isOn ? "ON" : "off"));
    switch (alongWhichAxis)
    {
        case alongZ: DrawLineThruXY(c, r, isOn); break; // line is along Z axis
        case alongY: DrawLineThruXZ(c, r, isOn); break; // Line is along Y axis
        case alongX: DrawLineThruYZ(c, r, isOn); break; // Line is along X axis
    }
}

void DrawDotOnPlane(byte x, byte y, byte p, byte alongWhichAxis, boolean isOn)
{
    //Serial.println(  String(c) + "," + String(r) + ": " + (isOn ? "ON" : "off"));
    switch (alongWhichAxis)
    {
        case alongZ: DrawDot(x, y, p, isOn); break; // point p is along Z axis
        case alongY: DrawDot(x, p, y, isOn); break; // point p is along Y axis
        case alongX: DrawDot(p, x, y, isOn); break; // Point p is along X axis
    }

}


void PatternUp(byte* pattern, int patternLen, int tailLen, bool wrapAround, byte plane, byte atDepth, int interval)
{
    if (DEBUG) Serial.println();
    for (int i = 0; i < (patternLen + (wrapAround ? 0 : tailLen)); i++)
    {
        if (DEBUG) Serial.print("i=" + String(i) + " ");
        int k = i - tailLen;
        if (k < 0)
            k = patternLen + k;
        if (DEBUG) Serial.print(String(k) + "=off ");
        byte x = pattern[k * 2 + 0];
        byte y = pattern[k * 2 + 1];
        if (atDepth == 255)
            DrawLineThruPlane(x, y, plane, false);
        else
            DrawDotOnPlane(x, y, plane, atDepth, false);

        if (i < patternLen)
        {
            int j = (i >= patternLen) ? i - patternLen : i;
            if (DEBUG) Serial.print(String(j) + "=ON  ");
            byte x = pattern[j * 2 + 0];
            byte y = pattern[j * 2 + 1];
            if (atDepth == 255)
                DrawLineThruPlane(x, y, plane, true);
            else
                DrawDotOnPlane(x, y, plane, atDepth, true);

        }

        if (DEBUG) Serial.println();
        delay(interval);
    }
}

void PatternDown(byte* pattern, byte patternLen, int tailLen, bool wrapAround, byte plane, byte atDepth, int interval)
{
    for (int i = patternLen - 1; i >= -tailLen; i--)
    {
        if (i >= 0)
        {
            byte x = pattern[i * 2 + 0];
            byte y = pattern[i * 2 + 1];
            if (atDepth == 255)
                DrawLineThruPlane(x, y, plane, true);
            else
                DrawDotOnPlane(x, y, plane, atDepth, true);
        }

        byte k = i + tailLen - 1;
        if (k <= patternLen)
        {
            byte x = pattern[k * 2 + 0];
            byte y = pattern[k * 2 + 1];
            if (atDepth == 255)
                DrawLineThruPlane(x, y, plane, false);
            else
                DrawDotOnPlane(x, y, plane, atDepth, false);
        }
        delay(interval);
    }
}


void Pattern3DUp(byte* pattern, int patternLen, int tailLen, bool wrapAround, byte alongWhichAxis, int interval)
{
    if (DEBUG) Serial.println();
    for (int i = 0; i < (patternLen + (wrapAround ? 0 : tailLen)); i++)
    {
        if (DEBUG) Serial.print("i=" + String(i) + " ");
        int k = i - tailLen;
        if (k < 0)
            k = patternLen + k;
        if (DEBUG) Serial.print(String(k) + "=off ");
        byte x = pattern[k * 3 + 0];
        byte y = pattern[k * 3 + 1];
        byte z = pattern[k * 3 + 2];
        DrawDotOnPlane(x, y, z, alongWhichAxis, false);

        if (i < patternLen)
        {
            int j = (i >= patternLen) ? i - patternLen : i;
            if (DEBUG) Serial.print(String(j) + "=ON  ");
            byte x = pattern[j * 3 + 0];
            byte y = pattern[j * 3 + 1];
            byte z = pattern[j * 3 + 2];
            DrawDotOnPlane(x, y, z, alongWhichAxis, true);
        }

        if (DEBUG) Serial.println();
        delay(interval);
    }
}


void Pattern3DDown(byte* pattern, int patternLen, int tailLen, bool wrapAround, byte alongWhichAxis, int interval)
{
    if (DEBUG) Serial.println();
    for (int i = patternLen - 1; i >= (wrapAround ? 0 : -tailLen); i--)
    {
        if (DEBUG) Serial.print("i=" + String(i) + " ");
        int k = i + tailLen;
        if (k >= patternLen)
            k = i - patternLen - 1;
        if (DEBUG) Serial.print(String(k) + "=off ");
        byte x = pattern[k * 3 + 0];
        byte y = pattern[k * 3 + 1];
        byte z = pattern[k * 3 + 2];
        DrawDotOnPlane(x, y, z, alongWhichAxis, false);

        if (i >= 0)
        {
            int j = (i < 0) ? patternLen - i : i;
            if (DEBUG) Serial.print(String(j) + "=ON  ");
            byte x = pattern[j * 3 + 0];
            byte y = pattern[j * 3 + 1];
            byte z = pattern[j * 3 + 2];
            DrawDotOnPlane(x, y, z, alongWhichAxis, true);
        }

        if (DEBUG) Serial.println();
        delay(interval);
    }
}


void Animate(byte* pattern, byte frameLen, byte plane, byte frame, int interval)
{
    //-- Current frame twice: once to draw, second to erase--
    for (int d = 0; d < 2; d++)
    {
        for (int i = 0; i < frameLen; i++)
        {
            byte x = pattern[(frame * frameLen + i) * 2 + 0];
            byte y = pattern[(frame * frameLen + i) * 2 + 1];
            DrawDot(x, y, 3, d == 0);
        }

        if (d == 0) delay(interval);
    }
}

void AnimateRect(byte* pattern, byte frameLen, byte alongWhichAxis, byte frame, int interval)
{
    //-- Current frame twice: once to draw, second to erase--
    for (int d = 0; d < 2; d++)
    {
        for (int i = 0; i < frameLen; i++)
        {
            byte x = pattern[(frame * frameLen + i) * 2 + 0];
            byte y = pattern[(frame * frameLen + i) * 2 + 1];
            bool isOn = d == 0;
            //DrawDot(x, y, 0, d == 0);
            DrawDotOnPlane(x, y, 0, alongWhichAxis, isOn);
            if (i == 0 || i == (frameLen - 1))
            {
                //DrawDot(x, y, 1, d == 0);
                //DrawDot(x, y, 2, d == 0);
                DrawDotOnPlane(x, y, 1, alongWhichAxis, isOn);
                DrawDotOnPlane(x, y, 2, alongWhichAxis, isOn);
            }
            //DrawDot(x, y, 3, d == 0);
            DrawDotOnPlane(x, y, 3, alongWhichAxis, isOn);
        }

        if (d == 0) delay(interval);
    }
}


void Blink(int blinkRate)
{
    All(on); delay(blinkRate);
    All(off); delay(blinkRate);
}

void Blinks(int blinkRate, byte howManyTimes)
{
    for (int i = 0; i < howManyTimes; i++)
        Blink(blinkRate);
}


void ShiftLeft()
{
    // Shift Top layer
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 4; y++)
            cube[x][y][3] = cube[x + 1][y][3];
    }

    // Shift Front Layer
    for (int x = 0; x < 3; x++)
    {
        for (int z = 0; z < 3; z++)
            cube[x][0][z] = cube[x + 1][0][z];
    }
}

void ScrollMessage(String message, int interval)
{
    message.toUpperCase();

    for (int charIndex = 0; charIndex < message.length(); charIndex++)
    {
        char ch = message[charIndex];
        int alphaIndex = ch - ' ';
        if (alphaIndex < 0) alphaIndex = 0;

        for (int column = 0; column < 7; column++)// Each character is only 5 columns wide, but I loop two more times to create 2 pixel space betwen characters
        {
            // Shift one pixel left
            ShiftLeft();

            // Draw rightmost column
            byte alphaBits = (column < 5) ? alphabets[alphaIndex][column] : 0; // sixth and seventh columns are always blank
            // Topmost row in the alphabet bitmap is always off, and we only have 7 rows (4+3), so ignore highest bit of the font.
            for (int row = 1; row < 8; row++) // font row 1,2,3,4 on top layer, row 5,6,7 on front layer
            {
                bool isOn = bitRead(alphaBits, 7 - row);
                if (row < 5)
                    cube[3][4 - row][3] = isOn;
                else
                    cube[3][0][7 - row] = isOn;
            }
            delay(interval);
        }
    }
}



// Pretty draw animated rect
void SlowDrawRect(byte p, byte alongWhichAxis, bool isOn, int interval)
{
    byte halfRect[][2] = {
        { 1,3},{ 0,3},{ 0,2},{ 0,1},{ 0,0},{ 1,0}
    };

    for (int i = 0; i < sizeof(halfRect) / 2; i++)
    {
        byte x = halfRect[i][0];
        byte y = halfRect[i][1];
        DrawDotOnPlane(x, y, p, alongWhichAxis, isOn);
        DrawDotOnPlane(3 - x, y, p, alongWhichAxis, isOn);
        delay(interval);
    }
}

void RaiseCube(byte alongWhichAxis, int interval)
{
    // Animate rect at bottom of cube
    SlowDrawRect(0, alongWhichAxis, true, interval);
    delay(interval);

    // Animate that rect raising and leaving trail of edges
    for (int z = 1; z < 4; z++)
    {
        // draw the rect on this layer
        SlowDrawRect(z, alongWhichAxis, true, 0);

        // Draw the pillar trail
        int q = z - 1;
        DrawDotOnPlane(0, 0, q, alongWhichAxis, true);
        DrawDotOnPlane(0, 3, q, alongWhichAxis, true);
        DrawDotOnPlane(3, 0, q, alongWhichAxis, true);
        DrawDotOnPlane(3, 3, q, alongWhichAxis, true);

        delay(interval);

        // erase the rect on this layer except the topmost
        if (z < 3)
            SlowDrawRect(z, alongWhichAxis, false, 0);
    }
}

void LowerCube(byte alongWhichAxis, int interval)
{
    // Animate that rect lowering and erasing pillars
    for (int z = 3; z >= 0; z--)
    {
        // draw the rect on this layer
        if (z > 0) SlowDrawRect(z, alongWhichAxis, false, 0);
        delay(interval);
    }

    // Animate erasing rect at bottom of cube
    delay(interval);
    SlowDrawRect(0, alongWhichAxis, false, interval);
}

void RaiseAndLowerCube(int interval, int pause)
{
    RaiseCube(alongZ, interval);
    delay(pause);
    LowerCube(alongZ, interval);
    delay(pause);
}
