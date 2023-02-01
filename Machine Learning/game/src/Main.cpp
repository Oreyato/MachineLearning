#include "raylib.h"

#include <iostream>
#include <vector>
#include <math.h>

#include <fstream>
#include <sstream>

using namespace std;

struct Thetas
{
    float zero = 0.0f;
    float one = 0.0f;
};

struct Datas
{
    vector<float> x;
    vector<float> y;
    uint32_t size;
};

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screenWidth = 1000;
static const int screenHeight = 500;
static const char* windowName = "raylib basic game template";

static float theta_0 = 0.0f;
static float theta_1 = 0.0f;

static Thetas thetas;
static vector<Thetas> thetasSets;

static Datas datas;

//v Graph values =================================================
static float graphOffset = 60.0f;
static float xPadding = 10.0f;
static float yPadding = 10.0f;
static Color subLinesColor = Color{ 200, 200, 200, 255 };
static Color thiSubLinesColor = Color{ 125, 125, 125, 255 };
static int8_t thickerLinesFrequency = 5;
static float marksTextSize = 15.0f;

static float axisThickness = 4.0f;

static float pointRadius = 2.0f;
static float xMaxValue = 105.0f;
static float yMaxValue = 115.0f;

//^ Graph values =================================================
//v Thetas curves ================================================
static float thetasCurvesThickness = 2.0f;
static Color thetasCurvesColor = RED;
//^ Thetas curves ================================================

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------

static void Init(void);

static void UpdateDrawFrame(void);          // Update and draw one frame
static void Update(void);
static void DrawUI(void);
static void Draw(void);

static const vector<vector<float>> RetrieveCsvFileData(string filePathP);
static float Hypothesis(const float xP, const float theta_0P, const float theta_1P);
static float Cost(const vector<float>& xP, const vector<float>& yP, const float theta_0P, const float theta_1P);
static void GradientDescent(float& theta_0P, float& theta_1P, const float alphaP, const vector<float>& xP, const vector<float>& yP);
static void FindBestThetas(float& theta_0P, float& theta_1P, const float alphaP, const float diffThresholdP, const vector<float>& xP, const vector<float>& yP);

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
    const vector<vector<float>> retrievedData = RetrieveCsvFileData("Resources/train.csv");

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
    float firstHypothesis = Hypothesis(xTest, theta_0, theta_1);
    cout << "Before using gradient descent, we obtain " << firstHypothesis << " for x = " << xTest << endl;
    cout << "Expected value: " << yExpected << endl;
    cout << "\n\n" << endl;
    //^ Prediction test, before gradient descent =====================

    // Train algorithm
    FindBestThetas(theta_0, theta_1, 0.0001f, 0.00001f, xVal, yVal);

    cout << "\n" << endl;

    //v Prediction test, after gradient descent ======================
    float secondHypothesis = Hypothesis(xTest, theta_0, theta_1);
    cout << "After using gradient descent, we obtain " << secondHypothesis << " for x = " << xTest << endl;
    cout << "Expected value: " << yExpected << endl;

    cout << "\nFinal theta_0: " << theta_0 << endl;
    cout << "Final theta_1: " << theta_1 << endl;
    //^ Prediction test, after gradient descent ======================
}

/*
* Retrieve values from the given file and turn it into float vectors
*
* @param filePath: relative or absolute path to the file location
*/
const vector<vector<float>> RetrieveCsvFileData(string filePathP) {
    ifstream file{ filePathP, ifstream::in };

    string line, val;
    vector<string> row;

    vector<float> xVal;
    vector<float> yVal;

    if (file.is_open()) {
        // Skip first line
        getline(file, line);

        // Read line
        while (getline(file, line)) {
            row.clear();

            stringstream str{ line };

            // By line, create characters array 
            // and stop when facing a ","
            while (getline(str, val, ','))
            {
                row.push_back(val);
            }

            xVal.push_back(std::stof(row[0]));
            yVal.push_back(std::stof(row[1]));
        }
    }

    return vector<vector<float>> { xVal, yVal };
}

/**
* Predict y value
*
* @param x: absciss variable
* @param theta_0: where the prediction function intercepts the ordinate
* @param theta_1: prediction function slope
*
*  @return Ordinate prediction
*/
float Hypothesis(const float xP, const float theta_0P, const float theta_1P) {

    return theta_0P + theta_1P * xP;
}

/**
* Measure the total error we make comparing predicted and actual values on the dataset
*
* @param x: inputs
* @param y: real target value
* @param theta_0: where the prediction function intercepts the ordinate
* @param theta_1: prediction function slope
*
* @return Total error count
*/
float Cost(const vector<float>& xP, const vector<float>& yP, const float theta_0P, const float theta_1P) {
    // Handle datasets size difference
    if (xP.size() != yP.size()) {
        cout << "x and y should have the same size" << endl;

        return -1;
    }

    uint32_t dataSize = xP.size();
    float errorSum{ 0.0f };

    // Calculate error for each data point
    for (uint32_t i = 0; i < dataSize; i++)
    {
        float predictedValue = Hypothesis(xP[i], theta_0P, theta_1P);

        float squaredDiff = pow(predictedValue - yP[i], 2.0f);

        errorSum += squaredDiff;
    }

    float averageError = (1.0f / (2.0f * dataSize)) * errorSum;

    return averageError;
}

