#ifndef _PROJECTS_PROJECT1_ROBOT_H__
#define _PROJECTS_PROJECT1_ROBOT_H__

#include <stdbool.h>

#include "projects/automated_warehouse/aw_message.h"

typedef struct {
    char cargo; // 0 1 2
    char loading_dock; // A B C
    bool is_loaded;
} pair;

/**
 * A Structure representing robot
 */
struct robot {
    const char* name;
    int row;
    int col;
    int required_payload;
    int current_payload;
    pair* task;
};

void setRobot(struct robot* _robot, const char* name, int row, int col, int required_payload, int current_payload, pair* task);

int moveRobot(struct robot* robots, int idx, int number_of_robots, struct message* msg);

#endif