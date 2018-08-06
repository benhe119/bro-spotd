//
// Created by asr on 26/06/18.
//

#include "Dispotcher.h"

using namespace std;

#define INIT_PARSER_NOKEY (-1)

SpotConfig get_default_config(dictionary *ini)
{
    int default_exists = iniparser_find_entry(ini, "default");
    SpotConfig default_config;

    //cout << "Parsing default profile ... ";
    if (default_exists != 1)
    {
        //cout << FAIL << endl;
        //cout << "\t" << "Using backup procedure ";
    }

    double q = iniparser_getdouble(ini, "default:q", INIT_PARSER_NOKEY);
    if (q != INIT_PARSER_NOKEY)
    {
        default_config.q = q;
    }
    else
    {
        default_config.q = 1e-4;
    }

    int bounded = iniparser_getboolean(ini, "default:bounded", INIT_PARSER_NOKEY);
    if (bounded != INIT_PARSER_NOKEY)
    {
        default_config.bounded = (bounded == 1);
    }
    else
    {
        default_config.bounded = true;
    }

    int max_excess = iniparser_getint(ini, "default:max_excess", INIT_PARSER_NOKEY);
    if (max_excess != INIT_PARSER_NOKEY)
    {
        default_config.max_excess = max_excess;
    }
    else
    {
        default_config.max_excess = 200;
    }

    default_config.alert = true;

    int up = iniparser_getboolean(ini, "default:up", INIT_PARSER_NOKEY);
    if (up != INIT_PARSER_NOKEY)
    {
        default_config.up = (up == 1);
    }
    else
    {
        default_config.up = true;
    }

    int down = iniparser_getboolean(ini, "default:down", INIT_PARSER_NOKEY);
    if (down != INIT_PARSER_NOKEY)
    {
        default_config.down = (down == 1);
    }
    else
    {
        default_config.down = false;
    }

    int n_init = iniparser_getint(ini, "default:n_init", INIT_PARSER_NOKEY);
    if (n_init != INIT_PARSER_NOKEY)
    {
        default_config.n_init = n_init;
    }
    else
    {
        default_config.n_init = 1000;
    }

    double level = iniparser_getdouble(ini, "default:level", INIT_PARSER_NOKEY);
    if (level != INIT_PARSER_NOKEY)
    {
        default_config.level = level;
    }
    else
    {
        default_config.level = 0.99;
    }

    //cout << OK << endl;
    return default_config;
}

SpotConfig get_config(dictionary *ini, string feature, SpotConfig &default_config)
{
    SpotConfig config;

    double q = iniparser_getdouble(ini, (feature + ":q").c_str(), INIT_PARSER_NOKEY);
    if (q != INIT_PARSER_NOKEY)
    {
        config.q = q;
    }
    else
    {
        config.q = default_config.q;
    }

    int bounded = iniparser_getboolean(ini, (feature + ":bounded").c_str(), INIT_PARSER_NOKEY);
    if (bounded != INIT_PARSER_NOKEY)
    {
        config.bounded = (bounded == 1);
    }
    else
    {
        config.bounded = default_config.bounded;
    }

    int max_excess = iniparser_getint(ini, (feature + ":max_excess").c_str(), INIT_PARSER_NOKEY);
    if (max_excess != INIT_PARSER_NOKEY)
    {
        config.max_excess = max_excess;
    }
    else
    {
        config.max_excess = default_config.max_excess;
    }

    config.alert = true;

    int up = iniparser_getboolean(ini, (feature + ":up").c_str(), INIT_PARSER_NOKEY);
    if (up != INIT_PARSER_NOKEY)
    {
        config.up = (up == 1);
    }
    else
    {
        config.up = default_config.up;
    }

    int down = iniparser_getboolean(ini, (feature + ":down").c_str(), INIT_PARSER_NOKEY);
    if (down != INIT_PARSER_NOKEY)
    {
        config.down = (down == 1);
    }
    else
    {
        config.down = default_config.down;
    }

    int n_init = iniparser_getint(ini, (feature + ":n_init").c_str(), INIT_PARSER_NOKEY);
    if (n_init != INIT_PARSER_NOKEY)
    {
        config.n_init = n_init;
    }
    else
    {
        config.n_init = default_config.n_init;
    }

    double level = iniparser_getdouble(ini, (feature + ":level").c_str(), INIT_PARSER_NOKEY);
    if (level != INIT_PARSER_NOKEY)
    {
        config.level = level;
    }
    else
    {
        config.level = default_config.level;
    }

    return config;
}

