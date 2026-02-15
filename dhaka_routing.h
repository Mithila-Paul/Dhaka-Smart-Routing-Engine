#ifndef DHAKA_ROUTING_H
#define DHAKA_ROUTING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <time.h>

// Constants
#define MAX_NODES 60000
#define MAX_EDGES 150000
#define MAX_LINE 10000
#define INF DBL_MAX
#define EARTH_RADIUS 6371.0
#define WALKING_SPEED 2.0

// Structure definitions
typedef struct {
    double lat;
    double lon;
} Coordinate;

typedef struct {
    int id;
    Coordinate coord;
    char type[50];
} Node;

typedef struct {
    int from;
    int to;
    double distance;
    char mode[50];
} Edge;

typedef struct {
    int node_id;
    double cost;
    double distance;
    double time;
    int prev;
    char prev_mode[50];
} PathInfo;

typedef struct {
    int *path;
    char **modes;
    double *distances;
    double *costs;
    double *times;
    int length;
    double total_distance;
    double total_cost;
    double total_time;
} Route;

typedef struct {
    char mode[50];
    int interval_minutes;
    int start_hour;
    int end_hour;
} Schedule;

// Global variables
extern Node *nodes;
extern Edge *edges;
extern int node_count;
extern int edge_count;

// Parser functions (parser.c)
void load_roadmap(const char *filename);
void load_routemap(const char *filename, const char *mode);

// Graph functions (graph.c)
void init_graph();
void free_graph();
int find_or_add_node(Coordinate coord, const char *type);
int find_nearest_node(Coordinate coord);
void add_edge(int from, int to, double distance, const char *mode);

// Dijkstra functions (dijkstra.c)
Route dijkstra_shortest_distance(int start, int end);
Route dijkstra_cheapest_route(int start, int end, int use_metro, int use_bikolpo, int use_uttara);
Route dijkstra_cheapest_with_time(int start, int end, double start_time, 
                                   int use_metro, int use_bikolpo, int use_uttara);
Route dijkstra_fastest_route(int start, int end, double start_time);
Route dijkstra_cheapest_with_deadline(int start, int end, double start_time, double deadline,
                                       int use_metro, int use_bikolpo, int use_uttara);

// Scheduler functions (scheduler.c)
double get_next_departure(const char *mode, double current_time);
double get_travel_time(double distance, const char *mode);
double get_mode_speed(const char *mode);
int is_service_available(const char *mode, double time);

// Output functions (output.c)
void print_route_output(Route route, Coordinate src, Coordinate dst, int problem_num,
                       double start_time, double deadline);
void create_kml_file(Route route, Coordinate src, Coordinate dst, const char *filename);

// Utility functions (utils.c)
double haversine_distance(Coordinate c1, Coordinate c2);
double get_mode_cost(const char *mode);
char* format_time(double minutes);
double parse_time(const char *time_str);
void free_route(Route *route);

#endif // DHAKA_ROUTING_H
