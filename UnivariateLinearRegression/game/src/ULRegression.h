#pragma once

#include <vector>
#include <string>

struct Thetas
{
    float zero = 0.0f;
    float one = 0.0f;
};

class ULRegression
{
public:
    static const std::vector<std::vector<float>> RetrieveCsvFileData(std::string filePathP);
    static float Hypothesis(const float xP, const float theta_0P, const float theta_1P);
    static float Cost(const std::vector<float>& xP, const std::vector<float>& yP, const float theta_0P, const float theta_1P);
    static void GradientDescent(float& theta_0P, float& theta_1P, const float alphaP, const std::vector<float>& xP, const std::vector<float>& yP);
    static const std::vector<Thetas> FindBestThetas(float& theta_0P, float& theta_1P, const float alphaP, const float diffThresholdP, const std::vector<float>& xP, const std::vector<float>& yP);

private:
    static std::vector<Thetas> thetasSets;
};

