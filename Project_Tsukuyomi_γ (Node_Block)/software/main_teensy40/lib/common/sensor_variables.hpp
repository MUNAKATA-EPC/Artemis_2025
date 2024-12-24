#ifndef _SENSOR_VARIABLES_HPP
#define _SENSOR_VARIABLES_HPP

// omnidirectional camera data
extern int cam_ball_deg;
extern int cam_ball_distance;
extern int cam_goal_yellow_deg;
extern int cam_goal_yellow_distance;
extern int cam_goal_blue_deg;
extern int cam_goal_blue_distance;

// front camera data
extern int fcam_ball_deg;
extern int fcam_ball_distance;
extern int fcam_goal_yellow_deg;
extern int fcam_goal_yellow_distance;
extern int fcam_goal_blue_deg;
extern int fcam_goal_blue_distance;

// back camera data;
extern int bcam_ball_deg;
extern int bcam_ball_distance;
extern int bcam_goal_yellow_deg;
extern int bcam_goal_yellow_distance;
extern int bcam_goal_blue_deg;
extern int bcam_goal_blue_distance;

// gyro sensor
extern int gyro_deg;

// line sensor
extern int line_data[16];
extern int line_deg;

// touch sensors
extern int touch_front;
extern int touch_back;

extern int process_type;
extern int is_processing;

#endif