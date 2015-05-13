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


#ifndef INCLUDED_STREAMSINK_STREAMSINK_H
#define INCLUDED_STREAMSINK_STREAMSINK_H

#include <streamsink/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
    namespace streamsink {

        /*!
        * \brief A GNURadio sink block to stream the audio to SHOUTcast or Icecast
        * \ingroup streamsink
        *
        */
        class STREAMSINK_API streamsink : virtual public gr::sync_block
        {
        public:
            typedef boost::shared_ptr<streamsink> sptr;

            /*!
            * \brief Return a shared_ptr to a new instance of streamsink::streamsink.
            *
            * To avoid accidental use of raw pointers, streamsink::streamsink's
            * constructor is in a private implementation
            * class. streamsink::streamsink::make is the public interface for
            * creating new instances.
            */
            static sptr make(
                int samplerate,
                int bitrate = 128,
                std::string type = "SHOUTcast",
                std::string address = "127.0.0.1",
                int port = 8001,
                std::string password = "",
                std::string icecast_user = "source",
                std::string icecast_mountpoint = "/stream"
            );

            virtual void set_stream_public(bool pub) = 0;
            virtual void set_stream_name(std::string name) = 0;
            virtual void set_stream_url(std::string url) = 0;
            virtual void set_stream_genre(std::string genre) = 0;
            virtual void set_stream_description(std::string description) = 0;
        };

    } // namespace streamsink
} // namespace gr

#endif /* INCLUDED_STREAMSINK_STREAMSINK_H */
