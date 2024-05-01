#ifndef _PROJECTS_PROJECT1_ROBOT_H__
#define _PROJECTS_PROJECT1_ROBOT_H__

#include "projects/automated_warehouse/aw_message.h"

typedef struct {
    int cargo;
    char loading_dock;
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

int moveRobot(struct robot* robot, struct message* msg);

#endif