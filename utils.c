#include "dhaka_routing.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// haversine
double haversine_distance(Coordinate c1, Coordinate c2)
{
    double lat1 = c1.lat * M_PI / 180.0;
    double lon1 = c1.lon * M_PI / 180.0;
    double lat2 = c2.lat * M_PI / 180.0;
    double lon2 = c2.lon * M_PI / 180.0;

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return EARTH_RADIUS * c;
}

double get_mode_cost(const char *mode)
{
    if (strcmp(mode, "car") == 0)
        return 20.0;
    if (strcmp(mode, "metro") == 0)
        return 5.0;
    if (strcmp(mode, "bikolpo") == 0)
        return 7.0;
    if (strcmp(mode, "uttara") == 0)
        return 10.0;
    if (strcmp(mode, "walk") == 0)
        return 0.0;
    return 20.0;
}

//  HH:MM AM/PM
char *format_time(double minutes)
{
    static char buffer[20];
    int total_mins = (int)minutes;
    int hours = (total_mins / 60) % 24;
    int mins = total_mins % 60;

    const char *period = hours >= 12 ? "PM" : "AM";
    if (hours > 12)
        hours -= 12;
    if (hours == 0)
        hours = 12;

    sprintf(buffer, "%d:%02d %s", hours, mins, period);
    return buffer;
}

double parse_time(const char *time_str)
{
    int hour, minute;
    char period[3];

    sscanf(time_str, "%d:%d %s", &hour, &minute, period);

    if (strcmp(period, "PM") == 0 && hour != 12)
    {
        hour += 12;
    }
    else if (strcmp(period, "AM") == 0 && hour == 12)
    {
        hour = 0;
    }

    return hour * 60.0 + minute;
}

void free_route(Route *route)
{
    if (route->path)
        free(route->path);
    if (route->modes)
    {
        for (int i = 0; i < route->length; i++)
        {
            if (route->modes[i])
                free(route->modes[i]);
        }
        free(route->modes);
    }
    if (route->distances)
        free(route->distances);
    if (route->costs)
        free(route->costs);
    if (route->times)
        free(route->times);
}
