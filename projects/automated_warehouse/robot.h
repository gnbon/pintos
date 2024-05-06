#ifndef _PROJECTS_PROJECT1_ROBOT_H__
#define _PROJECTS_PROJECT1_ROBOT_H__

#include <stdbool.h>

#include "projects/automated_warehouse/aw_message.h"

#define UPPER(num) ((num >> 16) & 0xFFFF)
#define LOWER(num) (num & 0xFFFF)

typedef  struct  __attribute__ ((__packed__)) {
    short req_load; // '0', '1', '2', ...
    short req_dork; // 'A', 'B', 'C', ...
} req_task;

/**
 * A Structure representing robot
 */
struct robot {
    const char* name;
    int row;
    int col;
    int required_payload;
    int current_payload;
    int nop_count;
};

void setRobot(struct robot* _robot, const char* name, int row, int col, int required_payload, int current_payload);

int moveRobot(struct robot* robots, int idx, int number_of_robots, struct message* msg);

#endif