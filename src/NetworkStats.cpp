//
// Created by asr on 27/06/18.
//

#include "NetworkStats.h"


NetworkStats::NetworkStats(BroRecord *record) {

    int type_double = BRO_TYPE_DOUBLE;
    int type_count = BRO_TYPE_COUNT;
    int type_time = BRO_TYPE_TIME;

    this->t = *(double *) bro_record_get_named_val(record, "t", &type_time);
    this->ratio = *(double *) bro_record_get_named_val(record, "ratio", &type_double);
    this->nb_srcport = *(uint64 *) bro_record_get_named_val(record, "nb_srcport", &type_count);
    this->nb_dstport = *(uint64 *) bro_record_get_named_val(record, "nb_dstport", &type_count);
    this->avg_pkt_size = *(double *) bro_record_get_named_val(record, "avg_pkt_size", &type_double);
    this->ratio_syn_pkt = *(double *) bro_record_get_named_val(record, "ratio_syn_pkt",
                                                               &type_double);
    this->ratio_icmp_pkt = *(double *) bro_record_get_named_val(record, "ratio_icmp_pkt",
                                                                &type_double);
}


string NetworkStats::to_str() {
    stringstream ss;
    ss.precision(6);
    ss << this->t << "," << this->ratio << "," << this->nb_srcport << "," << this->nb_dstport << ",";
    ss << this->avg_pkt_size << "," << this->ratio_syn_pkt << "," << this->ratio_icmp_pkt << endl;
    return ss.str();
}