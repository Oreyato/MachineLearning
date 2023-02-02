#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "ULRegression.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>

#include <fstream>
#include <sstream>

using namespace std;

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screenWidth = 1000;
static const int screenHeight = 500;
static const char* windowName = "Univariate Linear Regression";

static float theta_0 = 0.0f;
static float theta_1 = 0.0f;

static vector<Thetas> thetasSets;

static Datas datas;

//v Graph values =================================================
static Color graphBGColor = RAYWHITE;

static float graphOffset = 60.0f;
static float xPadding = 10.0f;
static float yPadding = 10.0f;
static float xMaxValue = 110.0f;
static float yMaxValue = 115.0f;
static float xMinValue = 0.0f;
static float yMinValue = 0.0f;

static Color subLinesColor = Color{ 200, 200, 200, 255 };
static Color thiSubLinesColor = Color{ 125, 125, 125, 255 };
static int8_t thickerLinesFrequency = 5;
static float marksTextSize = 15.0f;

static float axisThickness = 4.0f;

static float pointRadius = 2.0f;

//^ Graph values =================================================
//v Thetas curves ================================================
static bool showAllCurves = true;

static float thetasCurvesThickness = 2.0f;
static Color thetasCurvesColor = RED;

//^ Thetas curves ================================================

//v EDUCATIONAL CONTENT ==========================================
static bool showEducationalContent = true;

static float eduThetasCurveThickness = 4.0f;
static Color eduThetasCurveColor = GREEN;

static int inputXValue = 0;
static char inputXText = '0';
static bool XValueBoxEditMode = false;

//v Sliders =============================
static float theta0Slider = 0.0f;
static float theta1Slider = 0.0f;

static float alphaSlider = 0.0001f;
static float thresholdSlider = 0.00001f;

// ===

static float minT0SliderValue = 0.0f;
static float maxT0SliderValue = 2.0f;

static float minT1SliderValue = -0.25f;
static float maxT1SliderValue = 1.5f;

static float minAlphaSliderValue = 0.00001f;
static float maxAlphaSliderValue = 0.001f;

static float minThresholdSliderValue = 0.000001f;
static float maxThresholdSliderValue = 0.0001f;

//^ Sliders =============================
//^ EDUCATIONAL CONTENT ========================================== 
 
//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------

static void Init(void);

static void Inputs(void);

static void UpdateDrawFrame(void);
static void Update(void);
static void DrawUI(void);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, windowName);

    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second

    Init();
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
        Inputs();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

void Init() {
    //v Retrieve training values =====================================
    const vector<vector<float>> retrievedData = ULRegression::RetrieveCsvFileData("Resources/train.csv");

    const vector<float> xVal{ retrievedData[0] };
    const vector<float> yVal{ retrievedData[1] };

    datas.x = retrievedData[0];
    datas.y = retrievedData[1];
    datas.size = datas.x.size();

    //^ Retrieve training values =====================================
    //v Testing purpose ==============================================
    /*const vector<float> xVal{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f };
    const vector<float> yVal{ 0.25f, 0.50f, 0.75f, 1.0f, 1.25f, 1.5f, 1.75f, 2.0f, 2.25f, 2.50f };*/
    //^ Testing purpose ==============================================

    float theta_0{ 0.0f };
    float theta_1{ 0.0f };

    float xTest = xVal[2];
    float yExpected = yVal[2];

    //v Prediction test, before gradient descent =====================
    float firstHypothesis = ULRegression::Hypothesis(xTest, theta_0, theta_1);
    cout << "Before using gradient descent, we obtain " << firstHypothesis << " for x = " << xTest << endl;
    cout << "Expected value: " << yExpected << endl;
    cout << "\n\n" << endl;
    //^ Prediction test, before gradient descent =====================

    // Train algorithm
    thetasSets = ULRegression::RefineThetas(theta_0, theta_1, 0.0001f, 0.00001f, xVal, yVal);

    cout << "\n" << endl;

    //v Prediction test, after gradient descent ======================
    float secondHypothesis =  ULRegression::Hypothesis(xTest, theta_0, theta_1);
    cout << "After using gradient descent, we obtain " << secondHypothesis << " for x = " << xTest << endl;
    cout << "Expected value: " << yExpected << endl;

    cout << "\nFinal theta_0: " << theta_0 << endl;
    cout << "Final theta_1: " << theta_1 << endl;
    //^ Prediction test, after gradient descent ======================
}

