/* 
 * h264bitstream - a library for reading and writing H.264 video
 * Copyright (C) 2005-2006 Auroras Entertainment, LLC
 * 
 * Written by Alex Izvorski <aizvorski@gmail.com>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "h264_stream.h"

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE 8*1024*1024



int main(int argc, char *argv[])
{
	uint8_t* buf = (uint8_t*)malloc(BUFSIZE);
	h264_stream_t* h = (h264_stream_t*)malloc(sizeof(h264_stream_t));
	nal_t* nal = (nal_t*)malloc(sizeof(nal_t));
	sps_t* sps = (sps_t*)malloc(sizeof(sps_t));
	pps_t* pps = (pps_t*)malloc(sizeof(pps_t));
	slice_header_t* sh = (slice_header_t*)malloc(sizeof(slice_header_t));
	h->nal = nal;
	h->sps = sps;
	h->pps = pps;
	h->sh = sh;

	if (argc < 2)
	{
		printf("h264_analyze, version 0.1.3\n");
		printf("Usage: \n");
		printf("  h264_analyze stream.h264\n");
		printf("where stream.h264 is a raw H264 stream, as produced by JM or x264\n");
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd == -1) { perror("could not open file"); exit(0); }

	int rsz = 0;
	int sz = 0;
	int off = 0;
	uint8_t* p = buf;

	int nal_start, nal_end;
		
	while (rsz = read(fd, buf + sz, BUFSIZE - sz))
	{
		sz += rsz;

		while (find_nal_unit(p, sz, &nal_start, &nal_end) > 0)
		{
			printf("!! Found NAL at offset %04X, size %04X (%d) \n", 
				   off + (p - buf) + nal_start, (nal_end - nal_start), (nal_end - nal_start));
			p += nal_start;
			read_nal_unit(h, p, nal_end - nal_start);
			debug_nal(h, h->nal);
			p += (nal_end - nal_start);
			sz -= nal_end;
		}

		memmove(buf, p, sz);
		off += p - buf;
		p = buf;
	}

}
