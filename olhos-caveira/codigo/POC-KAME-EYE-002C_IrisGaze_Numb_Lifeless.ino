#include "LGFX_DinoKame.h"

// ============================================================
// KAME STUDIO
// POC-KAME-EYE-002C
//
// IRIS GAZE + NUMB / LIFELESS
//
// NORMAL:
//   - Iris + pupil move together
//   - Black pupil
//
// NUMB:
//   - Iris remains visible
//   - Gaze becomes slow/minimal
//   - Pupil becomes light gray
//   - After a few seconds, returns to NORMAL
//
// ============================================================


// ============================================================
// DISPLAYS
// ============================================================

LGFX_DinoKame leftDisplay(DISPLAY_LEFT_CS_PIN);
LGFX_DinoKame rightDisplay(DISPLAY_RIGHT_CS_PIN);


// ============================================================
// GEOMETRY
// ============================================================

constexpr int EYE_CX = 120;
constexpr int EYE_CY = 120;

constexpr int SCLERA_RADIUS = 105;

constexpr int OUTER_RADIUS = 65;
constexpr int IRIS_RADIUS  = 55;

constexpr int PUPIL_RADIUS = 25;


// ============================================================
// GAZE POSITIONS
// ============================================================

constexpr int GAZE_LEFT   = 90;
constexpr int GAZE_CENTER = 120;
constexpr int GAZE_RIGHT  = 150;

constexpr int GAZE_UP     = 90;
constexpr int GAZE_DOWN   = 150;


// ============================================================
// FRAMEBUFFER
// ============================================================

constexpr int IRIS_BUFFER_WIDTH  = 201;
constexpr int IRIS_BUFFER_HEIGHT = 201;

constexpr int IRIS_BUFFER_X = 20;
constexpr int IRIS_BUFFER_Y = 20;


uint16_t irisBuffer[
    IRIS_BUFFER_WIDTH * IRIS_BUFFER_HEIGHT
];


// ============================================================
// COLORS
// ============================================================

constexpr uint16_t COLOR_BACKGROUND = TFT_BLACK;
constexpr uint16_t COLOR_SCLERA     = TFT_LIGHTGREY;
constexpr uint16_t COLOR_OUTER      = TFT_MAROON;
constexpr uint16_t COLOR_IRIS       = TFT_BROWN;

constexpr uint16_t COLOR_NORMAL_PUPIL = TFT_BLACK;
constexpr uint16_t COLOR_NUMB_PUPIL   = TFT_LIGHTGREY;


// ============================================================
// NUMB CONFIGURATION
// ============================================================

constexpr int NUMB_CHANCE_PERCENT = 3;

constexpr unsigned long NUMB_MIN_TIME = 2500;
constexpr unsigned long NUMB_MAX_TIME = 5000;


// ============================================================
// EYE STATE
// ============================================================

enum EyeState
{
    EYE_NORMAL,
    EYE_NUMB
};

EyeState eyeState = EYE_NORMAL;


// ============================================================
// NUMB TIMING
// ============================================================

unsigned long numbStartedAt = 0;
unsigned long numbDuration  = 0;


// ============================================================
// RGB565 BYTE SWAP
// ============================================================

inline uint16_t swapRGB565(uint16_t color)
{
    return (color >> 8) | (color << 8);
}


void swapBufferBytes(
    uint16_t* buffer,
    int count
)
{
    for (int i = 0; i < count; i++)
    {
        buffer[i] = swapRGB565(buffer[i]);
    }
}


// ============================================================
// CIRCLE
// ============================================================

inline bool insideCircle(
    int x,
    int y,
    int cx,
    int cy,
    int radius
)
{
    int dx = x - cx;
    int dy = y - cy;

    return
        (dx * dx + dy * dy) <=
        (radius * radius);
}


// ============================================================
// STATIC EYE
// ============================================================