// Update and draw game frame
void UpdateDrawFrame(void)
{
    Update();

    //----------------------------------------------------------------------------------
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(RAYWHITE);
    //DrawFPS(10, 10);
    DrawUI();

    EndDrawing();
    //----------------------------------------------------------------------------------
}

void Update(void) {

}

void DrawGraph(float graphWidth, float graphHeight, Vector2 graphOrigin) {
    // Draw grid ============================
    // Draw absissa marks
    uint8_t absMarksNb = floor(xMaxValue / xPadding);
    for (uint8_t i = 1; i < absMarksNb + 1; i++)
    {
        float normalizedXPadding = (xPadding * i) * graphWidth / xMaxValue;

        Vector2 startingPos{ graphOffset + normalizedXPadding, graphOrigin.y };
        Vector2 endingPos{ startingPos.x, graphOffset };

        if (i % thickerLinesFrequency != 0) {
            DrawLineEx(startingPos, endingPos, axisThickness / 2.0f, subLinesColor);

            int currentX = floor(xPadding * i);
            DrawText(TextFormat("%i", currentX), startingPos.x - marksTextSize * 0.25f, startingPos.y + graphOffset / 4.0f, marksTextSize * 0.5f, subLinesColor);
        }
        else
        {
            DrawLineEx(startingPos, endingPos, axisThickness / 1.5f, thiSubLinesColor);

            int currentX = floor(xPadding * i);
            DrawText(TextFormat("%i", currentX), startingPos.x - marksTextSize * 0.5f, startingPos.y + graphOffset / 4.0f, marksTextSize, thiSubLinesColor);
        }

    }
    // Draw ordonate marks
    uint8_t ordMarksNb = floor(yMaxValue / yPadding);
    for (uint8_t i = 1; i < ordMarksNb + 1; i++)
    {
        float normalizedYPadding = (yPadding * i) * graphHeight / yMaxValue;

        Vector2 startingPos{ graphOrigin.x, screenHeight - graphOffset - normalizedYPadding };
        Vector2 endingPos{ screenWidth - graphOffset , startingPos.y };

        if (i % thickerLinesFrequency != 0) {
            DrawLineEx(startingPos, endingPos, axisThickness / 2.0f, subLinesColor);

            int currentY = floor(yPadding * i);
            DrawText(TextFormat("%i", currentY), startingPos.x - marksTextSize * 2.0f, startingPos.y - 0.5f * marksTextSize * 0.5f, marksTextSize * 0.5f, subLinesColor);
        }
        else
        {
            DrawLineEx(startingPos, endingPos, axisThickness / 1.5f, thiSubLinesColor);

            int currentY = floor(yPadding * i);
            DrawText(TextFormat("%i", currentY), startingPos.x - marksTextSize * 2.0f, startingPos.y - 0.5f * marksTextSize, marksTextSize, thiSubLinesColor);
        }
    }

    // Draw abscissa ========================
    Vector2 absStartPos{ graphOrigin.x / 2.0f, graphOrigin.y };
    Vector2 absEndPos{ screenWidth - graphOffset, absStartPos.y };

    DrawLineEx(absStartPos, absEndPos, axisThickness, BLACK);

    // Draw ordonate ========================
    Vector2 ordStartPos{ graphOrigin.x, screenHeight - graphOffset / 2.0f };
    Vector2 ordEndPos{ ordStartPos.x, graphOffset };

    DrawLineEx(ordStartPos, ordEndPos, axisThickness, BLACK);
}

