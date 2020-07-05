/*
 * Copyright (c) 2005-2008, The Android Open Source Project
 * Copyright (c) 2009-2013, Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <logger.h>
#include <logprint.h>

#ifdef DEBUG_ON
#define _D(...) printf(__VA_ARGS__)
#else
#define _D(...) do { } while (0)
#endif
#define _E(...) fprintf(stderr, __VA_ARGS__)

#define COMMAND_MAX 5
#define DELIMITER " "
#define FILE_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)
#define MAX_ARGS 16
#define MAX_ROTATED 4
#define MAX_QUEUED 4096
#define BUFFER_MAX 100
#define INTERVAL_MAX 60*60

#define CONFIG_FILE "/usr/etc/dlog_logger.conf"

#define ARRAY_SIZE(name) (sizeof(name)/sizeof(name[0]))

struct queued_entry {
	union {
		unsigned char buf[LOGGER_ENTRY_MAX_LEN + 1] __attribute__((aligned(4)));
		struct logger_entry entry __attribute__((aligned(4)));
	};
	struct queued_entry *next;
};

struct log_command {
	char *filename;
	int option_file;
	int option_buffer;
	int rotate_size;
	int max_rotated;
	int devices[LOG_ID_MAX];
	log_format *format;
};

struct log_work {
	char *filename;
	bool printed;
	int fd;
	int size;
	int rotate_size;
	int max_rotated;
	log_format *format;
	struct log_work *next;
};

struct log_task_link {
	struct log_work *work;
	struct log_task_link *next;
};

struct log_device {
	int id;
	int fd;
	struct queued_entry *queue;
	struct log_task_link *task;
	struct log_device *next;
};

static const char *device_path_table[] = {
	[LOG_ID_MAIN] = "/dev/log_main",
	[LOG_ID_RADIO] = "/dev/log_radio",
	[LOG_ID_SYSTEM] = "/dev/log_system",
	[LOG_ID_APPS] = "/dev/log_apps",
	[LOG_ID_MAX] = NULL
};

static struct log_work *works;
static struct log_device *devices;
static int device_list[] = {
	[LOG_ID_MAIN] = false,
	[LOG_ID_RADIO] = false,
	[LOG_ID_SYSTEM] = false,
	[LOG_ID_APPS] = false,
	[LOG_ID_MAX] = false,
};

static int buffer_size = 0;
static int min_interval = 0;

/*
 * get log device id from device path table by device name
 */
static int get_device_id_by_name(const char *name)
{
	int i;

	if (name == NULL)
		return -1;
	for (i = 0; i < ARRAY_SIZE(device_path_table); i++) {
		if (strstr(device_path_table[i], name) != NULL)
			return i;
	}

	return -1;
}

/*
 * check device registration on watch device list
 */
static int check_device(int id)
{
	if (id < 0 || LOG_ID_MAX <= id)
		return 0;

	return (device_list[id] == true) ? 0 : -1;
}

/*
 * register device to watch device list
 */
static int register_device(int id)
{
	if (id < 0 || LOG_ID_MAX <= id)
		return -1;
	device_list[id] = true;

	return 0;
}

/*
 * comparison function to distinct entries by time
 */
static int cmp(struct queued_entry *a, struct queued_entry *b)
{
	int n = a->entry.sec - b->entry.sec;
	if (n != 0)
		return n;

	return a->entry.nsec - b->entry.nsec;
}

/*
 * enqueueing the log_entry into the log_device
 */
static void enqueue(struct log_device *device, struct queued_entry *entry)
{
	if (device->queue == NULL) {
		device->queue = entry;
	} else {
		struct queued_entry **e = &device->queue;
		while (*e && cmp(entry, *e) >= 0)
			e = &((*e)->next);
		entry->next = *e;
		*e = entry;
	}
}

/*
 * open file
 */
static int open_work(const char *path)
{
	return open(path, O_WRONLY | O_APPEND | O_CREAT, FILE_PERMS);
}

/*
 * rotate log files
 */