void drawStaticEye(
    LGFX_DinoKame& display
)
{
    display.fillScreen(
        COLOR_BACKGROUND
    );

    // --------------------------------------------------------
    // ESCLERA
    // --------------------------------------------------------

    display.fillCircle(
        EYE_CX,
        EYE_CY,
        SCLERA_RADIUS,
        COLOR_SCLERA
    );

    // --------------------------------------------------------
    // OUTER RING
    // --------------------------------------------------------

    display.fillCircle(
        EYE_CX,
        EYE_CY,
        OUTER_RADIUS,
        COLOR_OUTER
    );

    // --------------------------------------------------------
    // IRIS
    // --------------------------------------------------------

    display.fillCircle(
        EYE_CX,
        EYE_CY,
        IRIS_RADIUS,
        COLOR_IRIS
    );

    // --------------------------------------------------------
    // PUPIL
    // --------------------------------------------------------

    display.fillCircle(
        EYE_CX,
        EYE_CY,
        PUPIL_RADIUS,
        COLOR_NORMAL_PUPIL
    );
}


// ============================================================
// BUILD IRIS BUFFER
// ============================================================
//
// irisX / irisY:
//     position of the entire iris.
//
// pupilColor:
//     black during NORMAL
//     light gray during NUMB
//
// ============================================================

void buildIrisBuffer(
    uint16_t* buffer,
    int irisX,
    int irisY,
    uint16_t pupilColor
)
{
    for (
        int y = 0;
        y < IRIS_BUFFER_HEIGHT;
        y++
    )
    {
        for (
            int x = 0;
            x < IRIS_BUFFER_WIDTH;
            x++
        )
        {
            int screenX =
                IRIS_BUFFER_X + x;

            int screenY =
                IRIS_BUFFER_Y + y;


            // ------------------------------------------------
            // Background
            // ------------------------------------------------

            uint16_t color =
                COLOR_BACKGROUND;


            // ------------------------------------------------
            // Sclera
            // ------------------------------------------------

            if (
                insideCircle(
                    screenX,
                    screenY,
                    EYE_CX,
                    EYE_CY,
                    SCLERA_RADIUS
                )
            )
            {
                color = COLOR_SCLERA;
            }


            // ------------------------------------------------
            // Outer ring
            // ------------------------------------------------

            if (
                insideCircle(
                    screenX,
                    screenY,
                    irisX,
                    irisY,
                    OUTER_RADIUS
                )
            )
            {
                color = COLOR_OUTER;
            }


            // ------------------------------------------------
            // Iris
            // ------------------------------------------------

            if (
                insideCircle(
                    screenX,
                    screenY,
                    irisX,
                    irisY,
                    IRIS_RADIUS
                )
            )
            {
                color = COLOR_IRIS;
            }


            // ------------------------------------------------
            // Pupil
            //
            // Pupil remains centered inside the iris.
            // ------------------------------------------------

            if (
                insideCircle(
                    screenX,
                    screenY,
                    irisX,
                    irisY,
                    PUPIL_RADIUS
                )
            )
            {
                color = pupilColor;
            }


            buffer[
                y * IRIS_BUFFER_WIDTH + x
            ] = color;
        }
    }
}


// ============================================================
// RENDER IRIS
// ============================================================

void renderIris(
    int irisX,
    int irisY
)
{
    uint16_t pupilColor =
        COLOR_NORMAL_PUPIL;


    if (eyeState == EYE_NUMB)
    {
        pupilColor =
            COLOR_NUMB_PUPIL;
    }


    // --------------------------------------------------------
    // Build
    // --------------------------------------------------------

    buildIrisBuffer(
        irisBuffer,
        irisX,
        irisY,
        pupilColor
    );


    // --------------------------------------------------------
    // RGB565 byte order
    // --------------------------------------------------------

    swapBufferBytes(
        irisBuffer,
        IRIS_BUFFER_WIDTH *
        IRIS_BUFFER_HEIGHT
    );


    // --------------------------------------------------------
    // Both eyes
    // --------------------------------------------------------

    leftDisplay.pushImage(
        IRIS_BUFFER_X,
        IRIS_BUFFER_Y,
        IRIS_BUFFER_WIDTH,
        IRIS_BUFFER_HEIGHT,
        irisBuffer
    );


    rightDisplay.pushImage(
        IRIS_BUFFER_X,
        IRIS_BUFFER_Y,
        IRIS_BUFFER_WIDTH,
        IRIS_BUFFER_HEIGHT,
        irisBuffer
    );
}