void DrawDataPoints(float graphWidth, float graphHeight) {
    for (uint32_t i = 0; i < datas.size; i++)
    {
        // Normalize datas
        float normX = datas.x[i] * graphWidth / xMaxValue;
        float normY = datas.y[i] * graphHeight / yMaxValue;

        // Put them in the graph according to
        // ... windowOffset
        normX += graphOffset;
        normY += graphOffset;

        // ... reversed y axis
        normY = screenHeight - normY;

        DrawCircleV(Vector2{ normX, normY }, pointRadius, BLUE);
    }
}

void DrawThetasCurves(float graphWidth, float graphHeight, Vector2 graphOrigin) {
    if (showAllCurves)
    {
        for (uint32_t i = 0; i < thetasSets.size(); i++)
        {
            const float yHyp =  ULRegression::Hypothesis(xMaxValue, thetasSets[i].zero, thetasSets[i].one);
            const float yHypNorm = yHyp * graphHeight / yMaxValue;

            DrawLineEx(
                Vector2{ graphOrigin.x, graphOrigin.y - thetasSets[i].zero },
                Vector2{ graphOrigin.x + graphWidth, graphOrigin.y - yHypNorm },
                thetasCurvesThickness,
                thetasCurvesColor
            );
        }
    }
    else {
        const float yHyp =  ULRegression::Hypothesis(xMaxValue, thetasSets[thetasSets.size() - 1].zero, thetasSets[thetasSets.size() - 1].one);
        const float yHypNorm = yHyp * graphHeight / yMaxValue;

        DrawLineEx(
            Vector2{ graphOrigin.x, graphOrigin.y - thetasSets[thetasSets.size() - 1].zero },
            Vector2{ graphOrigin.x + graphWidth, graphOrigin.y - yHypNorm },
            thetasCurvesThickness,
            thetasCurvesColor
        );
    }
}

void DrawEduContent(float graphWidth, float graphHeight, Vector2 graphOrigin) {
    //v ==============================================================
    //v Sliders ======================================================
    float slidersWidth = 100.0f;
    float slidersXPos = screenWidth - slidersWidth - 25.0f;
    float slidersYpos = screenHeight - screenHeight * 0.2f;

    theta0Slider = GuiSliderBar(Rectangle{ slidersXPos, slidersYpos - 30.0f, slidersWidth, 20 }, "Theta0", NULL, theta0Slider, minT0SliderValue, maxT0SliderValue);
    theta1Slider = GuiSliderBar(Rectangle{ slidersXPos, slidersYpos, slidersWidth, 20 }, "Theta1", NULL, theta1Slider, minT1SliderValue, maxT1SliderValue);

    thresholdSlider = GuiSliderBar(Rectangle{ slidersXPos, slidersYpos - 90.0f, slidersWidth, 20 }, "Threshold", NULL, thresholdSlider, minAlphaSliderValue, maxAlphaSliderValue);
    alphaSlider = GuiSliderBar(Rectangle{ slidersXPos, slidersYpos - 120.0f, slidersWidth, 20 }, "Alpha", NULL, alphaSlider, minThresholdSliderValue, maxThresholdSliderValue);

    // Draw values
    DrawText(TextFormat("%05f", theta0Slider), slidersXPos + 20.0f, slidersYpos - 27.0f, 15.0f, DARKBLUE);
    DrawText(TextFormat("%05f", theta1Slider), slidersXPos + 20.0f, slidersYpos + 3.0f, 15.0f, DARKBLUE);
    DrawText(TextFormat("%05f", thresholdSlider), slidersXPos + 20.0f, slidersYpos - 87.0f, 15.0f, DARKBLUE);
    DrawText(TextFormat("%05f", alphaSlider), slidersXPos + 20.0f, slidersYpos - 117.0f, 15.0f, DARKBLUE);

    // Draw test curve
    const float yHyp =  ULRegression::Hypothesis(xMaxValue, theta0Slider, theta1Slider);
    const float yHypNorm = yHyp * graphHeight / yMaxValue;

    DrawLineEx(
        Vector2{ graphOrigin.x, graphOrigin.y - theta0Slider },
        Vector2{ graphOrigin.x + graphWidth, graphOrigin.y - yHypNorm },
        eduThetasCurveThickness,
        eduThetasCurveColor
    );

    //^ Sliders ======================================================
    //^ ============================================================== 
}

