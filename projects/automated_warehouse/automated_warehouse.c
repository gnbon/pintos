#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "threads/init.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"

#include "devices/timer.h"

#include "projects/automated_warehouse/automated_warehouse.h"
#include "projects/automated_warehouse/aw_debug.h"
#include "projects/automated_warehouse/aw_manager.h"
#include "projects/automated_warehouse/aw_message.h"
#include "projects/automated_warehouse/central_node.h"

struct robot* robots; // Robot informations managed by actual robots
struct robot* robots_info; // Robot informations managed by central control node
int num_robots;

/** message boxes from central control node to each robot */
struct messsage_box* boxes_from_central_control_node;
/** message boxes from robots to central control node */
struct messsage_box* boxes_from_robots;

// test code for central control node thread
void test_cnt(void){
	bool *received = malloc(sizeof(bool) * num_robots); // TODO: destroy this
	for (int i = 0; i < num_robots; i++) {
		received[i] = false;
	}

	while(1){
		thread_sleep(200);
		print_map(robots, num_robots);

		bool received_all = false;
		while (!received_all) {
			received_all = true;
			for (int i = 0; i < num_robots; i++) {
				if (received[i]) {      
					continue;
				}
				received_all = false;
				struct message msg;
				int res = recv_message(i, &msg, CENTRAL_CONTROL);
				if (res == 0) {
					setRobot(&robots_info[i], NULL, msg.row, msg.col, msg.required_payload, msg.current_payload);
					enum command cmd = find_path(robots_info, i);
					set_cmd(&msg, cmd);
					send_message(i, &msg, CENTRAL_CONTROL);
					received[i] = true;
				} 
			}
		}
		for (int i = 0; i < num_robots; i++) {
			received[i] = false;
		}
		increase_step();
		unblock_threads();
	}
}

// test code for robot thread
void test_thread(void* aux){
	int idx = *((int *)aux);
	int test = 0;
	int res = 0;
	// initialize robot
	struct message msg;
	
	while(1){
		memset(&msg, 0, sizeof(struct message));
		printf("thread %d : %d\n", idx, test++);
		res = recv_message(idx, &msg, ROBOT);
		if(res == 0){
			moveRobot(robots, idx, num_robots, &msg);
			send_message(idx, &msg, ROBOT);
			block_thread();
		}
	}
}

int parse_args(char *task_list, req_task *tasks) {
	char *saveptr;
	char *token = strtok_r(task_list, ":", &saveptr);
	int count = 0;

	while (token != NULL) {
		char number = token[0];
		char letter = token[1];

		if (count >= num_robots) {
			printf("Too many robots\n");
			return -1;
		}

		if (number < '0' || number > '9') {
			printf("Invalid number format: %s\n", token);
			return -2; 
		}
		
		if (letter < 'A' || letter > 'Z') {
			printf("Invalid letter format: %s\n", token);
			return -3;
		}

		if (token[2]) {
			printf("Invalid format: %s\n", token);
			return -4;
		}

		tasks[count].req_load = (short)number;
		tasks[count].req_dork = (short)letter;
		count++;

		token = strtok_r(NULL, ":", &saveptr);
	}

	for (int i = 0; i < count; i++) {
		INFO("task", "robot %d: %c%c", i + 1, tasks[i].req_load, tasks[i].req_dork);
	}

	return 0;
}

#define COMBINE(upper, lower) ((upper << 16) | lower)

int initialize_robots(req_task *tasks) {
	robots = malloc(sizeof(struct robot) * num_robots); // TODO: destroy this
	tid_t* threads = malloc(sizeof(tid_t) * num_robots); // TODO: destroy this
	char * rnames = malloc(sizeof(char) * num_robots * 3); // TODO: destroy this
	int * robot_idxs = malloc(sizeof(int) * num_robots); // TODO: destroy this

	for (int i = 0; i < num_robots; i++) {
		robot_idxs[i] = i;
		snprintf(rnames, 3, "R%d", i + 1);
		// int required_payload = COMBINE(tasks[i].req_load, tasks[i].req_dork);
		setRobot(&robots[i], rnames, ROW_W, COL_W, COMBINE(tasks[i].req_dork, tasks[i].req_load), PAYLOAD_UNLOAD);
		threads[i] = thread_create(rnames, 0, &test_thread, &robot_idxs[i]); // task 책임은 로봇한테 있다
		if (threads[i] == TID_ERROR) {
			printf("Thread creation failed\n");
			return -1;
		}
		rnames += 3;
	}

	return 0;
}

int initialize_central_control_node(req_task *tasks) {
	robots_info = malloc(sizeof(struct robot) * num_robots); // TODO: destroy this
	char * rnames = malloc(sizeof(char) * num_robots * 3); // TODO: destroy this

	for (int i = 0; i < num_robots; i++) {
		snprintf(rnames, 3, "R%d", i + 1);
		setRobot(&robots_info[i], rnames, ROW_W, COL_W, COMBINE(tasks[i].req_dork, tasks[i].req_load), PAYLOAD_UNLOAD);
	}

	tid_t* cnt_thread = thread_create("CNT", 0, &test_cnt, NULL);

	if (cnt_thread == TID_ERROR) {
		printf("Thread creation failed\n");
		return -1;
	}

	return 0;
}

void run_automated_warehouse(char **argv)
{
	init_automated_warehouse(argv); // do not remove this

	num_robots = atoi(argv[1]);
	char *task_list = argv[2];

	req_task* tasks = malloc(sizeof(req_task) * num_robots);
	if (tasks == NULL) {
		printf("Memory allocation failed\n");
		return -1;
	}

	int res = parse_args(task_list, tasks);
	if (res < 0) {
		printf("Parsing failed\n");
		return;
	}

	res = initialize_message_boxes(num_robots);
	if (res < 0) {
		printf("Message box initialization failed\n");
		return;
	}

	res = initialize_central_control_node(tasks);
	if (res < 0) {
		printf("Central control node initialization failed\n");
		return;
	}
	res = initialize_robots(tasks);
	if (res < 0) {
		printf("Robot initialization failed\n");
		return;
	}
}