static void rotate_logs(struct log_work *logwork)
{
	int i, ret;
	char *filename;
	char file0[NAME_MAX];
	char file1[NAME_MAX];

	close(logwork->fd);
	filename = logwork->filename;
	for (i = logwork->max_rotated ; i > 0 ; i--) {
		snprintf(file1, NAME_MAX, "%s.%d", filename, i);
		if (i - 1 == 0)
			snprintf(file0, NAME_MAX, "%s",  filename);
		else
			snprintf(file0, NAME_MAX, "%s.%d", filename, i - 1);
		ret = rename(file0, file1);
		if (ret < 0 && errno != ENOENT)
			_E("while rotating log works");
	}
	/* open log file again */
	logwork->fd = open_work(filename);
	if (logwork->fd < 0) {
		_E("couldn't open log file");
		exit(EXIT_FAILURE);
	}
	logwork->size = 0;

	return;
}

/*
 * process to print log
 * and check the log file size to rotate
 */
static void process_buffer(struct log_device *dev, struct logger_entry *buf)
{
	int bytes_written, err;
	log_entry entry;
	struct log_work *logwork;
	struct log_task_link *task;

	err = log_process_log_buffer(buf, &entry);
	if (err < 0)
		goto exit;

	for (task = dev->task; task; task = task->next) {
		logwork = task->work;
		if (log_should_print_line(logwork->format,
					entry.tag, entry.priority)) {
			bytes_written =
				log_print_log_line(logwork->format,
						logwork->fd, &entry);
			if (bytes_written < 0) {
				_E("work error");
				exit(EXIT_FAILURE);
			}
			logwork->size += bytes_written;
		}
		if (logwork->rotate_size > 0 &&
				(logwork->size / 1024) >= logwork->rotate_size) {
			rotate_logs(logwork);
		}
	}

exit:
	return;
}

/*
 * choose first device by log_entry
 */
static void choose_first(struct log_device *dev, struct log_device **firstdev)
{
	for (*firstdev = NULL; dev != NULL; dev = dev->next) {
		if (dev->queue != NULL &&
				(*firstdev == NULL ||
				 cmp(dev->queue,
					 (*firstdev)->queue) < 0)) {
			*firstdev = dev;
		}
	}
}

/*
 * print beginnig string into the log files
 */
static void maybe_print_start(struct log_device *dev)
{
	struct log_work *logwork;
	struct log_task_link *task;
	char buf[1024];

	for (task = dev->task; task; task = task->next) {
		logwork = task->work;
		if (!logwork->printed) {
			logwork->printed = true;
			snprintf(buf, sizeof(buf),
					"dlog--------- beginning of %s\n",
					device_path_table[dev->id]);
			if (write(logwork->fd, buf, strlen(buf)) < 0) {
				_E("maybe work error");
				exit(EXIT_FAILURE);
			}
		}
	}
}

/*
 * skip log_entry
 */
static void skip_next_entry(struct log_device *dev)
{
	maybe_print_start(dev);
	struct queued_entry *entry = dev->queue;
	dev->queue = entry->next;
	free(entry);
}

/*
 * print log_entry
 */
static void print_next_entry(struct log_device *dev)
{
	maybe_print_start(dev);
	process_buffer(dev, &dev->queue->entry);
	skip_next_entry(dev);
}

/*
 * do logging
 */
