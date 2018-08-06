//
// Created by asr on 18/04/18.
//

#include "BroConnection.h"

const char *bro_init_failure::what() const throw()
{
    return "The bro connection pointer is null";
}

BroConnection::BroConnection(string ip, int port, int flags)
{
    this->_ip = ip;
    this->_port = port;
    string dst = this->_ip + ':' + to_string(this->_port);

    bro_init(NULL);
    this->_bc = bro_conn_new_str(dst.c_str(), flags);
    if (not this->_bc)
    {
        throw bro_init_failure();
    }
}

BroConnection::~BroConnection()
{
    bro_conn_delete(this->_bc);
}

bool BroConnection::connect(bool wait)
{
    cout << "Connecting ... ";
    if (wait)
    {
        while (not this->is_connected())
        {
            bro_conn_connect(this->_bc);
        }
    }
    else
    {
        bro_conn_connect(this->_bc);
    }

    if (this->is_connected())
    {
        cout << OK << endl;
        return true;
    }
    else
    {
        cout << FAIL << endl;
        return false;
    }
}

bool BroConnection::is_connected()
{

    if (bro_conn_alive(this->_bc) == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void BroConnection::add_event(string event_name, BroEventFunc callback, void *user_data)
{
    cout << "Adding event " << event_name << " ";
    bro_event_registry_add(this->_bc, event_name.c_str(), callback, user_data);
    cout << OK << endl;
}

void BroConnection::send_event(BroEvent *event)
{
    bro_event_send(this->_bc, event);
    bro_event_free(event);
}

void BroConnection::run()
{
    while (this->is_connected())
    {
        bro_conn_process_input(this->_bc);
    }
}

thread BroConnection::silent_run()
{
    return std::thread(&BroConnection::run, this);
    //return thread([this] { this->run(); });
}

string BroConnection::get_ip()
{
    return this->_ip;
}

int BroConnection::get_port()
{
    return this->_port;
}

BroConnection::operator bool() const
{
    return (this->_bc != NULL);
}