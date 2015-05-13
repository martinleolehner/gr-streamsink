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

#ifndef INCLUDED_ICECAST_H
#define INCLUDED_ICECAST_H

#include <sys/socket.h>
#include <string>
#include <sstream>
#include <stdio.h>

#include "base64.h"

bool connect_icecast(
    const int sock,
    const std::string password,
    const std::string user,
    const std::string mountpoint,
    const std::string method,
    const int bitrate,
    const int samplerate,
    const bool stream_public = false,
    const std::string stream_name = "No name",
    const std::string stream_url = "",
    const std::string stream_genre = "",
    const std::string stream_description = ""
);

#endif /* INCLUDED_ICECAST_H */