static void do_logger(struct log_device *dev)
{
	time_t commit_time = 0, current_time = 0;
	struct log_device *pdev;
	int ret, result;
	fd_set readset;
	bool sleep = false;
	int queued_lines = 0;
	int max = 0;

	if (min_interval)
		commit_time = current_time = time(NULL);

	for (pdev = dev; pdev; pdev = pdev->next) {
		if (pdev->fd > max)
			max = pdev->fd;
	}

	while (1) {
		do {
			struct timeval timeout = { 0, 5000 /* 5ms */ };
			FD_ZERO(&readset);
			for (pdev = dev; pdev; pdev = pdev->next)
				FD_SET(pdev->fd, &readset);
			result = select(max + 1, &readset, NULL, NULL,
					sleep ? NULL : &timeout);
		} while (result == -1 && errno == EINTR);

		if (result < 0)
			continue;

		for (pdev = dev; pdev; pdev = pdev->next) {
			if (FD_ISSET(pdev->fd, &readset)) {
				struct queued_entry *entry =
					(struct queued_entry *)
					malloc(sizeof(struct queued_entry));
				if (entry == NULL) {
					_E("failed to malloc queued_entry\n");
					goto exit;//exit(EXIT_FAILURE);
				}
				entry->next = NULL;
				ret = read(pdev->fd, entry->buf,
						LOGGER_ENTRY_MAX_LEN);
				if (ret < 0) {
					if (errno == EINTR) {
						free(entry);
						goto next;
					}
					if (errno == EAGAIN) {
						free(entry);
						break;
					}
					_E("dlogutil read");
					goto exit;//exit(EXIT_FAILURE);
				} else if (!ret) {
					free(entry);
					_E("read: Unexpected EOF!\n");
					exit(EXIT_FAILURE);
				} else if (entry->entry.len !=
						ret - sizeof(struct logger_entry)) {
					free(entry);
					_E("unexpected length. Expected %d, got %d\n",
							entry->entry.len,
							ret - sizeof(struct logger_entry));
					goto exit;//exit(EXIT_FAILURE);
				}

				entry->entry.msg[entry->entry.len] = '\0';

				enqueue(pdev, entry);
				++queued_lines;

				if (MAX_QUEUED < queued_lines) {
					_D("queued_lines = %d\n", queued_lines);
					while (true) {
						choose_first(dev, &pdev);
						if (pdev == NULL)
							break;
						print_next_entry(pdev);
						--queued_lines;
					}
					if (min_interval)
						commit_time = time(NULL);
					break;
				}
			}
		}

		if (min_interval) {
			current_time = time(NULL);
			if (current_time - commit_time < min_interval &&
					queued_lines < buffer_size) {
				sleep = true;
				continue;
			}
		}

		if (result == 0) {
			sleep = true;
			while (true) {
				choose_first(dev, &pdev);
				if (pdev == NULL)
					break;
				print_next_entry(pdev);
				--queued_lines;
				if (min_interval)
					commit_time = current_time;
			}
		} else {
			/* print all that aren't the last in their list */
			sleep = false;
			while (true) {
				choose_first(dev, &pdev);
				if (pdev == NULL || pdev->queue->next == NULL)
					break;
				print_next_entry(pdev);
				--queued_lines;
				if (min_interval)
					commit_time = current_time;
			}
		}
next:
		;
	}
exit:
	exit(EXIT_FAILURE);
}


/*
 * create a work
 */
static struct log_work *work_new(void)
{
	struct log_work *work;

	work = malloc(sizeof(struct log_work));
	if (work == NULL) {
		_E("failed to malloc log_work\n");
		return NULL;
	}
	work->filename = NULL;
	work->fd = -1;
	work->printed = false;
	work->size = 0;
	work->next = NULL;
	_D("work alloc %p\n", work);

	return work;
}

/*
 * add a new log_work to the tail of chain
 */
static int work_add_to_tail(struct log_work *work, struct log_work *nwork)
{
	struct log_work *tail = work;

	if (!nwork)
		return -1;

	if (work == NULL) {
		work = nwork;
		return 0;
	}

	while (tail->next)
		tail = tail->next;
	tail->next = nwork;

	return 0;
}

/*
 * add a new work task to the tail of chain
 */
static void work_add_to_device(struct log_device *dev, struct log_work *work)
{
	struct log_task_link *tail;

	if (!dev || !work)
		return;
	_D("dev %p work %p\n", dev, work);
	if (dev->task == NULL) {
		dev->task =
			(struct log_task_link *)
			malloc(sizeof(struct log_task_link));
		if (dev->task == NULL) {
			_E("failed to malloc log_task_link\n");
			return;
		}
		tail = dev->task;
	} else {
		tail = dev->task;
		while (tail->next)
			tail = tail->next;
		tail->next =
			(struct log_task_link *)
			malloc(sizeof(struct log_task_link));
		if (tail->next == NULL) {
			_E("failed to malloc log_task_link\n");
			return;
		}
		tail = tail->next;
	}
	tail->work = work;
	tail->next = NULL;
}

/*
 * free work file descriptor
 */
static void work_free(struct log_work *work)
{
	if (!work)
		return;
	if (work->filename) {
		free(work->filename);
		work->filename = NULL;
		if (work->fd != -1) {
			close(work->fd);
			work->fd = -1;
		}
	}
	log_format_free(work->format);
	work->format = NULL;
	free(work);
	work = NULL;
}

