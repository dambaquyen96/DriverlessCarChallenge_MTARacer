#ifndef UTILS_H
#define UTILS_H

#include <opencv2/core/core.hpp>

struct _parameter_t
{
    double kp;
    double ki;
    double kd;
    int motor_rpm;
    bool is_save;

    _parameter_t()
    {
        kp = 0.01;
        ki = 0.01;
        kd = 0.01;
        motor_rpm = 500;
        is_save = false;
    }
};


struct list_node_s
{
    cv::Mat data;
    list_node_s* next;
};

void
Enqueue(cv::Mat &img,
        list_node_s** queue_head,
        list_node_s** queue_tail,
        int &queue_size);

list_node_s*
Dequeue(list_node_s** queue_head,
        list_node_s** queue_tail,
        int &queue_size);


float
get_center_dist( cv::Rect center, cv::Point vp );


void
get_argv( int argc,
          char** argv,
          _parameter_t &p);

void help();

/////////  PID  data type  //////////////
struct PID_t
{
    double windup_guard;
    double proportional_gain;
    double integral_gain;
    double derivative_gain;
    double prev_error;
    double int_error;
    double control;

    PID_t()
    {
        windup_guard = 0;
        proportional_gain = 0;
        integral_gain = 0;
        derivative_gain = 0;
        prev_error = 0;
        int_error = 0;
        control = 0;
    }
};

void
pid_update(PID_t* pid, double curr_error, double dt);



#endif // UTILS_H