Dispotcher::Dispotcher(string ini_file)
{
    dictionary *ini = iniparser_load(ini_file.c_str());
    if (not ini)
    {
        throw ios_base::failure("The config file has not been loaded");
    }
    string sec_name;
    int type;
    int n_sections = iniparser_getnsec(ini);

    cout << "Number of sections: " << n_sections << endl;

    cout << "Getting default config ... ";
    SpotConfig default_config = get_default_config(ini);
    cout << OK << endl;

    int default_type = iniparser_getint(ini, "default:bro_type", INIT_PARSER_NOKEY);
    if (default_type == INIT_PARSER_NOKEY)
    {
        default_type = 5; // BRO_TYPE_DOUBLE
    }

    cout << "Loading features config ... " << endl;
    for (int i = 0; i < n_sections; i++)
    {
        sec_name = string(iniparser_getsecname(ini, i));
        if (sec_name != "default")
        {
            cout << '\t' << sec_name << " ";
            type = iniparser_getint(ini, (sec_name + ":bro_type").c_str(), INIT_PARSER_NOKEY);
            if (type == INIT_PARSER_NOKEY)
            {
                type = default_type; // BRO_TYPE_DOUBLE
            }

            this->_features.emplace_back(sec_name, type, get_config(ini, sec_name, default_config));
            cout << OK << endl;
        }
    }
}

Dispotcher::Dispotcher(list<string> &feature_names, map<string, int> &feature_types,
                       map<string, SpotConfig> &configs)
{
    auto it = feature_names.begin();
    for (; it != feature_names.end(); ++it)
    {
        try
        {
            this->_features.emplace_back(*it, feature_types.at(*it), configs.at(*it));
        }
        catch (out_of_range &e)
        {
            this->_features.emplace_back(*it, BRO_TYPE_DOUBLE, Spot().config());
        }
    }
}

list<MonitoredFeature>::iterator Dispotcher::begin()
{
    return this->_features.begin();
}

list<MonitoredFeature>::iterator Dispotcher::end()
{
    return this->_features.end();
}

list<string> Dispotcher::names()
{
    list<string> ls;
    auto it = this->begin();
    for (; it != this->end(); ++it)
    {
        ls.push_back(it->_name);
    }

    return ls;
}

size_t Dispotcher::size()
{
    return this->_features.size();
}

SpotStatus Dispotcher::status(string feature)
{
    auto it = this->begin();
    for (; it != this->end(); ++it)
    {
        if (it->_name == feature)
        {
            return it->status();
        }
    }
    stringstream ss;
    ss << "The feature '" << feature << "' is unknown" << endl;
    throw invalid_argument(ss.str());
}

SpotConfig Dispotcher::config(string feature)
{
    auto it = this->begin();
    for (; it != this->end(); ++it)
    {
        if (it->_name == feature)
        {
            return it->config();
        }
    }
    stringstream ss;
    ss << "The feature '" << feature << "' is unknown" << endl;
    throw invalid_argument(ss.str());
}

void Dispotcher::reset(string feature)
{
    auto it = this->begin();
    for (; it != this->end(); ++it)
    {
        if (it->_name == feature)
        {
            SpotConfig conf = it->config();
            int type = it->_bro_type;
            this->_features.erase(it);
            this->_features.emplace_back(feature, type, conf);
            return;
        }
    }
    stringstream ss;
    ss << "The feature '" << feature << "' is unknown" << endl;
    throw invalid_argument(ss.str());
}