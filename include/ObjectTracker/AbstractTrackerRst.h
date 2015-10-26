/*
 * AbstractTrackerRst.h
 *
 *  Created on: Sep 3, 2015
 *      Author: leon
 */

#pragma once

#include "AbstractTracker.h"

#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#include <rsb/Listener.h>
#include <rsb/Informer.h>
#include <rsb/Factory.h>
#include <rsb/converter/ProtocolBufferConverter.h>
#include <rsb/converter/Repository.h>

#include <rsc/logging/Logger.h>

namespace objecttracker {

class AbstractCommunicator {

};

template<class RstType, int Dimensions>
class AbstractTrackerRst: public AbstractTrackerImpl<Dimensions> {
private:
    rsb::ListenerPtr listener;
    typename rsb::Informer<RstType>::Ptr informer;
    rsc::logging::LoggerPtr logger;
public:
    typedef RstType DataType;
    typedef boost::shared_ptr<RstType> DataPtr;
    AbstractTrackerRst(int observationDimension, TrackerConfig config) :
        AbstractTrackerImpl<Dimensions>(observationDimension, config), logger(rsc::logging::Logger::getLogger("AbstractTrackerRst")) {

        rsb::Factory &factory = rsb::getFactory();

        try {
            boost::shared_ptr<rsb::converter::ProtocolBufferConverter<RstType> > converter(
                    new rsb::converter::ProtocolBufferConverter<RstType>());

            rsb::converter::converterRepository<std::string>()->registerConverter(converter);
        } catch (std::invalid_argument &e) {
            RSCWARN(logger, "Warning: trying to register two converters for same type");
        }

        informer = factory.createInformer<RstType>(config.scope_out);
        listener = factory.createListener(config.scope_in);

        boost::function<void(DataPtr)> cb(
                boost::bind(&AbstractTrackerRst<RstType, Dimensions>::callback, this, _1));
        listener->addHandler(rsb::HandlerPtr(new rsb::DataFunctionHandler<RstType>(cb)));
    }
    virtual ~AbstractTrackerRst() {

    }

    void callback(DataPtr data) {

        Observations obs = makeObservations(data);
        std::vector<Track> tracks;
        std::map<int, int> mapping;

        this->process(obs, tracks, mapping);
        DataPtr result = makeResults(data, tracks, mapping);
        informer->publish(result);
    }

    virtual Observations makeObservations(DataPtr data) = 0;
    virtual DataPtr makeResults(DataPtr data, const std::vector<Track> &tracks, const std::map<int, int> obsvMappingOut) = 0;
};

} /* namespace objecttracker */
