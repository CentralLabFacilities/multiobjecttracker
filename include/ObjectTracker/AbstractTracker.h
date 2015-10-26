/*
 * AbstractTracker.h
 *
 *  Created on: Sep 3, 2015
 *      Author: leon
 */

#pragma once

#include "TrackerConfig.h"
#include "util/Utilities.h"
#include "model/BayesTrackingModel.h"

#include <rsc/logging/Logger.h>

namespace objecttracker {

typedef std::vector<float> Observation;
typedef std::vector<Observation> Observations;

class AbstractTracker {
protected:
    TrackerConfig config;
public:
    typedef boost::shared_ptr<AbstractTracker> Ptr;
    AbstractTracker(TrackerConfig config) :
            config(config) {
    }
    virtual ~AbstractTracker() {
    }
    TrackerConfig getConfiguration() {
        return config;
    }
};

template<int Dimensions>
class AbstractTrackerImpl: public AbstractTracker {
protected:
    Model::Ptr model;
    const rsc::logging::LoggerPtr logger;
public:
    typedef boost::shared_ptr<AbstractTrackerImpl<Dimensions> > Ptr;
    AbstractTrackerImpl(int dimensionObservation, TrackerConfig config) :
            AbstractTracker(config),
            logger(rsc::logging::Logger::getLogger("AbstractTrackerImpl")) {
        model = createModel(dimensionObservation);
    }
    virtual ~AbstractTrackerImpl() {
    }

    Model::Ptr createModel(int dimensionObservation) {
        if (config.filter_type == "UKFilter") {
            return createModelDims<UKFilter>(dimensionObservation);
        } else if (config.filter_type == "EKFilter") {
            return createModelDims<EKFilter>(dimensionObservation);
        } else if (config.filter_type == "") {
            RSCWARN(logger, "No filter type given. Fallback to UKFilter.");
            return createModelDims<UKFilter>(dimensionObservation);
        } else {
            stringstream ss;
            ss << "unknown filter type: " << config.filter_type;
            RSCERROR(logger, ss.str());
            throw ModelException(ss.str());
        }
    }

    template<typename FilterType>
    Model::Ptr createModelDims(int dimensionObservation) {
        if (dimensionObservation == 2) {
            checkDims(dimensionObservation, config.sigma, "sigma", config.name);
            checkDims(dimensionObservation, config.sd, "sd", config.name);
            Models::CVModel cvModel(config.sigma[0], config.sigma[1]);
            Models::CartesianModel cartModel(config.sd[0], config.sd[1]);
            return Model::Ptr(
                    new BayesTrackingModel<FilterType, Dimensions, Models::CVModel,
                            Models::CartesianModel>(dimensionObservation, config, cvModel, cartModel));
        } else if (dimensionObservation == 3) {
            checkDims(dimensionObservation, config.sigma, "sigma", config.name);
            checkDims(dimensionObservation, config.sd, "sd", config.name);
            Models::CVModel3D cvModel(config.sigma[0], config.sigma[1], config.sigma[2]);
            Models::CartesianModel3D cartModel(config.sd[0], config.sd[1], config.sd[2]);
            return Model::Ptr(
                    new BayesTrackingModel<FilterType, Dimensions, Models::CVModel3D,
                            Models::CartesianModel3D>(dimensionObservation, config, cvModel, cartModel));
        } else {
            stringstream ss;
            ss << "unsupported observation dimension: " << dimensionObservation;
            throw ModelException(ss.str());
        }
    }

    void process(const Observations &observations, std::vector<Track> &tracksOut, std::map<int, int> &obsvMappingOut) {
        model->predict();
        std::vector<std::vector<float> >::const_iterator it;
        for (it = observations.begin(); it != observations.end(); ++it) {
            model->addObservation(*it);
        }
        model->process(tracksOut, obsvMappingOut);
    }

    template<typename Type>
    void checkDims(int dims, const std::vector<Type> &vec, const std::string &param, const std::string &trackerName) {
        if (vec.size() < dims) {
            stringstream ss;
            ss << "For tracker " << trackerName << " parameter \"" << param << "\" has to be " << dims << "-dimensional, but has " << vec.size() << " dimensions.";
            throw ModelException(ss.str());
        }
    }
};

class AbstractTrackerBuilder {
public:
    typedef boost::shared_ptr<AbstractTrackerBuilder> Ptr;
    AbstractTrackerBuilder(const std::string &trackerName): trackerName(trackerName) {
    }
    virtual std::string getTrackerName() const {
        return trackerName;
    }
    virtual AbstractTracker::Ptr build(TrackerConfig config) const = 0;
    virtual ~AbstractTrackerBuilder() {
    }
protected:
    std::string trackerName;
};

#define CREATE_BUILDER_NESTED(TRACKER_NAME) class Builder: public AbstractTrackerBuilder {\
public:\
    Builder(const std::string &trackerName) :\
        AbstractTrackerBuilder(trackerName) {\
    }\
    virtual AbstractTracker::Ptr build(TrackerConfig config) const {\
        return AbstractTracker::Ptr(new TRACKER_NAME(config));\
    }\
};

} /* namespace objecttracker */
