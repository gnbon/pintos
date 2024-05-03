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

bool robot_is_loaded(struct robot* robot) {
    return robot->current_payload > robot->required_payload;
}

bool robot_is_unloaded(struct robot* robot) {
    bool is_pos_alpha = map_draw_default[robot->row][robot->col] >= 'A' && map_draw_default[robot->row][robot->col] <= 'Z';
    bool is_pos_current = robot->current_payload == map_draw_default[robot->row][robot->col];
    return is_pos_alpha && is_pos_current;
}

void update_msg(struct message* msg, struct robot* robot) {
    msg->row = robot->row;
    msg->col = robot->col;
    msg->current_payload = robot->current_payload;
    msg->required_payload = robot->required_payload;
    INFO("robot", "update msg row: %d, col: %d, current_payload: %c, required_payload: %c", msg->row, msg->col, msg->current_payload, msg->required_payload);
}


bool isOutOfBound(int row, int col) {
    return row < ROW_MIN || row > ROW_MAX || col < COL_MIN || col > COL_MAX;
}

bool isWall(int row, int col) {
    return map_draw_default[row][col] == 'X';
}

bool isAnotherCurrentPayload(int row, int col, int current_payload) {
    return map_draw_default[row][col] >= '0' && map_draw_default[row][col] <= '9' && 
        map_draw_default[row][col] != current_payload;
}

bool isAnotherRequiredPayload(int row, int col, int current_payload) {
    return map_draw_default[row][col] >= 'A' && map_draw_default[row][col] <= 'Z' && 
        map_draw_default[row][col] != 'W' && map_draw_default[row][col] != 'S' && 
        map_draw_default[row][col] != current_payload;
}

bool isAnotherRobot(struct robot* robots, int number_of_robots, int idx, int row, int col) {
    for (int robotIdx = 0; robotIdx < number_of_robots; robotIdx++){
        if (robotIdx == idx) {
            continue;
        }
        struct robot* robot = &robots[robotIdx];
        if (robot->col == col && robot->row == row){
            return true;
        }
    }
    return false;
}

enum command find_movable_direction(struct robot* robots, int idx, int number_of_robots) {
    struct robot* robot = &robots[idx];
   
    int row = robot->row;
    int col = robot->col;
    char current = robot->current_payload;
    char required = robot->required_payload;

    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int new_row = row + dr[i];
        int new_col = col + dc[i];
        if (isOutOfBound(new_row, new_col)) {
            continue;
        }
        
        if (isWall(new_row, new_col)) {
            continue;
        }

        if (isAnotherRobot(robots, number_of_robots, idx, new_row, new_col)) {
            continue;
        }
        
        if (isAnotherCurrentPayload(new_row, new_col, current)) {
            continue;
        }

        if (isAnotherRequiredPayload(new_row, new_col, current)) {
            continue;
        }
        
        switch (i) {
            case 0:
                INFO("robot", "robot %s can move up", robot->name);
                return CMD_UP;
            case 1:
                INFO("robot", "robot %s can move down", robot->name);
                return CMD_DOWN;
            case 2:
                INFO("robot", "robot %s can move left", robot->name);
                return CMD_LEFT;
            case 3:
                INFO("robot", "robot %s can move right", robot->name);
                return CMD_RIGHT;
        }
    }

    return CMD_NOP;
}

int moveRobot(struct robot* robots, int idx, int number_of_robots, struct message* msg) {
    struct robot* robot = &robots[idx];

    if (robot_is_loaded(robot) && map_draw_default[robot->row][robot->col] == robot->current_payload) {
        return 0;
    }

    enum command cmd = msg->cmd;
    if (robot->nop_count > 5) {
        robot->nop_count = 0;
        cmd = find_movable_direction(robots, idx, number_of_robots);
    }

    int new_row = robot->row;
    int new_col = robot->col;

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
            robot->nop_count++;
            break;
        default:
            ERROR("robot", "robot %s received invalid command %d", robot->name, cmd);
            update_msg(msg, robot);
            return -1; // Invalid command
    }
    
    char new_pos = map_draw_default[new_row][new_col];

    if (isOutOfBound(new_row, new_col)) {
        ERROR("robot", "robot %s out of bound at (%d, %d)", robot->name, new_row, new_col);
        robot->nop_count++;
        update_msg(msg, robot);
        return 0;
    }

    if (isWall(new_row, new_col)) {
        ERROR("robot", "robot %s hit the wall at (%d, %d)", robot->name, new_row, new_col);
        robot->nop_count++;
        update_msg(msg, robot);
        return 0;
    }

    // Check if the new position is '0', '2', '3', etc.
    if (isAnotherCurrentPayload(new_row, new_col, robot->current_payload)) {
        ERROR("robot", "robot %s hit the wrong payload at (%d, %d)", robot->name, new_row, new_col);
        robot->nop_count++;
        update_msg(msg, robot);
        return 0;
    }

    // Check if the new position is 'A', 'B', 'C', etc. (excluding 'W' and 'S')
    if (isAnotherRequiredPayload(new_row, new_col, robot->current_payload)) {
        ERROR("robot", "robot %s required %c hit the wrong required payload at (%d, %d)", robot->name, robot->required_payload, new_row, new_col);
        robot->nop_count++;
        update_msg(msg, robot);
        return 0;
    }

    // Check another robot is already there
    for (int robotIdx = 0; robotIdx < number_of_robots; robotIdx++){
        if (robotIdx == idx) {
            continue;
        }
        struct robot* __robot = &robots[robotIdx];
        if (__robot->col == new_col && __robot->row == new_row){
            if (robot_is_unloaded(__robot)) {
                continue;
            }
            WARN("robot", "robot %s hit another robot %s at (%d, %d)", robot->name, __robot->name, new_row, new_col);
            robot->nop_count++;
            update_msg(msg, robot);
            return 0;
        }
    }
    
                    
    // Update the robot's position
    robot->row = new_row;
    robot->col = new_col;
    INFO("robot", "new position: (%d, %d), current_payload: %c", new_row, new_col, robot->current_payload);
    
    if (new_pos == robot->current_payload) {
            if (robot_is_unloaded(robot)) {
                INFO("robot", "robot %s unloaded %c at (%d, %d)", robot->name, robot->current_payload, new_row, new_col);
            } else {
                INFO("robot", "robot %s loaded %c at (%d, %d)", robot->name, robot->current_payload, new_row, new_col);
                int tmp = robot->current_payload;
                robot->current_payload = robot->required_payload;
                robot->required_payload = tmp;
            }            
    }

    update_msg(msg, robot);
    INFO("robot", "move %s to (%d, %d)", robot->name, new_row, new_col);
    return 0;
}