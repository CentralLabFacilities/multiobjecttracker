/*
 * BayesTrackingModel.h
 *
 *  Created on: Sep 7, 2015
 *      Author: lziegler
 */

#pragma once

#pragma GCC diagnostic ignored "-Wnarrowing"

#include "../TrackerConfig.h"
#include "../util/Utilities.h"
#include "Model.h"

#include <bayes_tracking/multitracker.h>
#include <bayes_tracking/models.h>
#include <bayes_tracking/ekfilter.h>
#include <bayes_tracking/ukfilter.h>
#include <bayes_tracking/pfilter.h>
#include <bayes_tracking/trackwin.h>
#include <cstdio>
#include <sys/time.h>

#include <rsc/logging/Logger.h>

// rule to detect lost track
template<class FilterType>
bool MTRK::isLost(const FilterType* filter) {
    // track lost if var(x)+var(y) > 1
    if (filter->X(0, 0) + filter->X(2, 2) > Models::sqr(1.0))
        return true;
    return false;
}

// rule to create new track
template<class FilterType>
bool MTRK::initialize(FilterType* &filter, sequence_t& obsvSeq) {
    assert(obsvSeq.size());

    double dt = obsvSeq.back().time - obsvSeq.front().time;
    assert(dt); // dt must not be null
    FM::Vec v((obsvSeq.back().vec - obsvSeq.front().vec) / dt);

    FM::Vec x(4);
    FM::SymMatrix X(4, 4);

    x[0] = obsvSeq.back().vec[0];
    x[1] = v[0];
    x[2] = obsvSeq.back().vec[1];
    x[3] = v[1];
    X.clear();
    X(0, 0) = Models::sqr(0.5);
    X(1, 1) = Models::sqr(1.5);
    X(2, 2) = Models::sqr(0.5);
    X(3, 3) = Models::sqr(1.5);

    filter = new FilterType(4);
    filter->init(x, X);

    return true;
}

namespace objecttracker {

template<typename FilterType, int Dimensions, class VelocityModelType,
        typename ObservationModelType>
class BayesTrackingModel: public Model {
protected:
    MTRK::MultiTracker<FilterType, Dimensions> mtrk;    // state [x, v_x, y, v_y]
    /*
     * Constant Velocity Model
     */
    VelocityModelType velocityModel;

    /*
     * Observation Velocity Model
     */
    ObservationModelType observationModel;
    MTRK::association_t associationAlg;

    double time;
    int dimensionObservation;

    const rsc::logging::LoggerPtr logger;

public:
    BayesTrackingModel(int dimensionObservation, TrackerConfig config,
            const VelocityModelType &velModel, const ObservationModelType &obsModel) :
            time(Utilities::getTimeSeconds()),
            velocityModel(velModel),
            observationModel(obsModel),
            dimensionObservation(dimensionObservation),
            logger(rsc::logging::Logger::getLogger("BayesTrackingModel")){

        if (config.association_algorithm == "NNJPDA") {
            associationAlg = MTRK::NNJPDA;
        } else if(config.association_algorithm == "NN") {
            associationAlg = MTRK::NN;
        } else if(config.association_algorithm == "") {
            associationAlg = MTRK::NNJPDA;
            RSCWARN(logger, "No association algorithm given. Fallback to NNJPDA.");
        }  else {
            stringstream ss;
            ss << "Unknown association algorithm \"" << config.association_algorithm << "\"";
            RSCERROR(logger, ss.str());
            throw ModelException(ss.str());
        }
    }
    virtual ~BayesTrackingModel() {
    }

    void predict() {
        double dtime = Utilities::getTimeSeconds() - time;
        time += dtime;

        velocityModel.update(dtime);
        mtrk.predict(velocityModel);
    }

    void addObservation(const std::vector<float> &observation) {

        if (observation.size() != dimensionObservation) {
            throw ModelException("Observation dimension does not match configured dimensions.");
        }

        FM::Vec obsv(dimensionObservation);
        for (int i = 0; i < observation.size(); i++) {
            obsv[i] = observation[i];
        }
        mtrk.addObservation(obsv, time);
    }

    void process(std::vector<Track> &tracksOut, std::map<int, int> &obsvMappingOut) {
        mtrk.process(observationModel, associationAlg);
        obsvMappingOut = mtrk.assignments();
        for (int i = 0; i < mtrk.size(); i++) {
            Track t;
            t.id = mtrk[i].id;
            t.states.resize(mtrk[i].filter->x.size());
            for (int j = 0; j < mtrk[i].filter->x.size(); j++) {
                t.states[j] = mtrk[i].filter->x[j];
            }
            tracksOut.push_back(t);
        }
    }
};

} /* namespace objecttracker */
