/* -*- c++ -*- */
/*
* Copyright 2015 Martin Lehner - HES-SO Valais-Wallis
*
* This file is part of gr-streamsink
*
* gr-streamsink is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3, or (at your option)
* any later version.
*
* gr-streamsink is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with gr-streamsink. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INCLUDED_STREAMSINK_STREAMSINK_IMPL_H
#define INCLUDED_STREAMSINK_STREAMSINK_IMPL_H

#include <streamsink/streamsink.h>

#include <lame/lame.h>
#include <time.h>
//#include <signal.h>
#include <string>
#include <stdio.h>

#include "socketfuncs.h"
#include "shoutcast.h"
#include "icecast.h"

#define MAX_SEND_ERRORS 10
#define RECONNECT_TIME 10
#define HISTORY_SIZE 10000  // History size must be greater than the mp3 frame size
#define MAX_NOUTPUT_ITEMS 1000
#define MP3_FRAME_SIZE 8192

namespace gr {
    namespace streamsink {

        class streamsink_impl : public streamsink
        {
        private:
            int input_samplerate;
            int bitrate;
            std::string server_type;
            std::string server_address;
            int server_port;
            std::string server_password;
            std::string server_user;
            std::string server_mountpoint;

            bool stream_public;
            std::string stream_name;
            std::string stream_url;
            std::string stream_genre;
            std::string stream_description;

            int sock;
            bool already_connected;
            time_t last_connection_attempt;
            int send_errors;

            lame_t lame;
            char output_buffer[MP3_FRAME_SIZE];
            int len;
            int not_encoded;


            bool connect_server();

        public:
            streamsink_impl(
                int samplerate,
                int mp3_bitrate,
                std::string type,
                std::string address,
                int port,
                std::string password,
                std::string icecast_user,
                std::string icecast_mountpoint
            );

            ~streamsink_impl();

            void set_stream_public(bool pub);
            void set_stream_name(std::string name);
            void set_stream_url(std::string url);
            void set_stream_genre(std::string genre);
            void set_stream_description(std::string description);

            int work(int noutput_items,
                gr_vector_const_void_star &input_items,
                gr_vector_void_star &output_items);
        };

    } // namespace streamsink
} // namespace gr

#endif /* INCLUDED_STREAMSINK_STREAMSINK_IMPL_H */
