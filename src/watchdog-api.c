/*
   Copyright (c) 2012, Stanislav Yakush(st.yakush@yandex.ru)
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the watchdog-ctl nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>

#include "watchdog-api.h"

int open_watchdog(const char *path)
{
	return open(path, O_RDWR);
}

void enable_watchdog(int fd)
{
	int flags = WDIOS_ENABLECARD;

	ioctl(fd, WDIOC_SETOPTIONS, &flags);
}

void disable_watchdog(int fd)
{
	int flags = WDIOS_DISABLECARD;

	ioctl(fd, WDIOC_SETOPTIONS, &flags);
}

void set_timeout_watchdog(int fd, int timeout)
{
	ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
}

int get_timeout_watchdog(int fd)
{
	int timeout;

	ioctl(fd, WDIOC_GETTIMEOUT, &timeout);

	return timeout;
}

void reset_watchdog(int fd)
{
	int dummy;

	ioctl(fd, WDIOC_KEEPALIVE, &dummy);
}

void close_watchdog(int fd)
{
	close(fd);
}

int watchdog_ctl(const char *path, int action, int arg)
{
	int fd;
	int result = 0;

	if ((fd = open_watchdog(path)) == -1) {
		return -1;
	}

	switch (action)
	{
		case WATCHDOG_TEST:
			break;

		case WATCHDOG_ENABLE:
			enable_watchdog(fd);
			break;

		case WATCHDOG_DISABLE:
			disable_watchdog(fd);
			break;

		case WATCHDOG_SET_TIMEOUT:
			set_timeout_watchdog(fd, arg);
			break;

		case WATCHDOG_GET_TIMEOUT:
			result = get_timeout_watchdog(fd);
			break;

		case WATCHDOG_RESET:
			reset_watchdog(fd);
			break;

		default:
			close_watchdog(fd);
			return -1;
	}

	close_watchdog(fd);

	return result;
}
