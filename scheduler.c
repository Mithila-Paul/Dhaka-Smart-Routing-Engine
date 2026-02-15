#include "dhaka_routing.h"

Schedule get_schedule(const char *mode)
{
    Schedule sched;
    strcpy(sched.mode, mode);

    if (strcmp(mode, "metro") == 0)
    {
        sched.interval_minutes = 5;
        sched.start_hour = 1;
        sched.end_hour = 23;
    }
    else if (strcmp(mode, "bikolpo") == 0)
    {
        sched.interval_minutes = 20;
        sched.start_hour = 7;
        sched.end_hour = 22;
    }
    else if (strcmp(mode, "uttara") == 0)
    {
        sched.interval_minutes = 10;
        sched.start_hour = 6;
        sched.end_hour = 23;
    }
    else
    {
        sched.interval_minutes = 0;
        sched.start_hour = 0;
        sched.end_hour = 24;
    }

    return sched;
}

int is_service_available(const char *mode, double time)
{
    if (strcmp(mode, "car") == 0 || strcmp(mode, "walk") == 0)
    {
        return 1; // Always available
    }

    Schedule sched = get_schedule(mode);
    int hour = ((int)time / 60) % 24;

    return (hour >= sched.start_hour && hour < sched.end_hour);
}

double get_next_departure(const char *mode, double current_time)
{
    if (strcmp(mode, "car") == 0 || strcmp(mode, "walk") == 0)
    {
        return current_time; // Depart immediately
    }

    Schedule sched = get_schedule(mode);

    if (!is_service_available(mode, current_time))
    {
        int current_hour = ((int)current_time / 60) % 24;
        if (current_hour >= sched.end_hour || current_hour < sched.start_hour)
        {
            int wait_hours = (24 + sched.start_hour - current_hour) % 24;
            double start_time = current_time + wait_hours * 60;
            start_time = ((int)(start_time / 60)) * 60; // Round to hour
            return start_time;
        }
    }

    double mins_since_start = current_time - (sched.start_hour * 60);
    if (mins_since_start < 0)
        mins_since_start += 24 * 60;

    int intervals_passed = (int)(mins_since_start / sched.interval_minutes);
    double next_departure = sched.start_hour * 60 + (intervals_passed + 1) * sched.interval_minutes;

    int day_offset = ((int)current_time / (24 * 60)) * (24 * 60);
    next_departure += day_offset;

    if (next_departure < current_time)
    {
        next_departure += sched.interval_minutes;
    }

    return next_departure;
}

double get_mode_speed(const char *mode)
{
    if (strcmp(mode, "car") == 0)
        return 20.0;
    if (strcmp(mode, "metro") == 0)
        return 15.0;
    if (strcmp(mode, "bikolpo") == 0)
        return 10.0;
    if (strcmp(mode, "uttara") == 0)
        return 12.0;
    if (strcmp(mode, "walk") == 0)
        return 2.0;
    return 20.0;
}

double get_travel_time(double distance, const char *mode)
{
    double speed = get_mode_speed(mode);
    return (distance / speed) * 60.0; 
}
