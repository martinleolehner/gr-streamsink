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

#include "socketfuncs.h"

int create_socket(std::string address, int port)
{
    // Create a new socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("Error: Could not create socket.\n");
        return -1;
    }

    struct sockaddr_in server;

    // Check if the address is an IP address
    if(inet_addr(address.c_str()) == -1)
    {
        // Get the host by name
        struct hostent *he;
        struct in_addr **addr_list;

        if((he = gethostbyname(address.c_str())) == NULL)
        {
            // Try to use localhost if it failed to get the host by name
            server.sin_addr.s_addr = inet_addr("127.0.0.1");
            printf("Warning: Could not resolve hostname \"%s\", trying to use localhost instead.\n", address.c_str());
        }
        else
        {
            addr_list = (struct in_addr **) he->h_addr_list;

            if(addr_list[0] != NULL)
            {
                server.sin_addr = *addr_list[0];
                printf("%s resolved to %s\n", address.c_str(), inet_ntoa(*addr_list[0]));
            }
            else
            {
                // Try to use localhost if it failed to get the host by name
                server.sin_addr.s_addr = inet_addr("127.0.0.1");
                printf("Warning: Could not resolve hostname \"%s\", trying to use localhost instead.\n", address.c_str());
            }
        }
    }
    else
    {
        server.sin_addr.s_addr = inet_addr(address.c_str()); // Set the IP address
    }

    // Set properties and port
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Connect the socket
    if(connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        printf("Warning: Failed to establish the socket connection.\n");
        return -1;
    }

    // Set a timeout for reading
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(struct timeval)) != 0)
    {
        printf("Warning: Failed to set socket timeout option.\n");
    }

    return sock;
}

void close_socket(int sock)
{
    // Close the old socket
    if(sock >= 0)
    {
        if(close(sock) != 0) printf("Warning: Could not close socket.");
    }
}
