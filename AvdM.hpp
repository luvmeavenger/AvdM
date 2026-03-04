/*
 * AvdM.hpp - High-Performance Math & Physics Header
 * Author: AvdM Development
 * License: MIT
 * -------------------------------------------------------------------------
 * A master-tier C++ library covering ML, Signal Processing, Calculus, 
 * and Physics. Built for developers who value their time.
 */

#ifndef AVDM_MATH_HPP
#define AVDM_MATH_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <complex>
#include <valarray>
#include <functional>

// --- AVDM TERMINAL STYLING (For the Debugging Master) ---
#define AVDM_RESET   "\033[0m"
#define AVDM_RED     "\033[31m"
#define AVDM_GREEN   "\033[32m"
#define AVDM_YELLOW  "\033[33m"
#define AVDM_CYAN    "\033[36m"
#define AVDM_BOLD    "\033[1m"

enum class AvdM_Status {
    OK = 0,
    DIVIDE_BY_ZERO = 101,
    DATA_EMPTY = 102,
    PARSE_ERR = 103,
    DIMENSION_MISMATCH = 104
};

#define AVDM_LOG_ERR(code, msg) \
    std::cerr << AVDM_RED << AVDM_BOLD << "[AvdM ERROR " << (int)code << "] " << AVDM_RESET \
              << AVDM_CYAN << __FUNCTION__ << "(): " << AVDM_RESET \
              << msg << AVDM_YELLOW << " [Line: " << __LINE__ << "]" << AVDM_RESET << std::endl

namespace AvdM {

    // --- MACHINE LEARNING (K-Nearest Neighbors) ---
    struct DataPoint {
        std::vector<double> features;
        int label;
    };

    inline double euclideanDistance(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) {
            AVDM_LOG_ERR(AvdM_Status::DIMENSION_MISMATCH, "Feature vector sizes do not match.");
            return -1.0;
        }
        double sum = 0;
        for (size_t i = 0; i < a.size(); ++i) sum += std::pow(a[i] - b[i], 2);
        return std::sqrt(sum);
    }

    inline int predictKNN(const std::vector<DataPoint>& dataset, const std::vector<double>& target, int k) {
        if (dataset.empty()) {
            AVDM_LOG_ERR(AvdM_Status::DATA_EMPTY, "Training set is empty.");
            return -1;
        }
        std::vector<std::pair<double, int>> dists;
        for (const auto& p : dataset) dists.push_back({euclideanDistance(p.features, target), p.label});
        std::sort(dists.begin(), dists.end());

        std::map<int, int> votes;
        for (int i = 0; i < std::min(k, (int)dists.size()); ++i) votes[dists[i].second]++;
        
        int best = -1, maxV = -1;
        for (auto const& [label, count] : votes) {
            if (count > maxV) { maxV = count; best = label; }
        }
        return best;
    }

    // --- STRING EXPRESSION PARSER ---
    inline double evaluate(std::string expr) {
        std::stringstream ss(expr);
        double res, val;
        char op;
        if (!(ss >> res)) {
            AVDM_LOG_ERR(AvdM_Status::PARSE_ERR, "Invalid start of expression.");
            return 0;
        }
        while (ss >> op >> val) {
            if (op == '+') res += val;
            else if (op == '-') res -= val;
            else if (op == '*') res *= val;
            else if (op == '/') {
                if (val == 0) { AVDM_LOG_ERR(AvdM_Status::DIVIDE_BY_ZERO, "Attempted 1/0."); return 0; }
                res /= val;
            }
        }
        return res;
    }

    // --- SIGNAL PROCESSING (FFT) ---
    using Complex = std::complex<double>;
    using CArray  = std::valarray<Complex>;

    inline void fft(CArray& x) {
        const size_t N = x.size();
        if (N <= 1) return;
        CArray even = x[std::slice(0, N/2, 2)];
        CArray odd  = x[std::slice(1, N/2, 2)];
        fft(even); fft(odd);
        for (size_t k = 0; k < N/2; ++k) {
            Complex t = std::polar(1.0, -2 * M_PI * k / N) * odd[k];
            x[k] = even[k] + t;
            x[k+N/2] = even[k] - t;
        }
    }

    // --- PHYSICS (Projectile Dynamics) ---
    struct State { double x, y, vx, vy; };
    inline State computeStep(State s, double dt, double drag = 0.02) {
        double vel = std::sqrt(s.vx*s.vx + s.vy*s.vy);
        double ax = -(drag * vel * s.vx);
        double ay = -9.81 - (drag * vel * s.vy); // Earth g
        return { s.x + s.vx*dt, s.y + s.vy*dt, s.vx + ax*dt, s.vy + ay*dt };
    }

} // namespace AvdM

#endif
