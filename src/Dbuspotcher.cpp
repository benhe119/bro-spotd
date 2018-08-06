//
// Created by asr on 02/07/18.
//

#include <zconf.h>
#include "Dbuspotcher.h"

Dbuspotcher::Dbuspotcher(Dispotcher *dsp) : _dsp(dsp)
{
    // initialise the errors
    DBusError err;

    this->_err = &err;
    dbus_error_init(this->_err);

    // connect to the bus
    cout << "Connection to the bus ";
    // requests a session bus (not a system)
    this->_conn = dbus_bus_get(DBUS_BUS_SESSION, this->_err);
    if (dbus_error_is_set(this->_err))
    {
        cout << FAIL << endl;
        stringstream ss;
        ss << "Connection error (" << this->_err->message << ")" << endl;
        dbus_error_free(this->_err);
        throw runtime_error(ss.str());
    }
    cout << OK << endl;

    // requesting a bus name
    // string bus_name = "bro.spot";
    cout << "Requesting the bus name: " << BRO_SPOT_BUS_NAME << " ";
    int req = dbus_bus_request_name(this->_conn, BRO_SPOT_BUS_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING,
                                    this->_err);

    if (req == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
    {
        cout << OK << endl;
    }

    if (dbus_error_is_set(this->_err))
    {
        cout << FAIL << endl;
        stringstream ss;
        ss << "Name error (" << this->_err->message << ")" << endl;
        dbus_error_free(this->_err);
        throw runtime_error(ss.str());
    }
}

Dbuspotcher::~Dbuspotcher()
{
    dbus_connection_close(this->_conn);
}

void Dbuspotcher::listen()
{
    DBusMessage *msg;

    bool killed = false;

    while (not killed)
    {
        // non blocking read of the next available message
        dbus_connection_read_write(this->_conn, 0);
        msg = dbus_connection_pop_message(this->_conn);

        // loop again if we haven't got a message
        if (msg == nullptr)
        {
            sleep(1);
        }
        else
        {
            // Method to list the features
            if (dbus_message_is_method_call(msg, "bro.spot.Common", "Features"))
            {
                this->reply_features(msg);

                // method to get the details of the Spot instance monitoring a given feature
            }
            else if (dbus_message_is_method_call(msg, "bro.spot.Spot", "Status"))
            {
                this->reply_spotstatus(msg);

                // method to get the config of the Spot instance monitoring a given feature
            }
            else if (dbus_message_is_method_call(msg, "bro.spot.Spot", "Config"))
            {
                this->reply_spotconfig(msg);

                // method to reset a Spot instance
            }
            else if (dbus_message_is_method_call(msg, "bro.spot.Spot", "Reset"))
            {
                this->reply_reset(msg);
                // method to reset a Spot instance
            }
            else if (dbus_message_is_method_call(msg, "org.freedesktop.DBus.Introspectable", "Introspect"))
            {
                this->reply_introspection(msg);
                // method to end the program
            }
            else if (dbus_message_is_method_call(msg, "bro.spot.Common", "Kill"))
            {
                killed = true;
                this->reply(msg, "Stopping the daemon");
                dbus_message_unref(msg);
                exit(0);
            }
            // free the message
            dbus_message_unref(msg);
        }
    }
}

void Dbuspotcher::_send_flush_unref(DBusMessage *reply, string extra_info)
{
    if (!dbus_connection_send(this->_conn, reply, NULL))
    {
        string info = "[" + extra_info + "] Reply has not been sent";
        throw overflow_error(info);
    }
    // Blocks until the outgoing message queue is empty
    dbus_connection_flush(this->_conn);

    // free the reply
    dbus_message_unref(reply);
}

void Dbuspotcher::reply(DBusMessage *msg, string info)
{
    // Initialize an mesg iterator
    DBusMessageIter args;

    // Initialize the reply
    DBusMessage *reply = dbus_message_new_method_return(msg);

    // Ready to append arguments to the reply
    dbus_message_iter_init_append(reply, &args);

    // Appending the info message
    char *c = (char *)info.c_str();
    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &c);

    this->_send_flush_unref(reply, "INFO");
}

void Dbuspotcher::error(DBusMessage *msg, string error_message)
{
    // Initialize an mesg iterator
    DBusMessageIter args;
    // Initialize the reply
    DBusMessage *reply = dbus_message_new_method_return(msg);
    // Ready to append arguments to the reply
    dbus_message_iter_init_append(reply, &args);

    // Preparing and appending the error message
    error_message.insert(0, "[ERROR] ");
    char *c = (char *)error_message.c_str();
    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &c);

    this->_send_flush_unref(reply, "ERROR");
}

void Dbuspotcher::reply_features(DBusMessage *msg)
{
    DBusMessageIter args;

    // retrieve the monitored features
    list<string> str_list = this->_dsp->names();

    // create a reply from the message
    DBusMessage *reply = dbus_message_new_method_return(msg);

    // add the arguments to the reply
    dbus_message_iter_init_append(reply, &args);
    char *c;
    for (auto it = str_list.begin(); it != str_list.end(); ++it)
    {
        c = (char *)it->data();
        dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &c);
    }

    this->_send_flush_unref(reply, "FEATURES");
}

