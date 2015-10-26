/*
 * Model.h
 *
 *  Created on: Sep 7, 2015
 *      Author: lziegler
 */

#pragma once

#include <exception>
#include <vector>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

namespace objecttracker {

class Track {
public:
    std::vector<float> states;
    int id;
};

class Model {
public:
    typedef boost::shared_ptr<Model> Ptr;
    virtual void predict() = 0;
    virtual void addObservation(const std::vector<float> &observation) = 0;
    virtual void process(std::vector<Track> &tracksOut, std::map<int, int> &obsvMappingOut) = 0;
    virtual ~Model() {
    }
};

class ModelException: std::exception {
private:
    std::string msg;
public:
    ModelException(const std::string &msg) :
            msg(msg) {
    }
    virtual ~ModelException() throw() {
    }
    virtual const char* what() const throw() {
        return msg.c_str();
    }
};
}
