#ifndef _ENGELLINE_HPP
#define _ENGELLINE_HPP

extern void line_init();
extern void line_process();
extern void reset_line_process();
extern bool is_line_detected();
extern bool is_previous_line_detected();
extern bool is_line_evacuation();
extern int get_count_of_detected_sensor();

extern int line_evacuation_deg;

#endif