#ifndef __PROJECTS_AUTOMATED_WAREHOUSE_H__
#define __PROJECTS_AUTOMATED_WAREHOUSE_H__

#include "projects/automated_warehouse/robot.h"

#define PAYLOAD_UNLOAD 0
#define PAYLOAD_LOAD 1

void test_cnt(void);

void test_thread(void* aux);

int parse_args(char *task_list, req_task *tasks);

int initialize_robots(req_task *tasks);

int initialize_central_control_node(req_task *tasks);

void run_automated_warehouse(char **argv);

extern struct robot* robots_info;

#endif 
