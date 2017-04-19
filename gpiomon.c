/**
 * Copyright © 2017 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <libevdev/libevdev.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(FILE *stream, char *pname)
{
	fprintf(stream, "Usage: %s EVDEV KEYCODE\n", pname);
}

static int do_gpiomon(const char *evdev, unsigned int code)
{
	struct libevdev *dev = NULL;
	struct input_event ev;
	struct pollfd pollfd;
	int initial;
	int ret = 1;

	if ((pollfd.fd = open(evdev, O_RDONLY|O_NONBLOCK)) < 0) {
		perror("Failed opening evdev device");
		return -1;
	}

	if ((ret = libevdev_new_from_fd(pollfd.fd, &dev)) < 0) {
		perror("Failed initializing libevdev\n");
		return -1;
	}

	ret = libevdev_fetch_event_value(dev, EV_KEY, code, &initial);
	if (ret <= 0) {
		fprintf(stderr, "Failed determining keycode initial state\n");
		return -1;
	}

	printf("Initial state of keycode[%d]: %d\n", code, initial);

	while (1) {
		pollfd.events = POLLIN | POLLERR;
		if((ret = poll(&pollfd, 1, -1)) < 0) {
			perror("Failed waiting for evdev");
			return -1;
		}
		if (pollfd.revents & POLLERR) {
			fprintf(stderr, "POLLERR waiting for readable evdev\n");
			return -1;
		}

		while (ret >= 0) {
			ret = libevdev_next_event(dev,
					LIBEVDEV_READ_FLAG_NORMAL, &ev);
			if (ret < 0)
				break;

			if (ev.type == EV_SYN && ev.code == SYN_REPORT)
				continue;
			if (ev.code != code)
				continue;

			printf("Event: %s %s %d\n",
					libevdev_event_type_get_name(ev.type),
					libevdev_event_code_get_name(ev.type, ev.code),
					ev.value);
		}
	}

	return ret;
}

int main(int argc, char** argv)
{
	int opt;
	int ret = 0;

	while ((opt = getopt(argc, argv, "")) != -1) { }

	if (optind == argc) {
		fprintf(stderr, "%s; No evdev device specified.\n", argv[0]);
		ret = 1;
		goto exit;
	}

	if (optind +1 == argc) {
		fprintf(stderr, "%s; No linux keycode specified.\n", argv[1]);
		ret = 1;
		goto exit;
	}

	ret = do_gpiomon(argv[optind], strtoul(argv[optind +1], 0, 0));

exit:
	if (ret == 1)
		usage(stderr, argv[0]);

	exit(ret);
}
