#include "normal_spikegen2.h"
#include <random>
#include <cmath>
#include <iostream>

NormalSpikeGenerator2::NormalSpikeGenerator2(float mean, float sd, int coefficient, bool write_to_file)
    : mean_(mean), sd_(sd), coefficient_(coefficient) {
    spikeTimesPerNeuron_.resize(coefficient_);
    generateSpikeTimes();
}

void NormalSpikeGenerator2::generateSpikeTimes() {
    const float pi = 3.14159265f;
    const int duration = 1000; // simulation length in ms
    const float coeff = coefficient_ / (sd_ * std::sqrt(2 * pi));

    for (int t = 0; t < duration; ++t) {
        float exponent = -0.5f * std::pow((t - mean_) / sd_, 2);
        float probability = coeff * std::exp(exponent);
        for (int nid = 1; nid < probability; ++nid)
            spikeTimesPerNeuron_[nid].push(t);
     }
}

int NormalSpikeGenerator2::nextSpikeTime(CARLsim* sim, int grpId, int nid, int currentTime, int lastScheduledSpikeTime, int endOfTimeSlice) {
    if (nid >= spikeTimesPerNeuron_.size() || spikeTimesPerNeuron_[nid].empty()) return -1;
    while (!spikeTimesPerNeuron_[nid].empty()) {
        int next = spikeTimesPerNeuron_[nid].top();
        if (next > lastScheduledSpikeTime && next <= endOfTimeSlice) {
            spikeTimesPerNeuron_[nid].pop();
            return next;
        }
        spikeTimesPerNeuron_[nid].pop();
    }
    return -1;
}
