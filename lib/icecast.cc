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

#include "icecast.h"

bool connect_icecast(
    const int sock,
    const std::string password,
    const std::string user,
    const std::string mountpoint,
    const std::string method,
    const int bitrate,
    const int samplerate,
    const bool stream_public,
    const std::string stream_name,
    const std::string stream_url,
    const std::string stream_genre,
    const std::string stream_description)
{
    // Create the first header line
    std::string first_header;
    first_header += ((method == "PUT")? "PUT " : "SOURCE ");
    if(mountpoint.length() == 0) first_header += "/stream";
    else if((char) mountpoint.at(0) == '/') first_header += mountpoint;
    else first_header += "/" + mountpoint;
    first_header += ((method == "PUT")? " HTTP/1.1\n" : " HTTP/1.0\n");

    // Create the authorization header
    std::string authorization = user + ":" + password;
    authorization = "Authorization: Basic " + base64_encode(authorization.c_str(), authorization.length()) + "\n";

    // Create the audio info header
    std::ostringstream ice_audio_info;
    ice_audio_info << "ice-audio-info: ice-bitrate=" << bitrate << ";ice-channels=1;ice-samplerate=" << samplerate << "\n";

    // Create the name header
    std::string name_header("ice-name:");
    name_header += (stream_name != "")? stream_name : "No name";
    name_header += "\n";

    // Create the public header
    std::string pub_header(stream_public? "ice-public:1\n" : "ic-public:0\n");

    // Create additional headers
    std::string additional_headers;
    if(stream_url != "") additional_headers += "ice-url:" + stream_url + "\n";
    if(stream_genre != "") additional_headers += "ice-genre:" + stream_genre + "\n";
    if(stream_description != "") additional_headers += "ice-description:" + stream_description + "\n";

    // Put all headers in an array
    std::string headers[] = {
        first_header,
        authorization,
        "User-Agent: gr-streamsink\n",
        "Content-Type: audio/mpeg\n",
        name_header,
        pub_header,
        ice_audio_info.str(),
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

    // Read icecast's answer
    char ok_buffer[15];
    if(recv(sock, ok_buffer, sizeof(ok_buffer), 0) < 0)
    {
        printf("Warning: Icecast server not responding.\n");
        return false;
    }

    // The answer should be "HTTP/1.0 200 OK"
    if(ok_buffer[9] != '2' || ok_buffer[10] != '0' || ok_buffer[11] != '0' || ok_buffer[13] != 'O' || ok_buffer[14] != 'K')
    {
        // Print a warning if the SOURCE method was used
        if(method != "PUT") printf("Warning: Could not connect to the icecast server. Are the server specifications and the password correct?\n");
        return false;
    }

    return true;    // Connection successful
}