/**
* Using gradient descent algorithm, find the best suiting thetas for the dataset
*
* @param theta_0: ordinate interception
* @param theta_1: slope
* @param alpha: learning rate
* @param x: inputs
* @param y: actual outputs
*
* @return improved thetas
*/
void GradientDescent(float& theta_0P, float& theta_1P, const float alphaP, const vector<float>& xP, const vector<float>& yP) {
    uint32_t dataSize = xP.size();
    float errorSum{ 0.0f };

    // Find new theta_0 ===============================================
    for (uint32_t i = 0; i < dataSize; i++)
    {
        const float hyp = Hypothesis(xP[i], theta_0P, theta_1P);
        const float errorDiff = hyp - yP[i];

        errorSum += errorDiff;
    }

    float tempTheta_0 = (alphaP / dataSize) * errorSum;

    errorSum = 0.f;
    // ================================================================
    // Find new theta_1 ===============================================
    for (uint32_t i = 0; i < dataSize; i++)
    {
        const float hyp = Hypothesis(xP[i], theta_0P, theta_1P);
        const float errorDiff = (hyp - yP[i]) * xP[i];

        errorSum += errorDiff;
    }

    float tempTheta_1 = (alphaP / dataSize) * errorSum;
    // ================================================================

    theta_0P -= tempTheta_0;
    theta_1P -= tempTheta_1;
}

void FindBestThetas(float& theta_0P, float& theta_1P, const float alphaP, const float diffThresholdP, const vector<float>& xP, const vector<float>& yP) {
    // ================================================================
    // Handle datasets size difference ================================
    if (xP.size() != yP.size()) {
        cout << "x and y should have the same size" << endl;
    }
    // ================================================================
    // Initialisations ================================================

    uint32_t iterations{ 0 };

    float errorSum{ 0.0f };
    uint32_t dataSize = xP.size();

    float diff = Cost(xP, yP, theta_0P, theta_1P);
    cout << "Initial cost: " << diff << endl;

    while (diff >= diffThresholdP)
    {
        float initialDiff = Cost(xP, yP, theta_0P, theta_1P);

        GradientDescent(theta_0P, theta_1P, alphaP, xP, yP);

        float newDiff = Cost(xP, yP, theta_0P, theta_1P);

        diff = initialDiff - newDiff;

        iterations++;

        thetasSets.emplace_back(Thetas{theta_0P, theta_1P});

        cout << "Iteration #" << iterations << " =======" << endl;
        cout << "Difference: " << diff << endl;
        cout << "Theta_0: " << theta_0P << endl;
        cout << "Theta_1: " << theta_1P << endl;
    }

    cout << "\nGradient descent algorithm done in " << iterations << " iterations" << endl;
}

// Update and draw game frame
static void UpdateDrawFrame(void)
{
    Update();

    //----------------------------------------------------------------------------------
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(RAYWHITE);
    //DrawFPS(10, 10);

    Draw();
    DrawUI();

    EndDrawing();
    //----------------------------------------------------------------------------------
}

static void Update(void) {

}

static void Draw(void) {

}

static void DrawGraph(float graphWidth, float graphHeight, Vector2 graphOrigin) {
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

static void DrawUI(void) {
    float graphWidth = screenWidth - 2 * graphOffset;
    float graphHeight = screenHeight - 2 * graphOffset;

    Vector2 graphOrigin{ graphOffset, screenHeight - graphOffset };

    DrawGraph(graphWidth, graphHeight, graphOrigin);

    //v ==============================================================
    //v Draw data points =============================================

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

    //^ Draw data points =============================================
    //^ ==============================================================
    //v ============================================================== 
    //v Draw thetas curves ===========================================
    for (uint32_t i = 0; i < thetasSets.size(); i++)
    {
        const float yHyp = Hypothesis(xMaxValue, thetasSets[i].zero, thetasSets[i].one);
        const float yHypNorm = yHyp * graphHeight / yMaxValue;

        DrawLineEx(
            Vector2{ graphOrigin.x, graphOrigin.y - thetasSets[i].zero },
            Vector2{ graphOrigin.x + graphWidth, graphOrigin.y - yHypNorm },
            thetasCurvesThickness,
            thetasCurvesColor
        );
    }

    //^ Draw thetas curves ===========================================
    //^ ==============================================================
    float finalTextSize = 20.0f;
    uint32_t thetasSetsSize = thetasSets.size();
    Thetas finalThetas{ thetasSets[thetasSetsSize - 1].zero, thetasSets[thetasSetsSize - 1].one };

    DrawText("Final thetas:", 50.f, 5.f, finalTextSize, BLACK);
    DrawText(TextFormat("Theta0: %05f", finalThetas.zero), 80.f, 25.0f, finalTextSize - 2.5f, RED);
    DrawText(TextFormat("Theta1: %05f", finalThetas.one), 80.f, 40.0f, finalTextSize - 2.5f, RED);
}