/*
 * free all the nodes after the "work" and includes itself
 */
static void work_chain_free(struct log_work *work)
{
	if (!work)
		return;
	while (work->next) {
		struct log_work *tmp = work->next;
		work->next = tmp->next;
		work_free(tmp);
	}
	work_free(work);
	work = NULL;
}

/*
 * create a new log_device instance
 * and open device
 */
static struct log_device *device_new(int id)
{
	struct log_device *dev;

	if (LOG_ID_MAX <= id)
		return NULL;
	dev = malloc(sizeof(struct log_device));
	if (dev == NULL) {
		_E("failed to malloc log_device\n");
		return NULL;
	}
	dev->id = id;
	dev->fd = open(device_path_table[id], O_RDONLY);
	if (dev->fd < 0) {
		_E("Unable to open log device '%s': %s\n",
				device_path_table[id],
				strerror(errno));
		free(dev);
		return NULL;
	}
	_D("device new id %d fd %d\n", dev->id, dev->fd);
	dev->task = NULL;
	dev->queue = NULL;
	dev->next = NULL;

	return dev;
}

/*
 * add a new log_device to the tail of chain
 */
static int device_add_to_tail(struct log_device *dev, struct log_device *ndev)
{
	struct log_device *tail = dev;

	if (!dev || !ndev)
		return -1;

	while (tail->next)
		tail = tail->next;
	tail->next = ndev;

	return 0;
}

/*
 * add a new log_device or add to the tail of chain
 */
static void device_add(int id)
{
	if (check_device(id) < 0)
		return;

	if (!devices) {
			devices = device_new(id);
			if (devices == NULL) {
				_E("failed to device_new: %s\n",
						device_path_table[id]);
				exit(EXIT_FAILURE);
			}
	} else {
		if (device_add_to_tail(devices, device_new(id)) < 0) {
			_E("failed to device_add_to_tail: %s\n",
					device_path_table[id]);
			exit(EXIT_FAILURE);
		}
	}
	return;
}

/*
 * free one log_device  and it doesn't take care of chain so it
 * may break the chain list
 */
static void device_free(struct log_device *dev)
{
	if (!dev)
		return;
	if (dev->queue) {
		while (dev->queue->next) {
			struct queued_entry *tmp =
				dev->queue->next;
			dev->queue->next = tmp->next;
			free(tmp);
		}
		free(dev->queue);
		dev->queue = NULL;
	}
	if (dev->task) {
		while (dev->task->next) {
			struct log_task_link *tmp =
				dev->task->next;
			dev->task->next = tmp->next;
			free(tmp);
		}
		free(dev->task);
		dev->task = NULL;
	}
	free(dev);
	dev = NULL;
}

/*
 * free all the nodes after the "dev" and includes itself
 */
static void device_chain_free(struct log_device *dev)
{
	if (!dev)
		return;
	while (dev->next) {
		struct log_device *tmp = dev->next;
		dev->next = tmp->next;
		device_free(tmp);
	}
	device_free(dev);
	dev = NULL;
}

/*
 * parse command line
 * using getopt function
 */