string read_file(string file_path)
{
    ifstream file(file_path, std::ios::in);
    stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void Dbuspotcher::reply_introspection(DBusMessage *msg)
{
    // retrieve the info
    string xml = read_file("/home/asr/CLionProjects/bro-spotd2/bro.spot.xml");
    // Reply!
    this->reply(msg, xml);
}

void append_spot_status(DBusMessageIter *it, SpotStatus &ss)
{
    dbus_message_iter_append_basic(it, DBUS_TYPE_INT32, &ss.n);
    dbus_message_iter_append_basic(it, DBUS_TYPE_INT32, &ss.ex_up);
    dbus_message_iter_append_basic(it, DBUS_TYPE_INT32, &ss.ex_down);
    dbus_message_iter_append_basic(it, DBUS_TYPE_INT32, &ss.Nt_up);
    dbus_message_iter_append_basic(it, DBUS_TYPE_INT32, &ss.Nt_down);
    dbus_message_iter_append_basic(it, DBUS_TYPE_INT32, &ss.al_up);
    dbus_message_iter_append_basic(it, DBUS_TYPE_INT32, &ss.al_down);
    dbus_message_iter_append_basic(it, DBUS_TYPE_DOUBLE, &ss.t_up);
    dbus_message_iter_append_basic(it, DBUS_TYPE_DOUBLE, &ss.t_down);
    dbus_message_iter_append_basic(it, DBUS_TYPE_DOUBLE, &ss.z_up);
    dbus_message_iter_append_basic(it, DBUS_TYPE_DOUBLE, &ss.z_down);
}

void append_spot_config(DBusMessageIter *it, SpotConfig &sc)
{
    dbus_message_iter_append_basic(it, DBUS_TYPE_DOUBLE, &sc.q);
    dbus_message_iter_append_basic(it, DBUS_TYPE_BOOLEAN, &sc.bounded);
    dbus_message_iter_append_basic(it, DBUS_TYPE_INT32, &sc.max_excess);
    dbus_message_iter_append_basic(it, DBUS_TYPE_BOOLEAN, &sc.alert);
    dbus_message_iter_append_basic(it, DBUS_TYPE_BOOLEAN, &sc.up);
    dbus_message_iter_append_basic(it, DBUS_TYPE_BOOLEAN, &sc.down);
    dbus_message_iter_append_basic(it, DBUS_TYPE_INT32, &sc.n_init);
    dbus_message_iter_append_basic(it, DBUS_TYPE_DOUBLE, &sc.level);
}

void Dbuspotcher::reply_spotstatus(DBusMessage *msg)
{
    DBusMessageIter args;
    char *param;

    // Point to the first argument
    if (!dbus_message_iter_init(msg, &args))
        this->error(msg, "Message has no arguments!");
    // check the type of the argument (it must be a string)
    else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        this->error(msg, "Argument is not a string!");
        // Reply
    }
    else
    {
        // Retrieve the value of the argument (the name of the feature)
        dbus_message_iter_get_basic(&args, &param);
        string feature(param);

        // create a reply from the message
        DBusMessage *reply = dbus_message_new_method_return(msg);

        // add the arguments to the reply
        dbus_message_iter_init_append(reply, &args);

        // Get the status of the feature
        // it raises an exception if the feature does not exists
        try
        {
            SpotStatus status = this->_dsp->status(feature);
            append_spot_status(&args, status);
        }
        catch (invalid_argument &e)
        {
            this->error(msg, e.what());
        }

        // Send the reply
        this->_send_flush_unref(reply, "SPOT STATUS " + feature);
    }
}

void Dbuspotcher::reply_spotconfig(DBusMessage *msg)
{
    DBusMessageIter args;
    char *param;

    // Point to the first argument
    if (!dbus_message_iter_init(msg, &args))
        this->error(msg, "Message has no arguments!");
    // check the type of the argument (it must be a string)
    else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        this->error(msg, "Argument is not a string!");
    }
    else
    {
        // Retrieve the value of the argument (the name of the feature)
        dbus_message_iter_get_basic(&args, &param);
        string feature(param);

        // create a reply from the message
        DBusMessage *reply = dbus_message_new_method_return(msg);

        // add the arguments to the reply
        dbus_message_iter_init_append(reply, &args);

        // Get the status of the feature
        // it raises an exception if the feature does not exists
        try
        {
            SpotConfig conf = this->_dsp->config(feature);
            append_spot_config(&args, conf);
        }
        catch (invalid_argument &e)
        {
            this->error(msg, e.what());
        }

        // send th reply
        this->_send_flush_unref(reply, "SPOT CONFIG " + feature);
    }
}

void Dbuspotcher::reply_reset(DBusMessage *msg)
{
    DBusMessageIter args;
    char *param;

    // Point to the first argumentlinux where to put ini files
    if (!dbus_message_iter_init(msg, &args))
        this->error(msg, "Message has no arguments!");
    // check the type of the argument (it must be a string)
    else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        this->error(msg, "Argument is not a string!");
        // Reply
    }
    else
    {
        // Retrieve the value of the argument (the name of the feature)
        dbus_message_iter_get_basic(&args, &param);
        string feature(param);

        // create a reply from the message
        DBusMessage *reply = dbus_message_new_method_return(msg);

        // add the arguments to the reply
        dbus_message_iter_init_append(reply, &args);

        // Get the status of the feature
        // it raises an exception if the feature does not exists
        try
        {
            this->_dsp->reset(feature);
            string info = "The feature " + feature + " has been reset";
            this->reply(msg, info);
        }
        catch (invalid_argument &e)
        {
            this->error(msg, e.what());
        }

        // send th reply
        this->_send_flush_unref(reply, "RESET " + feature);
    }
}