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

#if defined(__linux__)
	#define _XOPEN_SOURCE 700
#else
	#define _XOPEN_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "main.h"
#include "watchdog-api.h"

int loop = 0;

void daemonize(void)
{
	int fd;

	if (fork() != 0) {
		exit(0);
	}

	setsid();

	if ((fd = open("/dev/null", O_RDWR, 0)) != -1)
	{
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		if (fd > STDERR_FILENO) {
			close(fd);
		}
	}
}

void sigterm_handler(int sig)
{
	NOTUSED(sig);

	printf("Received SIGTERM...\n");

	exit(0);
}

void setup_signals(void)
{
	struct sigaction sig;

	sigemptyset(&sig.sa_mask);
	sig.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
	sig.sa_handler = sigterm_handler;

	sigaction(SIGTERM, &sig, NULL);
}

void usage(void)
{
	printf(
		"watchdog-ctl %s\n"
		"Usage: watchdog-ctl [options]\n"
		"--daemon - run as daemon\n"
		"-e or --enable - enable watchdog timer\n"
		"-d or --disable - disable watchdog timer\n"
		"-s <arg> or --set-timeout <arg> - set timeout to watchdog timer\n"
		"-g or --get-timeout - get timeout for watchdog timer\n"
		"-r or --reset - reset watchdog timer\n"
		"-l or --loop - reset watchdog timer in infinity loop\n"
		"-h or --help - show this message and exit\n",
		WATCHDOG_CTL_VERSION);
}

void parse_args(int argc, char *argv[])
{
	int i, last_arg;

	for (i = 1; i < argc; i++)
	{
		last_arg = i == argc - 1;

		if (!strcmp(argv[i], "--daemon")) {
			daemonize();
		} else if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--enable")) {
			printf("Enable watchdog timer\n");
			watchdog_ctl(WATCHDOG_PATH, WATCHDOG_ENABLE, 0);
		} else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--disable")) {
			printf("Disable watchdog timer\n");
			watchdog_ctl(WATCHDOG_PATH, WATCHDOG_DISABLE, 0);
		} else if ((!strcmp(argv[i], "-s") || !strcmp(argv[i], "--set-timeout")) && !last_arg) {
			int timeout = atoi(argv[i + 1]);
			printf("Set timeout %d to watchdog timer\n", timeout);
			watchdog_ctl(WATCHDOG_PATH, WATCHDOG_SET_TIMEOUT, timeout);
		} else if (!strcmp(argv[i], "-g") || !strcmp(argv[i], "--get-timeout")) {
			int timeout = watchdog_ctl(WATCHDOG_PATH, WATCHDOG_GET_TIMEOUT, 0);
			printf("Watchdog timeout: %d\n", timeout);
		} else if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--reset")) {
			printf("Reset watchdog\n");
			watchdog_ctl(WATCHDOG_PATH, WATCHDOG_RESET, 0);
		} else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--loop")) {
			loop = 1;
		} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			usage();
			exit(0);
		}
	}
}

int main(int argc, char *argv[])
{
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);

	setup_signals();

	if (watchdog_ctl(WATCHDOG_PATH, WATCHDOG_TEST, 0) == -1) {
		fprintf(stderr, "Error open watchdog device %s\n", WATCHDOG_PATH);
	}

	parse_args(argc, argv);

	if (loop)
	{
		int timeout;

		if ((timeout = watchdog_ctl(WATCHDOG_PATH, WATCHDOG_GET_TIMEOUT, 0)) == -1) {
			fprintf(stderr, "Error start infinity loop\n");
			return -1;
		}

		printf("Infinity loop with timeout %d started...\n", --timeout);
		for (;;) {
			watchdog_ctl(WATCHDOG_PATH, WATCHDOG_RESET, 0);
			sleep(timeout);
		}
	}

	return 0;
}