static int parse_command_line(char *linebuffer, struct log_command *cmd)
{
	int i, ret, id, argc;
	char *argv[MAX_ARGS];
	char *tok, *cmdline;

	if (linebuffer == NULL || cmd == NULL)
		return -1;
	/* copy command line */
	cmdline = strdup(linebuffer);
	tok = strtok(cmdline, DELIMITER);
	/* check the availability of command line
	   by comparing first word with dlogutil*/
	if (!tok || strcmp(tok, "dlogutil")) {
		_D("Ignore this line (%s)\n", linebuffer);
		free(cmdline);
		return -1;
	}
	_D("Parsing this line (%s)\n", linebuffer);
	/* fill the argc and argv
	   for extract option from command line */
	argc = 0;
	while (tok && (argc < MAX_ARGS)) {
		argv[argc] = strdup(tok);
		tok = strtok(NULL, DELIMITER);
		argc++;
	}
	free(cmdline);

	/* initialize the command struct with the default value */
	memset(cmd->devices, 0, sizeof(cmd->devices));
	cmd->option_file = false;
	cmd->option_buffer = false;
	cmd->filename = NULL;
	cmd->rotate_size = 0;
	cmd->max_rotated = MAX_ROTATED;
	cmd->format = (log_format *)log_format_new();

	/* get option and fill the command struct */
	while ((ret = getopt(argc, argv, "f:r:n:v:b:")) != -1) {
		switch (ret) {
		case 'f':
			cmd->filename = strdup(optarg);
			_D("command filename %s\n", cmd->filename);
			cmd->option_file = true;
			break;
		case 'b':
			id = get_device_id_by_name(optarg);
			_D("command device name %s id %d\n", optarg, id);
			if (id < 0 || LOG_ID_MAX <= id)
				break;
			cmd->option_buffer = true;
			/* enable to log in device on/off struct */
			cmd->devices[id] = true;
			/* enable to open in global device on/off struct */
			register_device(id);
			break;
		case 'r':
			if (!isdigit(optarg[0]))
				goto exit_free;
			cmd->rotate_size = atoi(optarg);
			_D("command rotate size %d\n", cmd->rotate_size);
			break;
		case 'n':
			if (!isdigit(optarg[0]))
				goto exit_free;
			cmd->max_rotated = atoi(optarg);
			_D("command max rotated %d\n", cmd->max_rotated);
			break;
		case 'v':
			{
				log_print_format print_format;
				print_format = log_format_from_string(optarg);
				if (print_format == FORMAT_OFF) {
					_E("failed to add format\n");
					goto exit_free;
				}
				_D("command format %s\n", optarg);
				log_set_print_format(cmd->format, print_format);
			}
			break;
		default:
			break;
		}
	}
	/* add filter string, when command line have tags */
	if (argc != optind) {
		for (i = optind ; i < argc ; i++) {
			ret = log_add_filter_string(cmd->format, argv[i]);
			_D("command add fileter string %s\n", argv[i]);
			if (ret < 0) {
				_E("Invalid filter expression '%s'\n", argv[i]);
				goto exit_free;
			}
		}
	} else {
		ret = log_add_filter_string(cmd->format, "*:d");
		if (ret < 0) {
			_E("Invalid silent filter expression\n");
			goto exit_free;
		}
	}
	/* free argv */
	for (i = 0; i < argc; i++)
		free(argv[i]);

	if (cmd->option_file == false)
		goto exit_free;
	/* If it have not the -b option,
	   set the default devices to open and log */
	if (cmd->option_buffer == false) {
		_D("set default device\n");
		cmd->devices[LOG_ID_MAIN] = true;
		cmd->devices[LOG_ID_SYSTEM] = true;
		register_device(LOG_ID_MAIN);
		register_device(LOG_ID_SYSTEM);
	}
	/* for use getopt again */
	optarg = NULL;
	optind = 1;
	optopt = 0;

	return 0;

exit_free:
	if (cmd->filename)
		free(cmd->filename);
	return -1;
}

/*
 * parse command from configuration file
 * and return the command list with number of command
 * if an command was successfully parsed, then it returns number of parsed command.
 * on error or not founded, 0 is returned
 */
static int parse_command(struct log_command *command_list)
{
	FILE *fp;
	int ncmd;
	char *line_p;
	char linebuffer[1024];

	fp = fopen(CONFIG_FILE, "re");
	if (!fp) {
		_E("no config file\n");
		goto exit;
	}
	ncmd = 0;
	while (fgets(linebuffer, sizeof(linebuffer), fp) != NULL) {
		line_p = strchr(linebuffer, '\n');
		if (line_p != NULL)
			*line_p = '\0';
		if (parse_command_line(linebuffer, &command_list[ncmd]) == 0)
			ncmd++;
		if (COMMAND_MAX <= ncmd)
			break;
	}
	fclose(fp);

	return ncmd;

exit:
	return 0;
}

/*
 * free dynamically allocated memory
 */
static void cleanup(void)
{
	work_chain_free(works);
	device_chain_free(devices);
}

/*
 * SIGINT, SIGTERM, SIGQUIT signal handler
 */
static void sig_handler(int signo)
{
	_D("sig_handler\n");
	cleanup();
	exit(EXIT_SUCCESS);
}

