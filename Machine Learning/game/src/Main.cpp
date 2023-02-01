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
static Datas datas;

//v Graph values =================================================
static float graphOffset = 40.0f;
static float axisThickness = 4.0f;
static uint8_t marksNumber = 15;

static float pointRadius = 2.0f;
static float xMaxValue = 110.0f;
static float yMaxValue = 110.0f;

//^ Graph values =================================================

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------

static void Init(void);

static void UpdateDrawFrame(void);          // Update and draw one frame
static void Update(void);
static void DrawUI(void);
static void Draw(void);

static const vector<vector<float>> retrieveCsvFileData(string filePathP);
static float hypothesis(const float xP, const float theta_0P, const float theta_1P);
static float cost(const vector<float>& xP, const vector<float>& yP, const float theta_0P, const float theta_1P);
static void gradientDescent(float& theta_0P, float& theta_1P, const float alphaP, const vector<float>& xP, const vector<float>& yP);
static void findBestThetas(float& theta_0P, float& theta_1P, const float alphaP, const float diffThresholdP, const vector<float>& xP, const vector<float>& yP);

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
    const vector<vector<float>> retrievedData = retrieveCsvFileData("Resources/test.csv");

    //const vector<float> xVal{ retrievedData[0] };
    //const vector<float> yVal{ retrievedData[1] };

    datas.x = retrievedData[0];
    datas.y = retrievedData[1];
    datas.size = datas.x.size();

    const vector<float> xVal{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f };
    const vector<float> yVal{ 0.25f, 0.50f, 0.75f, 1.0f, 1.25f, 1.5f, 1.75f, 2.0f, 2.25f, 2.50f };

    float theta_0{ 0.0f };
    float theta_1{ 0.0f };

    float xTest = xVal[2];
    float yExpected = yVal[2];

    // Prediction test, before gradient descent
    float firstHypothesis = hypothesis(xTest, theta_0, theta_1);
    cout << "Before using gradient descent, we obtain " << firstHypothesis << " for x = " << xTest << endl;
    cout << "Expected value: " << yExpected << endl;
    cout << "\n\n" << endl;

    findBestThetas(theta_0, theta_1, 0.001f, 0.0001f, xVal, yVal);

    cout << "\n" << endl;
    // Prediction test, after gradient descent
    float secondHypothesis = hypothesis(xTest, theta_0, theta_1);
    cout << "After using gradient descent, we obtain " << secondHypothesis << " for x = " << xTest << endl;
    cout << "Expected value: " << yExpected << endl;

    cout << "\nFinal theta_0: " << theta_0 << endl;
    cout << "Final theta_1: " << theta_1 << endl;
}

/*
* Retrieve values from the given file and turn it into float vectors
*
* @param filePath: relative or absolute path to the file location
*/
const vector<vector<float>> retrieveCsvFileData(string filePathP) {
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
float hypothesis(const float xP, const float theta_0P, const float theta_1P) {

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
float cost(const vector<float>& xP, const vector<float>& yP, const float theta_0P, const float theta_1P) {
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
        float predictedValue = hypothesis(xP[i], theta_0P, theta_1P);

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
void gradientDescent(float& theta_0P, float& theta_1P, const float alphaP, const vector<float>& xP, const vector<float>& yP) {
    uint32_t dataSize = xP.size();
    float errorSum{ 0.0f };

    // Find new theta_0 ===============================================
    for (uint32_t i = 0; i < dataSize; i++)
    {
        const float hyp = hypothesis(xP[i], theta_0P, theta_1P);
        const float errorDiff = hyp - yP[i];

        errorSum += errorDiff;
    }

    float tempTheta_0 = (alphaP / dataSize) * errorSum;

    errorSum = 0.f;
    // ================================================================
    // Find new theta_1 ===============================================
    for (uint32_t i = 0; i < dataSize; i++)
    {
        const float hyp = hypothesis(xP[i], theta_0P, theta_1P);
        const float errorDiff = (hyp - yP[i]) * xP[i];

        errorSum += errorDiff;
    }

    float tempTheta_1 = (alphaP / dataSize) * errorSum;
    // ================================================================

    theta_0P -= tempTheta_0;
    theta_1P -= tempTheta_1;
}

void findBestThetas(float& theta_0P, float& theta_1P, const float alphaP, const float diffThresholdP, const vector<float>& xP, const vector<float>& yP) {
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

    float diff = cost(xP, yP, theta_0P, theta_1P);
    cout << "Initial cost: " << diff << endl;

    while (diff >= diffThresholdP)
    {
        float initialDiff = cost(xP, yP, theta_0P, theta_1P);

        gradientDescent(theta_0P, theta_1P, alphaP, xP, yP);

        float newDiff = cost(xP, yP, theta_0P, theta_1P);

        diff = initialDiff - newDiff;

        iterations++;

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

static void DrawUI(void) {
    Vector2 graphOrigin{ graphOffset, screenHeight - graphOffset };
    float graphWidth = screenWidth - 2 * graphOffset;
    float graphHeight = screenHeight - 2 * graphOffset;

    //v ==============================================================
    //v Draw graph =================================================== 
    // Draw grid ============================
    // Draw absissa marks
    float xSpacing = graphWidth / marksNumber;
    for (uint8_t i = 1; i < marksNumber + 1; i++)
    {
        Vector2 startingPos{ graphOffset + xSpacing * i, graphOrigin.y };
        Vector2 endingPos{ startingPos.x, graphOffset };

        DrawLineEx(startingPos, endingPos, axisThickness / 2.0f, LIGHTGRAY);
    }
    // Draw ordonate marks
    float ySpacing = graphHeight / marksNumber;
    for (uint8_t i = 0; i < marksNumber; i++)
    {
        Vector2 startingPos{ graphOrigin.x, graphOffset + ySpacing*i };
        Vector2 endingPos{ screenWidth - graphOffset, startingPos.y };

        DrawLineEx(startingPos, endingPos, axisThickness / 2.0f, LIGHTGRAY);
    }

    // Draw abscissa ========================
    Vector2 absStartPos{ graphOrigin.x / 2.0f, graphOrigin.y };
    Vector2 absEndPos{ screenWidth - graphOffset, absStartPos.y };

    DrawLineEx(absStartPos, absEndPos, axisThickness, BLACK);

    // Draw ordonate ========================
    Vector2 ordStartPos{ graphOrigin.x, screenHeight - graphOffset / 2.0f };
    Vector2 ordEndPos{ ordStartPos.x, graphOffset };

    DrawLineEx(ordStartPos, ordEndPos, axisThickness, BLACK);

    //^ Draw graph ===================================================
    //^ ==============================================================
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
}