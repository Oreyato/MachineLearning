#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

/**
* Predict y value
* 
* @param x: absciss variable
* @param theta_0: where the prediction function intercepts the ordinate
* @param theta_1: prediction function slope
* 
*  @return Ordinate prediction
*/
float hypothesis(const float xP, const float theta_0P = 0.0f, const float theta_1P = 0.0f) {

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
	// Handle size difference
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

	float averageError = errorSum / (2.0f * dataSize);

	return averageError;
}

int main(int argc, char* argv[])
{ 
	cout << hypothesis(5.0f, 2.0f, 2.f) << endl;

	vector<float> xVal = {
		1.0f, 2.0f, 3.0f, 4.0f, 5.0f
	};
	vector<float> yVal = {
		5.0f, 10.0f, 15.0f, 18.0f, 21.0f
	};

	cout << cost(xVal, yVal, 0.0f, 0.0f) << endl;
}