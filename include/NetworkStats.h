//
// Created by asr on 27/06/18.
//

#ifndef BRO_SPOTD_NETWORKSTATS_H
#define BRO_SPOTD_NETWORKSTATS_H

#include "broccoli.h"
#include <string>
#include <sstream>

class NetworkStats {
public:
    double t;
    double ratio;
    uint64 nb_srcport;
    uint64 nb_dstport;
    double avg_pkt_size;
    double ratio_syn_pkt;
    double ratio_icmp_pkt;

    NetworkStats(BroRecord *record);

    string to_str();
};

#endif //BRO_SPOTD_NETWORKSTATS_H
