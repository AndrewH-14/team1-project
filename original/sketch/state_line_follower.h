/**
 * Header for all line follower state.
 */
#ifndef STATE_LINE_FOLLOWER_H
#define STATE_LINE_FOLLOWER_H

enum LineFollowingStates {
    SEN1_IN_SEN2_IN,
    SEN1_OUT_SEN2_IN,
    SEN1_IN_SEN2_OUT,
    SEN1_OUT_SEN2_OUT
};

bool state_line_follower_start(void);

#endif // STATE_LINE_FOLLOWER_H
