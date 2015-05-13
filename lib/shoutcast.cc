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

#include "shoutcast.h"

bool connect_shoutcast(
    const int sock,
    const std::string password,
    const int bitrate,
    const bool stream_public,
    const std::string stream_name,
    const std::string stream_url,
    const std::string stream_genre)
{
    // Send the password followed by a new line
    std::string pw = password;
    pw.append("\n");
    if(send(sock, pw.c_str(), pw.length(), 0) < 0)
    {
        printf("Warning: Could not send password.\n");
        return false;
    }

    // Read two characters of SHOUTcast's answer
    char ok_buffer[2];
    if(recv(sock, ok_buffer, sizeof(ok_buffer), 0) < 0)
    {
        printf("Warning: SHOUTcast server not responding.\n");
        return false;
    }

    // The answer should be "OK2"
    if(ok_buffer[0] != 'O' || ok_buffer[1] != 'K')
    {
        printf("Warning: Could not connect to the SHOUTcast server. Are the server specifications and the password correct?\n");
        return false;
    }

    // Create the bitrate header
    std::ostringstream icy_br;
    icy_br << "icy-br:" << bitrate << "\n";

    // Create the name header
    std::string name_header("icy-name:");
    name_header += (stream_name != "")? stream_name : "No name";
    name_header += "\n";

    // Create the public header
    std::string pub_header(stream_public? "icy-pub:1\n" : "icy-pub:0\n");

    // Create additional headers
    std::string additional_headers;
    if(stream_url != "") additional_headers += "icy-url:" + stream_url + "\n";
    if(stream_genre != "") additional_headers += "icy-genre:" + stream_genre + "\n";

    // Put all headers in an array
    std::string headers[] = {
        name_header,
        pub_header,
        icy_br.str(),
        additional_headers,
        "\n\n"  // Two new lines to start feeding
    };

    // Send all headers
    for(int i=0; i<(sizeof(headers)/sizeof(headers[0])); i++)
    {
        if(send(sock, headers[i].c_str(), headers[i].length(), 0) < 0)
        {
            printf("Warning: Failed to send header \"%s\"", headers[i].c_str());
        }
    }

    return true;    // Connection successful
}
