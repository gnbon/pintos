#include <stdio.h>

#include "projects/automated_warehouse/aw_debug.h"
#include "projects/automated_warehouse/robot.h"
#include "projects/automated_warehouse/aw_message.h"
#include "projects/automated_warehouse/aw_manager.h"

#define DEBUG(...) printf(__VA_ARGS__)

/**
 * A function setting up robot structure
 */
void setRobot(struct robot* _robot, const char* name, int row, int col, int required_payload, int current_payload) {
    _robot->name = name;
    _robot->row = row;
    _robot->col = col;
    _robot->required_payload = required_payload;
    _robot->current_payload = current_payload;
    INFO("robot", "set robot %s at (%d, %d) with required_payload: %c, current_payload: %c", name, row, col, required_payload, current_payload);
}

bool is_loaded(struct robot* robot) {
    return robot->current_payload == robot->required_payload;
}

int moveRobot(struct robot* robots, int idx, int number_of_robots, struct message* msg) {
    struct robot* robot = &robots[idx];
    enum command cmd = msg->cmd;

    if (is_loaded(robot) && map_draw_default[robot->row][robot->col] == robot->current_payload) {
        return 0;
    }

    int new_row = msg->row;
    int new_col = msg->col;

    switch (cmd) {
        case CMD_UP:
            new_row--;
            break;
        case CMD_DOWN:
            new_row++;
            break;
        case CMD_LEFT:
            new_col--;
            break;
        case CMD_RIGHT:
            new_col++;
            break;
        case CMD_NOP:
            break;
        default:
            return -1; // Invalid command
    }
    
    // Check if the new position is within the map boundaries
    if (new_row < ROW_MIN || new_row >= ROW_MAX || new_col < COL_MIN || new_col >= COL_MAX) {
        return 0;
    }

    char new_pos = map_draw_default[new_row][new_col];

    // Check if the new position is 'X'
    if (new_pos == 'X') {
        ERROR("robot", "robot %s hit the wall at (%d, %d)", robot->name, new_row, new_col);
        return 0;
    }

    // Check if the new position is '0', '2', '3', etc.
    if (new_pos >= '0' && new_pos <= '9') {
        if (robot->current_payload != new_pos) {
            ERROR("robot", "robot %s hit the wrong payload at (%d, %d)", robot->name, new_row, new_col);
            return 0;
        }
    }

    // Check if the new position is 'A', 'B', 'C', etc. (excluding 'W' and 'S')
    if (new_pos >= 'A' && new_pos <= 'Z' && new_pos != 'W' && new_pos != 'S') {
        if (robot->required_payload != new_pos) {
            ERROR("robot", "robot %s hit the wrong required payload at (%d, %d)", robot->name, new_row, new_col);
            return 0;
        }
    }

    // Check another robot is already there
    for (int robotIdx = 0; robotIdx < number_of_robots; robotIdx++){
        struct robot* __robot = &robots[robotIdx];
        if (__robot->col == new_col && __robot->row == new_row){
            WARN("robot", "robot %s hit another robot %s at (%d, %d)", robot->name, __robot->name, new_row, new_col);
            return 0;
        }
    }
                    
    // Update the robot's position
    robot->row = new_row;
    robot->col = new_col;

    if (new_pos == robot->current_payload) {
            INFO("robot", "robot %s load %s at (%d, %d)", robot->name, robot->current_payload, new_row, new_col);
            robot->current_payload = robot->required_payload;
    }
    INFO("robot", "move %s to (%d, %d)", robot->name, new_row, new_col);
    return 0;
}