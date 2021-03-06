
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

const bool rightToLeft = true;
const bool leftToRight = false;

const byte alongZ = 10;
const byte alongY = 11;
const byte alongX = 12;

#define DEBUG false

byte wall[][2] = {
    { 2,3},{ 1,3},{ 0,3 }, // Back
        { 0,2},{ 0,1},{ 0,0}, // Left
        { 1,0},{ 2,0},{ 3,0}, // Front
        { 3,1},{ 3,2},{ 3,3}, // Right
    };


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
    delay(1000);
    All(off);
}

void loop()
{
    ShowOff();
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

void ShowOff()
{
    All(off);
    DropAndVanishCube(100, 1000); // (Interval, Pause)
    //RaiseAndLowerCube(100, 1000); // (Interval, Pause)

    Sweeps();

    PaintWallAnimations(100);

    All(off);

    Circles(alongZ, 150); // (alongWhichAxis, interval)

    Spinners();

    //CorkScrews(60, 500); // (interval, pause)
    //CorkScrew(alongX, 60); // Circle is on YZ plane, X left to right
    CorkScrew(alongZ, 60); // Circle is on XY plane,Z upward

    JumpingWater();

    //Hat3D(150);
    ////Scan(on, noTrail, 100); // Slowly Scan every pixel
    //Scan(on, hasTrail, 100); // Go through all pixels again, but this time leave it on.

    Hat2D(150, 4);

    ScrollMessage("---THANK YOU!!", 225);

    Blinks(500, 3);
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

    // PatternUp(byte* pattern, int patternLen, int tailLen, bool wrapAround, byte alongWhichAxis, byte atDepth, int interval)
    byte depth = 255;
    PatternUp(*hop, sizeof(hop) / 2, 3, false, alongX, 0, interval);
    PatternDown(*hop, sizeof(hop) / 2, 3, false, alongX, 1, interval);
    PatternUp(*hop, sizeof(hop) / 2, 3, false, alongX, 2, interval);
    PatternDown(*hop, sizeof(hop) / 2, 3, false, alongX, 3, interval);

    PatternUp(*hop, sizeof(hop) / 2, 3, false, alongY, 0, interval);
    PatternDown(*hop, sizeof(hop) / 2, 3, false, alongY, 1, interval);
    PatternUp(*hop, sizeof(hop) / 2, 3, false, alongY, 2, interval);
    PatternDown(*hop, sizeof(hop) / 2, 3, false, alongY, 3, interval);
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
    //CorkScrew(alongZ, interval); delay(pause); // Circle is on XY plane,Z upward
    //CorkScrew(alongY, interval); delay(pause); // Circle is on XZ plane,Y towards rear
    CorkScrew(alongX, interval); delay(pause);// Circle is on YZ plane, X left to right
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

void DrawLineThruPlane(byte a, byte b, byte alongWhichAxis, boolean isOn)
{
    //Serial.println(  String(c) + "," + String(r) + ": " + (isOn ? "ON" : "off"));
    switch (alongWhichAxis)
    {
        case alongZ: DrawLineThruXY(a, b, isOn); break; // line is along Z axis
        case alongY: DrawLineThruXZ(a, b, isOn); break; // Line is along Y axis
        case alongX: DrawLineThruYZ(a, b, isOn); break; // Line is along X axis
    }
}

void DrawDotOnPlane(byte a, byte b, byte c, byte alongWhichAxis, boolean isOn)
{
    //Serial.println(  String(c) + "," + String(r) + ": " + (isOn ? "ON" : "off"));
    switch (alongWhichAxis)
    {
        case alongZ: DrawDot(a, b, c, isOn); break; // point c is along Z axis
        case alongY: DrawDot(a, c, b, isOn); break; // point c is along Y axis
        case alongX: DrawDot(c, a, b, isOn); break; // Point c is along X axis
    }

}


void PatternUp(byte* pattern, int patternLen, int tailLen, bool wrapAround, byte alongWhichAxis, byte atDepth, int interval)
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
            DrawLineThruPlane(x, y, alongWhichAxis, false);
        else
            DrawDotOnPlane(x, y, atDepth, alongWhichAxis, false);

        if (i < patternLen)
        {
            int j = (i >= patternLen) ? i - patternLen : i;
            if (DEBUG) Serial.print(String(j) + "=ON  ");
            byte x = pattern[j * 2 + 0];
            byte y = pattern[j * 2 + 1];
            if (atDepth == 255)
                DrawLineThruPlane(x, y, alongWhichAxis, true);
            else
                DrawDotOnPlane(x, y, atDepth, alongWhichAxis, true);

        }

        if (DEBUG) Serial.println();
        delay(interval);
    }
}