// ============================================================
// EASING
// ============================================================

float easeInOut(
    float t
)
{
    return
        t * t *
        (3.0f - 2.0f * t);
}


// ============================================================
// NORMAL GAZE MOVEMENT
// ============================================================

void moveGaze(
    int startX,
    int startY,
    int endX,
    int endY,
    unsigned long duration
)
{
    unsigned long startTime =
        millis();


    while (true)
    {
        unsigned long elapsed =
            millis() - startTime;


        if (elapsed >= duration)
            break;


        float t =
            (float)elapsed /
            (float)duration;


        float e =
            easeInOut(t);


        int x =
            startX +
            (int)(
                (endX - startX) * e
            );


        int y =
            startY +
            (int)(
                (endY - startY) * e
            );


        renderIris(
            x,
            y
        );


        delay(15);
    }


    renderIris(
        endX,
        endY
    );
}


// ============================================================
// ENTER NUMB
// ============================================================

void enterNumb()
{
    eyeState = EYE_NUMB;

    numbStartedAt = millis();

    numbDuration =
        random(
            NUMB_MIN_TIME,
            NUMB_MAX_TIME + 1
        );


    Serial.println(
        ">>> NUMB / LIFELESS"
    );


    // --------------------------------------------------------
    // Immediately show the gray pupil.
    // --------------------------------------------------------

    renderIris(
        GAZE_CENTER,
        GAZE_CENTER
    );
}


// ============================================================
// EXIT NUMB
// ============================================================

void exitNumb()
{
    eyeState = EYE_NORMAL;

    Serial.println(
        "<<< NORMAL"
    );


    renderIris(
        GAZE_CENTER,
        GAZE_CENTER
    );
}


// ============================================================
// CHECK NUMB STATE
// ============================================================

void updateNumb()
{
    if (eyeState != EYE_NUMB)
        return;


    if (
        millis() - numbStartedAt
        >= numbDuration
    )
    {
        exitNumb();
    }
}


// ============================================================
// RANDOM BEHAVIOR DECISION
// ============================================================
//
// Called when the normal behavior reaches a new cycle.
//
// 3% chance -> NUMB
//
// ============================================================

void maybeEnterNumb()
{
    int value =
        random(100);


    if (
        value < NUMB_CHANCE_PERCENT
    )
    {
        enterNumb();
    }
}


// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);


    Serial.println();
    Serial.println(
        "========================================"
    );
    Serial.println(
        " KAME STUDIO"
    );
    Serial.println(
        " POC-KAME-EYE-002C"
    );
    Serial.println(
        " IRIS GAZE + NUMB"
    );
    Serial.println(
        "========================================"
    );


    // --------------------------------------------------------
    // CS
    // --------------------------------------------------------

    pinMode(
        DISPLAY_LEFT_CS_PIN,
        OUTPUT
    );

    pinMode(
        DISPLAY_RIGHT_CS_PIN,
        OUTPUT
    );


    // --------------------------------------------------------
    // Validated initialization
    // --------------------------------------------------------

    digitalWrite(
        DISPLAY_LEFT_CS_PIN,
        LOW
    );


    if (!rightDisplay.init())
    {
        Serial.println(
            "ERRO: Display direito nao iniciou!"
        );
    }


    leftDisplay.setRotation(0);
    rightDisplay.setRotation(2);


    // --------------------------------------------------------
    // Initial static eye
    // --------------------------------------------------------

    drawStaticEye(
        leftDisplay
    );

    drawStaticEye(
        rightDisplay
    );


    // --------------------------------------------------------
    // Deselect
    // --------------------------------------------------------

    digitalWrite(
        DISPLAY_LEFT_CS_PIN,
        HIGH
    );

    digitalWrite(
        DISPLAY_RIGHT_CS_PIN,
        HIGH
    );


    // --------------------------------------------------------
    // Random seed
    // --------------------------------------------------------

    randomSeed(
        micros()
    );


    Serial.println(
        "Eye Engine ready."
    );
}


