#include "ULRegression.h"

#include <iostream>

#include <fstream>
#include <sstream>

using std::vector;
using std::string;

using std::cout;
using std::endl;

using std::ifstream;
using std::stringstream;

//v Variables ====================================================
std::vector<Thetas> ULRegression::thetasSets;

//^ Variables ====================================================

/*
* Retrieve values from the given file and turn it into float vectors
*
* @param filePath: relative or absolute path to the file location
*/
const vector<vector<float>> ULRegression::RetrieveCsvFileData(string filePathP) {
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
float ULRegression::Hypothesis(const float xP, const float theta_0P, const float theta_1P) {

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
float ULRegression::Cost(const vector<float>& xP, const vector<float>& yP, const float theta_0P, const float theta_1P) {
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
void ULRegression::GradientDescent(float& theta_0P, float& theta_1P, const float alphaP, const vector<float>& xP, const vector<float>& yP) {
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

const vector<Thetas> ULRegression::RefineThetas(float& theta_0P, float& theta_1P, const float alphaP, const float diffThresholdP, const vector<float>& xP, const vector<float>& yP) {
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

        thetasSets.emplace_back(Thetas{ theta_0P, theta_1P });

        cout << "Iteration #" << iterations << " =======" << endl;
        cout << "Difference: " << diff << endl;
        cout << "Theta_0: " << theta_0P << endl;
        cout << "Theta_1: " << theta_1P << endl;
    }

    cout << "\nGradient descent algorithm done in " << iterations << " iterations" << endl;

    return thetasSets;
}

