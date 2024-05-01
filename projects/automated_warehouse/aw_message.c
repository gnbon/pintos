#include <stdio.h>
#include <stdlib.h>

#include "projects/automated_warehouse/aw_message.h"

int initialize_message_boxes(int num_robots) {
        NUM_ROBOTS = num_robots;

        boxes_from_central_control_node = malloc(sizeof(struct messsage_box) * num_robots);
        if (boxes_from_central_control_node == NULL) {
                printf("Memory allocation failed\n");
                return -1;
        }

        boxes_from_robots = malloc(sizeof(struct messsage_box) * num_robots);
        if (boxes_from_robots == NULL) {
                printf("Memory allocation failed\n");
                return -1;
        }

        for (int i = 0; i < num_robots; i++) {
                boxes_from_central_control_node[i].dirtyBit = 0;
                boxes_from_robots[i].dirtyBit = 0;
        }

        return 0;
}

int send_message_to_central_control_node(int robotIdx, struct message* msg) {
        if (robotIdx < 0 || robotIdx >= NUM_ROBOTS) {
                printf("Invalid robot index: %d\n", robotIdx);
                return -1;
        }

        if (boxes_from_robots[robotIdx].dirtyBit == 1) {
                printf("Message box is already written by others\n");
                return -1;
        }

        boxes_from_robots[robotIdx].msg = *msg;
        boxes_from_robots[robotIdx].dirtyBit = 1;

        return 0;
}

int send_message_to_robot(int robotIdx, struct message* msg) {
        if (robotIdx < 0 || robotIdx >= NUM_ROBOTS) {
                printf("Invalid robot index: %d\n", robotIdx);
                return -1;
        }

        if (boxes_from_central_control_node[robotIdx].dirtyBit == 1) {
                printf("Message box is already written by others\n");
                return -1;
        }

        boxes_from_central_control_node[robotIdx].msg = *msg;
        boxes_from_central_control_node[robotIdx].dirtyBit = 1;

        return 0;
}

int recv_message_from_central_control_node(int robotIdx, struct message* msg) {
        if (robotIdx < 0 || robotIdx >= NUM_ROBOTS) {
                printf("Invalid robot index: %d\n", robotIdx);
                return -1;
        }

        if (boxes_from_central_control_node[robotIdx].dirtyBit == 0) {
                printf("Message box is not written yet\n");
                return -1;
        }

        *msg = boxes_from_central_control_node[robotIdx].msg;
        boxes_from_central_control_node[robotIdx].dirtyBit = 0;

        return 0;
}

int recv_message_from_robot(int robotIdx, struct message* msg) {
        if (robotIdx < 0 || robotIdx >= NUM_ROBOTS) {
                printf("Invalid robot index: %d\n", robotIdx);
                return -1;
        }

        if (boxes_from_robots[robotIdx].dirtyBit == 0) {
                printf("Message box is not written yet\n");
                return -1;
        }

        *msg = boxes_from_robots[robotIdx].msg;
        boxes_from_robots[robotIdx].dirtyBit = 0;

        return 0;
}

void set_cmd(struct message* msg, enum command cmd) {
        msg->cmd = cmd;
}