void PatternDown(byte* pattern, byte patternLen, int tailLen, bool wrapAround, byte alongWhichAxis, byte atDepth, int interval)
{
    for (int i = patternLen - 1; i >= -tailLen; i--)
    {
        if (i >= 0)
        {
            byte x = pattern[i * 2 + 0];
            byte y = pattern[i * 2 + 1];
            if (atDepth == 255)
                DrawLineThruPlane(x, y, alongWhichAxis, true);
            else
                DrawDotOnPlane(x, y, atDepth, alongWhichAxis, true);
        }

        byte k = i + tailLen - 1;
        if (k <= patternLen)
        {
            byte x = pattern[k * 2 + 0];
            byte y = pattern[k * 2 + 1];
            if (atDepth == 255)
                DrawLineThruPlane(x, y, alongWhichAxis, false);
            else
                DrawDotOnPlane(x, y, atDepth, alongWhichAxis, false);
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


void DrawFrame(byte* pattern, byte frameLen, byte alongWhichAxis, byte frame, byte dot, byte offset, bool isOn)
{
    byte x = pattern[(frame * frameLen + dot) * 2 + 0];
    byte y = pattern[(frame * frameLen + dot) * 2 + 1];
    DrawDotOnPlane(x, y, offset, alongWhichAxis, isOn);
}


void Animate(byte* pattern, byte frameLen, byte alongWhichAxis, byte frame, int interval)
{
    //-- Current frame twice: once to draw, second to erase--
    for (int d = 0; d < 2; d++)
    {
        for (int i = 0; i < frameLen; i++)
        {
            byte x = pattern[(frame * frameLen + i) * 2 + 0];
            byte y = pattern[(frame * frameLen + i) * 2 + 1];
            bool isOn = d == 0;
            //DrawDot(x, y, 3, isOn);
            DrawDotOnPlane(x, y, 0, alongY, isOn);
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

        // If this is a blank space, make it only 2 columns wide. Otherwise...
        // Each character is 5 columns wide, but I loop two more times to create 2 pixel space between characters
        int columnMax = alphaIndex == 0 ? 3 : 5 + 2;

        for (int column = 0; column < columnMax; column++)
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

void DropCube(byte alongWhichAxis, int interval)
{
    // Animate rect at TOP of cube
    SlowDrawRect(3, alongWhichAxis, true, interval);
    delay(interval);

    // Animate that rect dropping and leaving trail of edges
    for (int z = 2; z >= 0; z--)
    {
        // draw the rect on this layer
        SlowDrawRect(z, alongWhichAxis, true, 0);

        // Draw the pillar trail
        int q = z + 1;
        DrawDotOnPlane(0, 0, q, alongWhichAxis, true);
        DrawDotOnPlane(0, 3, q, alongWhichAxis, true);
        DrawDotOnPlane(3, 0, q, alongWhichAxis, true);
        DrawDotOnPlane(3, 3, q, alongWhichAxis, true);

        delay(interval);

        // erase the rect on this layer except the lowest
        if (z > 0)
            SlowDrawRect(z, alongWhichAxis, false, 0);
    }
}

void VanishCube(byte alongWhichAxis, int interval)
{
    // Animate that rect raising and erasing pillars
    for (int z = 0; z < 3; z++)
    {
        // draw the rect on this layer
        if (z < 3) SlowDrawRect(z, alongWhichAxis, false, 0);
        delay(interval);
    }

    // Animate erasing rect at TOP of cube
    delay(interval);
    SlowDrawRect(3, alongWhichAxis, false, interval);
}

void DropAndVanishCube(int interval, int pause)
{
    DropCube(alongZ, interval);
    delay(pause);
    VanishCube(alongZ, interval);
    delay(pause);
}



void Hat2D(int interval, int howManyTimes)
{
    byte hat[][2] = {
        { 0,0}, { 0,0},{ 1,0}, { 2,0},{ 3,0}, { 3,0}, // bottom line only
       { 0,0}, { 0,0},{ 1,1}, { 2,1},{ 3,0}, { 3,0}, // bottom middle up one
       { 0,1}, { 0,1},{ 1,2}, { 2,2},{ 3,1}, { 3,1}, // bottom middle up two
       { 0,1}, { 0,2},{ 1,3}, { 2,3},{ 3,2}, { 3,1}, // bottom middle up three

       { 0,3}, { 0,2},{ 1,2}, { 2,2},{ 3,2}, { 3,3}, // bottom middle down, side up
       { 0,2}, { 0,2},{ 1,1}, { 2,1},{ 3,2}, { 3,2}, // bottom middle down more, keep side
       { 0,1}, { 0,2},{ 1,0}, { 2,0},{ 3,2}, { 3,1}, // bottom middle down more, keep side for one more frame
       { 0,1}, { 0,1},{ 1,0}, { 2,0},{ 3,1}, { 3,1}, // bottom middle already bottom out, move side down
    };

    byte patternLen = sizeof(hat);
    byte numFrames = 7;
    byte frameLen = patternLen / 2 / numFrames;
    for (int n = 0; n < howManyTimes; n++) // How many times to play this animation?
        for (int f = 0; f < numFrames; f++)
        {
            for (int d = 0; d < 2; d++) //-- Current frame twice: once to draw, second to erase--
            {
                bool isOn = d == 0;
                for (int dot = 0; dot < frameLen; dot++)
                {
                    DrawFrame(*hat, frameLen, alongY, f, dot, 0, isOn);
                    DrawFrame(*hat, frameLen, alongX, f, dot, 0, isOn);
                    DrawFrame(*hat, frameLen, alongY, f, dot, 3, isOn);
                    DrawFrame(*hat, frameLen, alongX, f, dot, 3, isOn);
                }
                if (d == 0) delay(interval);
            }
        }
}

// Animated 3D hat
void Hat3D(int interval)
{
    byte hat[][2] = {
        { 0,0}, { 0,0},{ 1,0}, // bottom line only
       { 0,0}, { 0,0},{ 1,1}, // bottom middle up one
       { 0,1}, { 0,1},{ 1,2}, // bottom middle up two
       { 0,1}, { 0,2},{ 1,3}, // bottom middle up three

       { 0,3}, { 0,2},{ 1,2}, // bottom middle down, side up
       { 0,2}, { 0,2},{ 1,1}, // bottom middle down more, keep side
       { 0,1}, { 0,2},{ 1,0}, // bottom middle down more, keep side for one more frame
       { 0,1}, { 0,1},{ 1,0}, // bottom middle already bottom out, move side down
    };

    byte patternLen = sizeof(hat);
    byte numFrames = 7;
    byte frameLen = patternLen / 2 / numFrames;
    for (int n = 0; n < 7; n++) // How many times to play this animation?
        for (int f = 0; f < numFrames; f++)
            DrawHat3D(*hat, frameLen, f, interval);
}


void DrawHat3D(byte* pattern, byte frameLen, byte frame, int interval)
{
    //-- Current frame twice: once to draw, second to erase--
    for (int d = 0; d < 2; d++)
    {
        bool isOn = d == 0;
        for (int i = 0; i < frameLen; i++)
        {
            byte x = pattern[(frame * frameLen + i) * 2 + 0];
            byte z = pattern[(frame * frameLen + i) * 2 + 1];

            // To turn the quarter flat animation frames on XZ plane to 3D:
            // - When X is 0: draw a circle on the XY plane at height Z
            // - When X is 1: draw a 2x2 rect in the middle of the XY plane at height Z also.

            byte x1 = x + 1;
            byte x2 = x + 2;
            if (x == 0)
            {
                DrawDot(x1, 0, z, isOn); DrawDot(x2, 0, z, isOn); // Front
                DrawDot(x1, 3, z, isOn); DrawDot(x2, 3, z, isOn); // Rear
                DrawDot(0, x1, z, isOn); DrawDot(0, x2, z, isOn); // Left
                DrawDot(3, x1, z, isOn); DrawDot(3, x2, z, isOn); // Right
            }
            else
            {
                DrawDot(x, 1, z, isOn); DrawDot(x1, 1, z, isOn); // Inner Front
                DrawDot(x, 2, z, isOn); DrawDot(x1, 2, z, isOn); // Inner Rear
            }

            //DrawDotOnPlane(x, z, y, alongY, isOn);

            //// Now mirror that 3 times along Z center of the cube
            //DrawDotOnPlane(3-x, z, y, alongY, isOn); // Front right edge
            //DrawDotOnPlane(x, z, 3-y, alongY, isOn); // Rear left edge

            //DrawDotOnPlane(3-x, z, 3 - y, alongY, isOn); // Rear right edge
        }

        if (d == 0) delay(interval);
    }
}

void DrawOneSlice(int* pattern, int offset)
{
    byte msbOffset = 15 - offset;
    for (int z = 0; z < 4; z++)
    {
        byte bit = bitRead(pattern[z], msbOffset);
        cube[3][3][3 - z] = bit != 0;
    }
}


void ShiftOuterWalls(bool rightToLeft)
{
    if (DEBUG)
    {
        Serial.println();
        Serial.print("righttoLeft=");
        Serial.println(rightToLeft);
    }

    for (int z = 0; z < 4; z++)
    {
        bool saved;
        byte wLen = sizeof(wall) / 2;
        for (int i = 0; i < wLen; i++)
        {
            int t = rightToLeft ? i : wLen - 1 - i;
            byte targetX = wall[t][0];
            byte targetY = wall[t][1];
            if (i == 0)
            {
                saved = cube[targetX][targetY][z];
                if (DEBUG && z == 0) { Serial.print("Save t="); Serial.println(t); }
            }

            if (i == (wLen - 1))
            {
                cube[targetX][targetY][z] = saved;
                if (DEBUG && z == 0) { Serial.print("restore t="); Serial.println(t); }
            }
            else
            {
                int s = rightToLeft ? t + 1 : t - 1;
                byte srcX = wall[s][0];
                byte srcY = wall[s][1];
                if (DEBUG && z == 0)
                {
                    Serial.print("copy s="); Serial.print(s);
                    Serial.print("to t="); Serial.println(t);
                }
                cube[targetX][targetY][z] = cube[srcX][srcY][z];
            }
        }
    }

}


void AnimateWalls(int interval, bool rightToLeft, int howManyColumns)
{
    for (int c = 0; c < howManyColumns; c++)
    {
        ShiftOuterWalls(rightToLeft);
        delay(interval);
    }
}

void PaintWall(int* pattern, int interval, bool rightToLeft)
{
    for (int column = 0; column < 12; column++)
    {
        ShiftOuterWalls(rightToLeft);
        DrawOneSlice(pattern, column);
        if (interval > 0) delay(interval);
    }
}

void PaintWallSpace(int interval, bool rightToLeft)
{
    for (int column = 0; column < 4; column++)
    {
        ShiftOuterWalls(rightToLeft);
        DrawLineThruXY(3, 3, off);
        if (interval > 0) delay(interval);
    }
}

void PaintWallSine(int interval, bool rightToLeft)
{
    //int sineTall[] =
    //{
    //    0B0110000110000000,
    //    0B1001001001000000,
    //    0B1001001001000000,
    //    0B0000110000110000,
    //};

    int sine[] =
    {
        0B0000000000000000,
        0B0110000110000000,
        0B1001001001000000,
        0B0000110000110000
    };

    PaintWall(sine, interval, rightToLeft);
}


void PaintWallBox(int interval, bool rightToLeft)
{
    int box[] =
    {
        0B1111000000000000,
        0B1001000000000000,
        0B1001000000000000,
        0B1111000000000000,
    };

    PaintWallWithPauses(box, interval, rightToLeft);
}

void PaintWallArrow(int interval, bool rightToLeft)
{
    int arrow[] =
    {
        0B0000000000000000,
        0B0100000000000000,
        0B1111100000000000,
        0B0100000000000000,
    };

    PaintWall(arrow, interval, rightToLeft);
}


void PaintWallSlash(int interval, bool rightToLeft)
{
    int slash[] =
    {
        0B0001000100010000,
        0B0011001100100000,
        0B0101010101000000,
        0B1001100110000000,
    };

    PaintWall(slash, interval, rightToLeft);
}

void PaintWallSawtooth(int interval, bool rightToLeft)
{
    int sawtooth[] =
    {
        0B0001000001000000,
        0B0010100010100000,
        0B0100010100010000,
        0B1000001000001000,
    };

    int sawtooth2[] =
    {
        0B0000000000000000,
        0B0010001000100000,
        0B0101010101010000,
        0B1000100010000000,
    };

    PaintWall(sawtooth2, interval, rightToLeft);
}

void PaintWallWithPauses(int* pattern, int interval, bool rightToLeft)
{
    for (int column = 0; column < 12; column++)
    {
        ShiftOuterWalls(rightToLeft);
        DrawOneSlice(pattern, column);
        if (interval > 0) delay(interval);
        if ((column % 3) == 0) delay(500);
    }
}

void PaintWallAnimations(int interval)
{
    PaintWallBox(interval, rightToLeft);
    AnimateWalls(interval, rightToLeft, 12);
    PaintWallSpace(interval, rightToLeft);

    PaintWallSine(interval, leftToRight);
    AnimateWalls(interval, leftToRight, 12);
    PaintWallSpace(interval, leftToRight);

    //PaintWallSawtooth(interval, rightToLeft);
    //AnimateWalls(interval, rightToLeft, 12);
    //PaintWallSpace(interval, rightToLeft);

    PaintWallArrow(interval, leftToRight);
    AnimateWalls(interval, leftToRight, 12);
    AnimateWalls(interval, rightToLeft, 12);
    PaintWallSpace(interval, rightToLeft);

    //paintwallslash(interval, righttoleft);
    //animatewalls(interval, righttoleft, 12);
    //paintwallspace(interval, righttoleft);
}

void Race()
{

}
