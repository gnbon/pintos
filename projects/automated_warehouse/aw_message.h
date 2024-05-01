#ifndef _PROJECTS_PROJECT1_AW_MESSAGE_H__
#define _PROJECTS_PROJECT1_AW_MESSAGE_H__

enum command {
    /** move up */
    CMD_UP = 0,
    /** move down */
    CMD_DOWN,
    /** move left */
    CMD_LEFT,
    /** move right */
    CMD_RIGHT,
    /** do nothing */
    CMD_NOP
};

/**
 * For easy to implement, combine robot and central control node message
 * If you want to modify message structure, don't split it
 */
struct message {
    //
    // To central control node
    //
    /** current row of robot */
    int row;
    /** current column of robot */
    int col;
    /** current payload of robot */
    int current_payload;
    /** required paylod of robot */
    int required_payload;

    //
    // To robots
    //
    /** next command for robot */
    enum command cmd;
};

/** 
 * Simple message box which can receive only one message from sender
*/
struct messsage_box {
    /** check if the message was written by others */
    int dirtyBit;
    /** stored message */
    struct message msg;
};

/** message boxes from central control node to each robot */
extern struct messsage_box* boxes_from_central_control_node;
/** message boxes from robots to central control node */
extern struct messsage_box* boxes_from_robots;

int NUM_ROBOTS;

/** initialize message boxes */
int initialize_message_boxes(int num_robots);
/** send message from robot to central control node */
int send_message_to_central_control_node(int robotIdx, struct message* msg);
/** send message from central control node to robot */
int send_message_to_robot(int robotIdx, struct message* msg);
/** receive message from robot */
int recv_message_from_robot(int robotIdx, struct message* msg);
/** receive message from central control node */
int recv_message_from_central_control_node(int robotIdx, struct message* msg);

void set_cmd(struct message* msg, enum command cmd);

#endif