void DrawUI(void) {
    float graphWidth = screenWidth - 2 * graphOffset;
    float graphHeight = screenHeight - 2 * graphOffset;

    Vector2 graphOrigin{ graphOffset, screenHeight - graphOffset };

    DrawGraph(graphWidth, graphHeight, graphOrigin);
    DrawDataPoints(graphWidth, graphHeight);
    DrawThetasCurves(graphWidth, graphHeight, graphOrigin);

    if (showEducationalContent)
    {
        DrawEduContent(graphWidth, graphHeight, graphOrigin);
    }

    //// Draw caches ==========================
    //DrawRectangle(0, 0, graphOffset, screenHeight, graphBGColor);
    //DrawRectangle(screenWidth - graphOffset, 0, graphOffset, screenHeight, graphBGColor);
    //DrawRectangle(0, screenHeight - graphOffset, screenWidth, graphOffset, graphBGColor);
    //DrawRectangle(0, 0, screenWidth, graphOffset, graphBGColor);

    //v ==============================================================
    //v Texts ========================================================
    //v Thetas text =================

    float finalTextSize = 20.0f;
    uint32_t thetasSetsSize = thetasSets.size();
    Thetas finalThetas{ thetasSets[thetasSetsSize - 1].zero, thetasSets[thetasSetsSize - 1].one };

    DrawText("Final thetas:", 50.f, 5.f, finalTextSize, BLACK);
    DrawText(TextFormat("Theta0: %05f", finalThetas.zero), 80.f, 25.0f, finalTextSize - 2.5f, RED);
    DrawText(TextFormat("Theta1: %05f", finalThetas.one), 80.f, 40.0f, finalTextSize - 2.5f, RED);

    //v Inputs text =================
    if (showAllCurves)
    {
        DrawText("\"C\": only show last thetas curve", 50.f, screenHeight - 25.f, 20.0f, BLACK);
    }
    else {
        DrawText("\"C\": show all thetas curves", 50.f, screenHeight - 25.f, 20.0f, BLACK);
    }
    if (showEducationalContent)
    {
        DrawText("\"E\": hide educational content", screenWidth / 2.0f, screenHeight - 25.f, 20.0f, BLACK);
    }
    else {
        DrawText("\"E\": show educational content", screenWidth / 2.0f, screenHeight - 25.f, 20.0f, BLACK);
    }

    //v Custom X Y value ============ 
    if (showEducationalContent) {
        DrawText("x value:", 325, 15, 20, DARKBLUE);
        
        // GuiValueBox(Rectangle{ 400.0f, 15.0f, 40.0f, 25.0f }, "", &inputXValue, 0, 110, true);
        if (GuiTextBox(Rectangle{ 410, 12, 45, 25 }, &inputXText, 25, true)) XValueBoxEditMode != XValueBoxEditMode;

        const float xVal = atof(&inputXText);
        const float yVal = ULRegression::Hypothesis(xVal, finalThetas.zero, finalThetas.one);
    
        DrawText(TextFormat("y value: %01f", yVal), 475, 15, 20, DARKBLUE);

        // Draw point

        // Normalize datas
        float normX = xVal * graphWidth / xMaxValue;
        float normY = yVal * graphHeight / yMaxValue;

        // Put them in the graph according to
        // ... windowOffset
        normX += graphOffset;
        normY += graphOffset;

        // ... reversed y axis
        normY = screenHeight - normY;

        DrawCircleV(Vector2{ normX, normY }, 5.0f, DARKBLUE);
    }

    //^ Texts ========================================================
    //^ ==============================================================
}

void Inputs(void)
{
    // Show latest curve only
    if (IsKeyPressed(KEY_C)) {
        showAllCurves = !showAllCurves;
    }
    // Show educational content
    if (IsKeyPressed(KEY_E)) {
        showEducationalContent = !showEducationalContent;
    }
}