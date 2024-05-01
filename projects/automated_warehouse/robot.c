#include "projects/automated_warehouse/robot.h"
#include "projects/automated_warehouse/aw_message.h"
#include "projects/automated_warehouse/aw_manager.h"

/**
 * A function setting up robot structure
 */
void setRobot(struct robot* _robot, const char* name, int row, int col, int required_payload, int current_payload, pair* task) {
    _robot->name = name;
    _robot->row = row;
    _robot->col = col;
    _robot->required_payload = required_payload;
    _robot->current_payload = current_payload;
    _robot->task = task;
}


int moveRobot(struct robot* robot, struct message* msg) {
    enum command required_cmd = msg->cmd;
    msg->required_payload = required_cmd;
    int* cur = &msg->current_payload;

    int new_row = robot->row;
    int new_col = robot->col;

    switch (required_cmd) {
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
            *cur = CMD_NOP;
            return 0;
        default:
            return -1; // Invalid command
    }

    // Check if the new position is within the map boundaries
    if (new_row < ROW_MIN || new_row >= ROW_MAX || new_col < COL_MIN || new_col >= COL_MAX) {
        *cur = CMD_NOP;
        return 0;
    }

    char new_pos = map_draw_default[new_row][new_col];

    // Check if the new position is 'X'
    if (new_pos == 'X') {
        *cur = CMD_NOP;
        return 0;
    }

    // Check if the new position is '1', '2', '3', etc.
    if (new_pos >= '1' && new_pos <= '9') {
        if (robot->task->cargo != new_pos - '0') {
            *cur = CMD_NOP;
            return 0;
        }
    }

    // Check if the new position is 'A', 'B', 'C', etc.
    if (new_pos >= 'A' && new_pos <= 'Z') {
        if (robot->task->loading_dock != new_pos) {
            *cur = CMD_NOP;
            return 0;
        }
    }

    // TODO: check another robot is already there

    // Update the robot's position
    robot->row = new_row;
    robot->col = new_col;
    *cur = required_cmd;

    return 0;
}