// ============================================================
// LOOP
// ============================================================

void loop()
{
    // ========================================================
    // If NUMB is active, do nothing except monitor its timer.
    // ========================================================

    if (eyeState == EYE_NUMB)
    {
        updateNumb();

        delay(20);

        return;
    }


    // ========================================================
    // NORMAL BEHAVIOR
    // ========================================================

    maybeEnterNumb();

    if (eyeState == EYE_NUMB)
        return;


    // --------------------------------------------------------
    // CENTER -> UP
    // --------------------------------------------------------

    moveGaze(
        GAZE_CENTER,
        GAZE_CENTER,
        GAZE_CENTER,
        GAZE_UP,
        700
    );

    delay(500);


    // --------------------------------------------------------
    // UP -> CENTER
    // --------------------------------------------------------

    moveGaze(
        GAZE_CENTER,
        GAZE_UP,
        GAZE_CENTER,
        GAZE_CENTER,
        700
    );

    delay(500);


    // --------------------------------------------------------
    // CENTER -> DOWN
    // --------------------------------------------------------

    moveGaze(
        GAZE_CENTER,
        GAZE_CENTER,
        GAZE_CENTER,
        GAZE_DOWN,
        700
    );

    delay(500);


    // --------------------------------------------------------
    // DOWN -> CENTER
    // --------------------------------------------------------

    moveGaze(
        GAZE_CENTER,
        GAZE_DOWN,
        GAZE_CENTER,
        GAZE_CENTER,
        700
    );

    delay(500);


    // --------------------------------------------------------
    // CENTER -> LEFT
    // --------------------------------------------------------

    moveGaze(
        GAZE_CENTER,
        GAZE_CENTER,
        GAZE_LEFT,
        GAZE_CENTER,
        700
    );

    delay(500);


    // --------------------------------------------------------
    // LEFT -> CENTER
    // --------------------------------------------------------

    moveGaze(
        GAZE_LEFT,
        GAZE_CENTER,
        GAZE_CENTER,
        GAZE_CENTER,
        700
    );

    delay(500);


    // --------------------------------------------------------
    // CENTER -> RIGHT
    // --------------------------------------------------------

    moveGaze(
        GAZE_CENTER,
        GAZE_CENTER,
        GAZE_RIGHT,
        GAZE_CENTER,
        700
    );

    delay(500);


    // --------------------------------------------------------
    // RIGHT -> CENTER
    // --------------------------------------------------------

    moveGaze(
        GAZE_RIGHT,
        GAZE_CENTER,
        GAZE_CENTER,
        GAZE_CENTER,
        700
    );

    delay(500);


    // --------------------------------------------------------
    // CENTER -> UP-LEFT
    // --------------------------------------------------------

    moveGaze(
        GAZE_CENTER,
        GAZE_CENTER,
        GAZE_LEFT,
        GAZE_UP,
        700
    );

    delay(500);


    // --------------------------------------------------------
    // UP-LEFT -> CENTER
    // --------------------------------------------------------

    moveGaze(
        GAZE_LEFT,
        GAZE_UP,
        GAZE_CENTER,
        GAZE_CENTER,
        700
    );

    delay(500);


    // --------------------------------------------------------
    // CENTER -> DOWN-RIGHT
    // --------------------------------------------------------

    moveGaze(
        GAZE_CENTER,
        GAZE_CENTER,
        GAZE_RIGHT,
        GAZE_DOWN,
        700
    );

    delay(500);


    // --------------------------------------------------------
    // DOWN-RIGHT -> CENTER
    // --------------------------------------------------------

    moveGaze(
        GAZE_RIGHT,
        GAZE_DOWN,
        GAZE_CENTER,
        GAZE_CENTER,
        700
    );

    delay(1000);
}