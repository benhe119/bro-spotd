//
// Created by asr on 27/06/18.
//

#include <utility>
#include "MonitoredFeature.h"

static int req_id = 0;

MonitoredFeature::MonitoredFeature(string name, int type, SpotConfig config) : _spot(config),
                                                                               _name(std::move(name)),
                                                                               _bro_type(type)
{
}

bool MonitoredFeature::process(double data, BroEvent *out_event)
{
    int spot_result = this->_spot.step(data);

    if (abs(spot_result) == 1)
    {
        // anomalous
        BroEvent *ev;
        BroString feature_name;
        double p;

        // creating the event
        if (!(ev = bro_event_new("spot_anomaly")))
        {
            throw std::runtime_error("Could not allocate new event");
        }

        // id
        bro_event_add_val(ev, BRO_TYPE_INT, nullptr, &req_id);
        req_id++;

        // feature name
        bro_string_set(&feature_name, this->_name.c_str());
        bro_event_add_val(ev, BRO_TYPE_STRING, nullptr, &feature_name);
        bro_string_cleanup(&feature_name);

        // proba
        if (spot_result == 1)
        {
            p = this->_spot.up_probability(data);
        }
        else
        {
            p = this->_spot.down_probability(data);
        }
        bro_event_add_val(ev, BRO_TYPE_DOUBLE, nullptr, &p);
        
        out_event = ev;
        return true;
    }

    return false;
}

SpotStatus MonitoredFeature::status()
{
    return this->_spot.status();
}

SpotConfig MonitoredFeature::config()
{
    return this->_spot.config();
}