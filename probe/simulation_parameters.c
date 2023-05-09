#include "simulation_parameters.h"


unsigned long long interval = 1000; // Interrupt interval in microseconds

int disable_crash_detection = 0;  // If 1, disable crash detection

float pump_acceleration = 0.2f;  // Acceleration of a pump
int pump_actuator_delay = 50;     // Delay of the pump actuator in miliseconds

// Luke position in space
float luke_pos_x = -130.0f;
float luke_pos_y = -50.0f;
float luke_pos_z = 0.0f;

// Luke speed
float luke_speed_x = 0.0f;
float luke_speed_y = 0.0f;
float luke_speed_z = 0.0f;

// Luke acceleration
float luke_accel_x = 0.0f;
float luke_accel_y = 0.0f;
float luke_accel_z = 0.0f;
