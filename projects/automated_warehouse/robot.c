#include <stdio.h>

#include "projects/automated_warehouse/aw_debug.h"
#include "projects/automated_warehouse/robot.h"
#include "projects/automated_warehouse/aw_message.h"
#include "projects/automated_warehouse/aw_manager.h"

/**
 * A function setting up robot structure
 */
void setRobot(struct robot* _robot, const char* name, int row, int col, int required_payload, int current_payload) {
    if (_robot != NULL) {
        _robot->name = name;
    }
    _robot->row = row;
    _robot->col = col;
    _robot->required_payload = required_payload;
    _robot->current_payload = current_payload;
    INFO("robot", "robot %s is set at (%d, %d), required dock: %c, required dock: %c current_payload: %d", name, row, col, LOWER(required_payload), UPPER(required_payload), current_payload);
}

bool robot_is_loaded(struct robot* robot) {
    return robot->required_payload > robot->required_payload;
}

bool robot_is_unloaded(struct robot* robot) {
    bool is_pos_alpha = map_draw_default[robot->row][robot->col] >= 'A' && map_draw_default[robot->row][robot->col] <= 'Z';
    bool is_pos_current = robot->required_payload == map_draw_default[robot->row][robot->col];
    return is_pos_alpha && is_pos_current;
}

void update_msg(struct message* msg, struct robot* robot) {
    msg->row = robot->row;
    msg->col = robot->col;
    msg->current_payload = robot->current_payload;
    msg->required_payload = robot->required_payload;
    INFO("robot", "update msg row: %d, col: %d, current_payload: %d, required_payload: %d", msg->row, msg->col, msg->current_payload, msg->required_payload);
}


bool isOutOfBound(int row, int col) {
    return row < ROW_MIN || row > ROW_MAX || col < COL_MIN || col > COL_MAX;
}

bool isWall(int row, int col) {
    return map_draw_default[row][col] == 'X';
}

bool isAnotherRequiredLoad(int row, int col, int current_payload) {
    return map_draw_default[row][col] >= '0' && map_draw_default[row][col] <= '9' && 
        map_draw_default[row][col] != current_payload;
}

bool isAnotherRequiredDock(int row, int col, int current_payload) {
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
    char current = robot->required_payload;
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
        
        // if (isAnotherRequiredPayload(new_row, new_col, current)) {
        //     continue;
        // }

        // if (isAnotherRequiredPayload(new_row, new_col, current)) {
        //     continue;
        // }
        
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

int moveRobot(struct robot* robots, int robot_idx, int number_of_robots, struct message* msg) {
    struct robot* robot = &robots[robot_idx];
    update_msg(msg, robot);

    if (robot_is_loaded(robot) && map_draw_default[robot->row][robot->col] == robot->required_payload) {
        return 0;
    }

    enum command cmd = msg->cmd;
    // if (robot->nop_count > 5) {
    //     robot->nop_count = 0;
    //     cmd = find_movable_direction(robots, idx, number_of_robots);
    // }

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
            return -1; // Invalid command
    }
    
    char new_pos = map_draw_default[new_row][new_col];

    if (is_out_of_bound(new_row, new_col)) {
        ERROR("robot", "error(is_out_of_bound) row: %d, col: %d", new_row, new_col);
        return -1;
    }

    // if (is_wall(new_row, new_col)) {
    //     ERROR("robot", "error(is_wall) row: %d, col: %d", new_row, new_col);
    //     ERROR("robot", "map[%d][%d] = %c", new_row, new_col, map_draw_default[new_row][new_col]);
    //     return -1;
    // }

    if (is_another_payload(new_row, new_col, robot->required_payload, robot->current_payload)) {
        ERROR("robot", "error(is_another_payload) row: %d, col: %d", new_row, new_col);
        return -1;
    }

    // if (is_hit_robot(new_row, new_col, robots, robot_idx)) {
    //     ERROR("robot", "error(is_hit_robot) row: %d, col: %d", new_row, new_col);
    //     return -1;
    // }
    
                    
    // Update the robot's position
    robot->row = new_row;
    robot->col = new_col;
    INFO("robot", "new position: (%d, %d), current_payload: %d", new_row, new_col, robot->current_payload);
    
    if (new_pos == robot->required_payload) {
            if (robot_is_unloaded(robot)) {
                INFO("robot", "robot %s unloaded %c at (%d, %d)", robot->name, robot->required_payload, new_row, new_col);
            } else {
                INFO("robot", "robot %s loaded %c at (%d, %d)", robot->name, robot->required_payload, new_row, new_col);
                int tmp = robot->required_payload;
                robot->required_payload = robot->required_payload;
                robot->required_payload = tmp;
            }            
    }

    update_msg(msg, robot);
    INFO("robot", "move %s to (%d, %d)", robot->name, new_row, new_col);
    return 0;
}