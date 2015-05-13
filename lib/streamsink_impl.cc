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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "streamsink_impl.h"

namespace gr {
    namespace streamsink {

        streamsink::sptr streamsink::make(
            int samplerate, int mp3_bitrate,
            std::string type, std::string address, int port,
            std::string password, std::string icecast_user, std::string icecast_mountpoint
        )
        {
            return gnuradio::get_initial_sptr (new streamsink_impl(
                samplerate, mp3_bitrate,
                type, address, port,
                password, icecast_user, icecast_mountpoint
            ));
        }

        streamsink_impl::streamsink_impl(
            int samplerate, int mp3_bitrate,
            std::string type, std::string address, int port,
            std::string password, std::string icecast_user, std::string icecast_mountpoint
        )
            : gr::sync_block(
                "streamsink",
                gr::io_signature::make(1, 1, sizeof(float)),
                gr::io_signature::make(0, 0, 0))
        {
            // Check samplerate and bitrate
            input_samplerate = (samplerate >= 16000 && samplerate <= 48000)? samplerate : 48000;
            bitrate = (mp3_bitrate >= 8 && mp3_bitrate <= 320)? mp3_bitrate : 128;

            server_type = type;
            server_address = address;
            server_port = port;
            server_password = password;
            server_user = icecast_user;
            server_mountpoint = icecast_mountpoint;

            stream_public = false;
            stream_name = "No name";
            stream_url = "";
            stream_genre = "";
            stream_description = "";

            // Configure input for the block
            set_max_noutput_items(MAX_NOUTPUT_ITEMS);
            set_history(HISTORY_SIZE + 1);

            // Socket
            sock = -1;
            already_connected = false;
            last_connection_attempt = time(NULL);
            send_errors = 0;

            //signal(SIGPIPE, SIG_IGN);
            //signal(SIGCHLD, SIG_IGN);

            // Initialize lame
            lame = lame_init();
            lame_set_in_samplerate(lame, input_samplerate);
            lame_set_brate(lame, bitrate);
            lame_init_params(lame);
            len = 0;
            not_encoded = 0;
        }

        streamsink_impl::~streamsink_impl()
        {
            lame_close(lame);
            close_socket(sock);
        }

        void streamsink_impl::set_stream_public(bool pub)
        {
            stream_public = pub;
        }

        void streamsink_impl::set_stream_name(std::string name)
        {
            stream_name = name;
        }

        void streamsink_impl::set_stream_url(std::string url)
        {
            stream_url = url;
        }

        void streamsink_impl::set_stream_genre(std::string genre)
        {
            stream_genre = genre;
        }

        void streamsink_impl::set_stream_description(std::string description)
        {
            stream_description = description;
        }

        bool streamsink_impl::connect_server()
        {
            last_connection_attempt = time(NULL);

            // Create a new socket
            close_socket(sock);
            if((sock = create_socket(server_address, server_port)) < 0) return false;

            if(server_type == "icecast")
            {
                // Connect to an icecast server, try PUT method first
                for(int i=0; i<2; i++)
                {
                    // Create a new socket if PUT method failed
                    if(i == 1)
                    {
                        close_socket(sock);
                        if((sock = create_socket(server_address, server_port)) < 0) return false;
                    }

                    if(connect_icecast(sock, server_password, server_user, server_mountpoint,
                        ((i==0)? "PUT" : "SOURCE"), bitrate, input_samplerate,
                        stream_public, stream_name, stream_url, stream_genre, stream_description))
                    {
                        return true;    // Connection successful
                    }
                }
            }
            else
            {
                // Connect to a SHOUTcast server
                if(connect_shoutcast(sock, server_password, bitrate,
                    stream_public, stream_name, stream_url, stream_genre))
                {
                    return true;    // Connection successful
                }
            }

            return false;   // Connection failed

        }

        int streamsink_impl::work(
            int noutput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items)
        {
            // Convert the input buffer
            const float *in = (const float *) input_items[0];

            // Connect to the streaming server if this is the first time working
            if(!already_connected)
            {
                if(connect_server()) printf("Connection to streaming server established.\n");
                already_connected = true;
            }

            // Try to reconnect if sending failed MAX_SEND_ERRORS times in a row
            if(send_errors >= MAX_SEND_ERRORS)
            {
                not_encoded = 0;    // Reset the number of not encoded items

                // Try to reconnect if the last attempt is more than RECONNECT_TIME seconds ago
                if(difftime(time(NULL), last_connection_attempt) > RECONNECT_TIME)
                {
                    printf("The connection to the server was lost, trying to reconnect...\n");

                    if(connect_server())
                    {
                        send_errors = 0;
                        printf("Connection to streaming server established.\n");
                    }
                    else return noutput_items;  // Connection failed, finish work
                }
                else return noutput_items;  // Connection is not alive, finish work
            }

            // Check the number of not encoded items
            if(not_encoded > HISTORY_SIZE)  // This should actually not happen
            {
                printf("Warning: Number of not encoded items was out of range.\n");
                not_encoded = HISTORY_SIZE;
            }

            // Check if enough items are available
            if((not_encoded + noutput_items) >= MP3_FRAME_SIZE)
            {
                in += (HISTORY_SIZE - not_encoded); // Make in point to the beginning of the frame

                // Encode the frame
                len = lame_encode_buffer_ieee_float(lame,
                    in, in, MP3_FRAME_SIZE,
                    (unsigned char *) output_buffer, MP3_FRAME_SIZE);

                not_encoded -= MP3_FRAME_SIZE;  // Decrease the number of not encoded items

                if(len > 0)
                {
                    // Send the encoded audio
                    if(send(sock, output_buffer, len, 0) < 0)
                    {
                        send_errors++;  // Increase send_errors if sending failed
                        //printf("Warning: Could not send mp3 frame.\n");
                    }
                    else send_errors = 0;   // Reset send_errors
                }
                else
                {
                    printf("Warning: Failed to encode mp3 frame.\n");
                }
            }
            else
            {
                // Increase the number of not encoded items
                not_encoded += noutput_items;
            }

            return noutput_items;
        }

    } /* namespace streamsink */
} /* namespace gr */
