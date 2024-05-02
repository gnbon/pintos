#include <stdio.h>
#include <stdlib.h>

#include "projects/automated_warehouse/aw_message.h"

#define DEBUG(...) printf(__VA_ARGS__)

int initialize_message_boxes(int num_robots) {
        NUM_ROBOTS = num_robots;

        boxes_from_central_control_node = malloc(sizeof(struct messsage_box) * num_robots);
        if (boxes_from_central_control_node == NULL) {
                printf("Memory allocation failed\n");
                return -1;
        }
        for (int i = 0; i < num_robots; i++) {
                boxes_from_central_control_node[i].dirtyBit = 1;
        }

        boxes_from_robots = malloc(sizeof(struct messsage_box) * num_robots);
        if (boxes_from_robots == NULL) {
                printf("Memory allocation failed\n");
                return -1;
        }

        for (int i = 0; i < num_robots; i++) {
                boxes_from_robots[i].dirtyBit = 0;
        }

        return 0;
}

int send_message_to_central_control_node(int robotIdx, struct message* msg) {
        DEBUG("send_message_to_central_control_node(%d)\n", robotIdx);
        if (robotIdx < 0 || robotIdx >= NUM_ROBOTS) {
                printf("Invalid robot index: %d\n", robotIdx);
                return -1;
        }

        if (boxes_from_robots[robotIdx].dirtyBit == 1) {
                printf("Message box is already written by others\n");
                return -1;
        }

        boxes_from_robots[robotIdx].msg.row = msg->row;
        boxes_from_robots[robotIdx].msg.col = msg->col;
        boxes_from_robots[robotIdx].msg.current_payload = msg->current_payload;
        boxes_from_robots[robotIdx].msg.required_payload = msg->required_payload;
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

        boxes_from_central_control_node[robotIdx].msg.cmd = msg->cmd;
        boxes_from_central_control_node[robotIdx].dirtyBit = 1;

        return 0;
}

int recv_message_from_central_control_node(int robotIdx, struct message* msg) {
        DEBUG("recv_message_from_central_control_node(%d)\n", robotIdx);
        if (robotIdx < 0 || robotIdx >= NUM_ROBOTS) {
                printf("Invalid robot index: %d\n", robotIdx);
                return -1;
        }

        if (boxes_from_central_control_node[robotIdx].dirtyBit == 0) {
                printf("Message box is not written yet\n");
                return -1;
        }

        msg->cmd = boxes_from_central_control_node[robotIdx].msg.cmd;
        boxes_from_central_control_node[robotIdx].dirtyBit = 0;
        DEBUG("recv_message_from_central_control_node ends(%d)\n", robotIdx);
        return 0;
}

int recv_message_from_robot(int robotIdx, struct message* msg) {
        DEBUG("recv_message_from_robot(%d)\n", robotIdx);
        DEBUG("msg->row: %d, msg->col: %d, msg->current_payload: %d, msg->required_payload: %d\n", msg->row, msg->col, msg->current_payload, msg->required_payload);
        if (robotIdx < 0 || robotIdx >= NUM_ROBOTS) {
                printf("Invalid robot index: %d\n", robotIdx);
                return -1;
        }

        if (boxes_from_robots[robotIdx].dirtyBit == 0) {
                printf("Message box is not written yet\n");
                return -1;
        }

        msg->row = boxes_from_robots[robotIdx].msg.row;
        msg->col = boxes_from_robots[robotIdx].msg.col;
        msg->current_payload = boxes_from_robots[robotIdx].msg.current_payload;
        msg->required_payload = boxes_from_robots[robotIdx].msg.required_payload;
        boxes_from_robots[robotIdx].dirtyBit = 0;
        DEBUG("recv_message_from_robot ends(%d)\n", robotIdx);

        return 0;
}

void set_cmd(struct message* msg, enum command cmd) {
        msg->cmd = cmd;
}