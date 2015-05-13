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

#include "base64.h"

std::string base64_encode(const char *input, const int len)
{
    std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string output;

    for(int i=0; i<(len - (len % 3)); i+=3)
    {
        output += base64_chars[(input[i] & 0xfc) >> 2];
        output += base64_chars[((input[i] & 0x03) << 4) + ((input[i+1] & 0xf0) >> 4)];
        output += base64_chars[((input[i+1]  & 0x0f) << 2) + ((input[i+2] & 0xc0) >> 6)];
        output += base64_chars[(input[i+2] & 0x3f)];
    }

    if(len % 3 == 2)
    {
        output += base64_chars[(input[len - 2] & 0xfc) >> 2];
        output += base64_chars[((input[len - 2] & 0x03) << 4) + ((input[len - 1] & 0xf0) >> 4)];
        output += base64_chars[((input[len - 1]  & 0x0f) << 2)];
        output += '=';
    }
    else if(len % 3 == 1)
    {
        output += base64_chars[(input[len - 1] & 0xfc) >> 2];
        output += base64_chars[((input[len - 1] & 0x03) << 4)];
        output += '=';
        output += '=';
    }

    return output;
}
