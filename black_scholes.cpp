#include <iostream>
#include <cmath>
#include <cstdlib>

// Cumulative distribution function for the standard normal distribution
double normCDF(double x) {
    return 0.5 * std::erfc(-x * M_SQRT1_2);
}

// Black-Scholes formula for call option pricing
double blackScholesCall(double S, double K, double T, double r, double sigma) {
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    double normCDF_d1 = normCDF(d1);
    double normCDF_d2 = normCDF(d2);
    return S * normCDF_d1 - K * std::exp(-r * T) * normCDF_d2;
}

// Greeks calculations
double delta(double S, double K, double T, double r, double sigma) {
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    return normCDF(d1);
}

double gamma(double S, double K, double T, double r, double sigma) {
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    return std::exp(-0.5 * d1 * d1) / (S * sigma * std::sqrt(T * 2 * M_PI));
}

double theta(double S, double K, double T, double r, double sigma) {
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    double term1 = -S * std::exp(-0.5 * d1 * d1) * sigma / (2 * std::sqrt(2 * M_PI * T));
    double term2 = r * K * std::exp(-r * T) * normCDF(d2);
    return term1 - term2;
}

double vega(double S, double K, double T, double r, double sigma) {
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    return S * std::exp(-0.5 * d1 * d1) / std::sqrt(2 * M_PI);
}

double rho(double S, double K, double T, double r, double sigma) {
    double d2 = (std::log(S / K) + (r - 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    return K * T * std::exp(-r * T) * normCDF(d2);
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " <stock_price> <strike_price> <time_to_maturity> <risk_free_rate> <volatility>" << std::endl;
        return 1;
    }

    double S = std::atof(argv[1]);   // Stock Price
    double K = std::atof(argv[2]);   // Strike Price
    double T = std::atof(argv[3]);   // Time to Maturity in years
    double r = std::atof(argv[4]);   // Risk-Free Rate (as a decimal)
    double sigma = std::atof(argv[5]); // Volatility (as a decimal)

    double callPrice = blackScholesCall(S, K, T, r, sigma);
    double deltaValue = delta(S, K, T, r, sigma);
    double gammaValue = gamma(S, K, T, r, sigma);
    double thetaValue = theta(S, K, T, r, sigma);
    double vegaValue = vega(S, K, T, r, sigma);
    double rhoValue = rho(S, K, T, r, sigma);

    std::cout << "Call Option Price: $" << callPrice << std::endl;
    std::cout << "Delta: " << deltaValue << std::endl;
    std::cout << "Gamma: " << gammaValue << std::endl;
    std::cout << "Theta: " << thetaValue << std::endl;
    std::cout << "Vega: " << vegaValue << std::endl;
    std::cout << "Rho: " << rhoValue << std::endl;

    return 0;
}