static int help(void) {

	printf("%s [OPTIONS...] \n\n"
			"Logger, records log messages to files.\n\n"
			"  -h      Show this help\n"
			"  -b N    Set number of logs to keep logs in memory buffer bafore write files (default:0, MAX:100)\n"
			"  -t N    Set minimum interval time to write files (default:0, MAX:3600, seconds)\n",
			program_invocation_short_name);

	return 0;
}

static int parse_argv(int argc, char *argv[]) {
	int ret = 1, option;

	while ((option = getopt(argc, argv, "hb:t:")) != -1) {
		switch (option) {
			case 't':
				if (!isdigit(optarg[0])) {
					ret = -EINVAL;
					printf("Wrong argument!\n");
					help();
					goto exit;
				}
				min_interval = atoi(optarg);
				if (min_interval < 0 || INTERVAL_MAX < min_interval)
					min_interval = 0;
				ret = 1;
				break;
			case 'b':
				if (!isdigit(optarg[0])) {
					ret = -EINVAL;
					printf("Wrong argument!\n");
					help();
					goto exit;
				}
				buffer_size = atoi(optarg);
				if (buffer_size < 0 || BUFFER_MAX < buffer_size)
					buffer_size = 0;
				ret = 1;
				break;
			case 'h':
				help();
				ret = 0;
				goto exit;
			default:
				ret = -EINVAL;
		}
	}
exit:
	optarg = NULL;
	optind = 1;
	optopt = 0;
	return ret;
}


int main(int argc, char **argv)
{
	int i, r, ncmd;
	struct stat statbuf;
	struct log_device *dev;
	struct log_work *work;
	struct log_command command_list[COMMAND_MAX];
	struct sigaction act;

	/* set the signal handler for free dynamically allocated memory. */
	memset(&act, 0, sizeof(struct sigaction));
	sigemptyset(&act.sa_mask);
	act.sa_handler = (void *)sig_handler;
	act.sa_flags = 0;

	if (sigaction(SIGQUIT, &act, NULL) < 0)
		_E("failed to sigaction for SIGQUIT");
	if (sigaction(SIGINT, &act, NULL) < 0)
		_E("failed to sigaction for SIGINT");
	if (sigaction(SIGTERM, &act, NULL) < 0)
		_E("failed to sigaction for SIGTERM");

	if (argc != 1) {
		r = parse_argv(argc, argv);
		if (r <= 0)
			goto exit;
	}
	/* parse command from command configuration file. */
	ncmd = parse_command(command_list);
	/* If it have nothing command, exit. */
	if (!ncmd)
		goto exit;

	/* create log device */
	device_add(LOG_ID_MAIN);
	device_add(LOG_ID_SYSTEM);
	device_add(LOG_ID_RADIO);

	/* create work from the parsed command */
	for (i = 0; i < ncmd; i++) {
		work = work_new();
		_D("work new\n");
		if (work == NULL) {
			_E("failed to work_new\n");
			goto clean_exit;
		}
		/* attatch the work to global works variable */
		if (work_add_to_tail(works, work) < 0) {
			_E("failed to work_add_to_tail\n");
			goto clean_exit;
		}
		/* 1. set filename, fd and file current size */
		work->filename = command_list[i].filename;
		if (work->filename == NULL) {
			_E("file name is NULL");
			goto clean_exit;
		}
		work->fd = open_work(work->filename);
		if (work->fd < 0) {
			_E("failed to open log file");
			exit(EXIT_FAILURE);
		}
		if (fstat(work->fd, &statbuf) == -1)
			work->size = 0;
		else
			work->size = statbuf.st_size;

		/* 2. set size limits for log files */
		work->rotate_size = command_list[i].rotate_size;

		/* 3. set limit on the number of rotated log files */
		work->max_rotated = command_list[i].max_rotated;

		/* 4. set log_format to filter logs*/
		work->format = command_list[i].format;

		/* 5. attatch the work to device task for logging */
		dev = devices;
		while (dev) {
			if (command_list[i].devices[dev->id] == true) {
				work_add_to_device(dev, work);
			}
			dev = dev->next;
		}
	}

	/* do log */
	do_logger(devices);

clean_exit:
	work_chain_free(works);
	device_chain_free(devices);
exit:
	return 0;
}
