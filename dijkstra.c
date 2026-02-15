#include "dhaka_routing.h"

// Problem 1: Shortest distance (car only)
Route dijkstra_shortest_distance(int start, int end)
{
    PathInfo *info = (PathInfo *)malloc(node_count * sizeof(PathInfo));
    int *visited = (int *)calloc(node_count, sizeof(int));

    for (int i = 0; i < node_count; i++)
    {
        info[i].node_id = i;
        info[i].distance = INF;
        info[i].cost = 0.0;
        info[i].time = 0.0;
        info[i].prev = -1;
        strcpy(info[i].prev_mode, "");
    }
    info[start].distance = 0.0;

    // Dijkstra
    for (int count = 0; count < node_count; count++)
    {
        int u = -1;
        double min_dist = INF;

        for (int i = 0; i < node_count; i++)
        {
            if (!visited[i] && info[i].distance < min_dist)
            {
                min_dist = info[i].distance;
                u = i;
            }
        }

        if (u == -1 || u == end)
            break;
        visited[u] = 1;

        // freeiing edges
        for (int i = 0; i < edge_count; i++)
        {
            if (edges[i].from == u && strcmp(edges[i].mode, "car") == 0)
            {
                int v = edges[i].to;
                double new_dist = info[u].distance + edges[i].distance;

                if (new_dist < info[v].distance)
                {
                    info[v].distance = new_dist;
                    info[v].prev = u;
                    strcpy(info[v].prev_mode, edges[i].mode);
                }
            }
        }
    }

    // Reconstruction path
    Route route;
    route.path = (int *)malloc(node_count * sizeof(int));
    route.modes = (char **)malloc(node_count * sizeof(char *));
    route.distances = (double *)malloc(node_count * sizeof(double));
    route.costs = (double *)malloc(node_count * sizeof(double));
    route.times = (double *)malloc(node_count * sizeof(double));
    route.length = 0;
    route.total_distance = info[end].distance;
    route.total_cost = info[end].distance * get_mode_cost("car");
    route.total_time = 0.0;

    if (info[end].prev != -1)
    {
        int curr = end;
        while (curr != -1)
        {
            route.path[route.length] = curr;
            route.modes[route.length] = (char *)malloc(50);
            strcpy(route.modes[route.length], info[curr].prev_mode);
            route.length++;
            curr = info[curr].prev;
        }

        // Reversing path
        for (int i = 0; i < route.length / 2; i++)
        {
            int temp = route.path[i];
            route.path[i] = route.path[route.length - 1 - i];
            route.path[route.length - 1 - i] = temp;

            char *temp_mode = route.modes[i];
            route.modes[i] = route.modes[route.length - 1 - i];
            route.modes[route.length - 1 - i] = temp_mode;
        }
    }

    free(info);
    free(visited);
    return route;
}

// Problems 2 & 3: Cheapest route
Route dijkstra_cheapest_route(int start, int end, int use_metro, int use_bikolpo, int use_uttara)
{
    PathInfo *info = (PathInfo *)malloc(node_count * sizeof(PathInfo));
    int *visited = (int *)calloc(node_count, sizeof(int));

    for (int i = 0; i < node_count; i++)
    {
        info[i].node_id = i;
        info[i].cost = INF;
        info[i].distance = 0.0;
        info[i].time = 0.0;
        info[i].prev = -1;
        strcpy(info[i].prev_mode, "");
    }
    info[start].cost = 0.0;

    // Dijkstra
    for (int count = 0; count < node_count; count++)
    {
        int u = -1;
        double min_cost = INF;

        for (int i = 0; i < node_count; i++)
        {
            if (!visited[i] && info[i].cost < min_cost)
            {
                min_cost = info[i].cost;
                u = i;
            }
        }

        if (u == -1 || u == end)
            break;
        visited[u] = 1;

        for (int i = 0; i < edge_count; i++)
        {
            if (edges[i].from == u)
            {
                // mode
                if (strcmp(edges[i].mode, "metro") == 0 && !use_metro)
                    continue;
                if (strcmp(edges[i].mode, "bikolpo") == 0 && !use_bikolpo)
                    continue;
                if (strcmp(edges[i].mode, "uttara") == 0 && !use_uttara)
                    continue;

                int v = edges[i].to;
                double edge_cost = edges[i].distance * get_mode_cost(edges[i].mode);
                double new_cost = info[u].cost + edge_cost;

                if (new_cost < info[v].cost)
                {
                    info[v].cost = new_cost;
                    info[v].distance = info[u].distance + edges[i].distance;
                    info[v].prev = u;
                    strcpy(info[v].prev_mode, edges[i].mode);
                }
            }
        }
    }

    Route route;
    route.path = (int *)malloc(node_count * sizeof(int));
    route.modes = (char **)malloc(node_count * sizeof(char *));
    route.distances = (double *)malloc(node_count * sizeof(double));
    route.costs = (double *)malloc(node_count * sizeof(double));
    route.times = (double *)malloc(node_count * sizeof(double));
    route.length = 0;
    route.total_cost = info[end].cost;
    route.total_distance = info[end].distance;
    route.total_time = 0.0;

    if (info[end].prev != -1)
    {
        int curr = end;
        while (curr != -1)
        {
            route.path[route.length] = curr;
            route.modes[route.length] = (char *)malloc(50);
            strcpy(route.modes[route.length], info[curr].prev_mode);
            route.length++;
            curr = info[curr].prev;
        }

        for (int i = 0; i < route.length / 2; i++)
        {
            int temp = route.path[i];
            route.path[i] = route.path[route.length - 1 - i];
            route.path[route.length - 1 - i] = temp;

            char *temp_mode = route.modes[i];
            route.modes[i] = route.modes[route.length - 1 - i];
            route.modes[route.length - 1 - i] = temp_mode;
        }
    }

    free(info);
    free(visited);
    return route;
}

// Problem 4: Cheapest route with time scheduling
Route dijkstra_cheapest_with_time(int start, int end, double start_time,
                                  int use_metro, int use_bikolpo, int use_uttara)
{
    PathInfo *info = (PathInfo *)malloc(node_count * sizeof(PathInfo));
    int *visited = (int *)calloc(node_count, sizeof(int));

    for (int i = 0; i < node_count; i++)
    {
        info[i].node_id = i;
        info[i].cost = INF;
        info[i].distance = 0.0;
        info[i].time = INF;
        info[i].prev = -1;
        strcpy(info[i].prev_mode, "");
    }
    info[start].cost = 0.0;
    info[start].time = start_time;

    for (int count = 0; count < node_count; count++)
    {
        int u = -1;
        double min_cost = INF;

        for (int i = 0; i < node_count; i++)
        {
            if (!visited[i] && info[i].cost < min_cost)
            {
                min_cost = info[i].cost;
                u = i;
            }
        }

        if (u == -1 || u == end)
            break;
        visited[u] = 1;

        for (int i = 0; i < edge_count; i++)
        {
            if (edges[i].from == u)
            {
                if (strcmp(edges[i].mode, "metro") == 0 && !use_metro)
                    continue;
                if (strcmp(edges[i].mode, "bikolpo") == 0 && !use_bikolpo)
                    continue;
                if (strcmp(edges[i].mode, "uttara") == 0 && !use_uttara)
                    continue;

                int v = edges[i].to;

                double departure_time = get_next_departure(edges[i].mode, info[u].time);
                double travel_time = get_travel_time(edges[i].distance, edges[i].mode);
                double arrival_time = departure_time + travel_time;

                double edge_cost = edges[i].distance * get_mode_cost(edges[i].mode);
                double new_cost = info[u].cost + edge_cost;

                if (new_cost < info[v].cost ||
                    (new_cost == info[v].cost && arrival_time < info[v].time))
                {
                    info[v].cost = new_cost;
                    info[v].distance = info[u].distance + edges[i].distance;
                    info[v].time = arrival_time;
                    info[v].prev = u;
                    strcpy(info[v].prev_mode, edges[i].mode);
                }
            }
        }
    }

    Route route;
    route.path = (int *)malloc(node_count * sizeof(int));
    route.modes = (char **)malloc(node_count * sizeof(char *));
    route.distances = (double *)malloc(node_count * sizeof(double));
    route.costs = (double *)malloc(node_count * sizeof(double));
    route.times = (double *)malloc(node_count * sizeof(double));
    route.length = 0;
    route.total_cost = info[end].cost;
    route.total_distance = info[end].distance;
    route.total_time = info[end].time - start_time;

    if (info[end].prev != -1)
    {
        int curr = end;
        while (curr != -1)
        {
            route.path[route.length] = curr;
            route.modes[route.length] = (char *)malloc(50);
            strcpy(route.modes[route.length], info[curr].prev_mode);
            route.times[route.length] = info[curr].time;
            route.length++;
            curr = info[curr].prev;
        }

        for (int i = 0; i < route.length / 2; i++)
        {
            int temp = route.path[i];
            route.path[i] = route.path[route.length - 1 - i];
            route.path[route.length - 1 - i] = temp;

            char *temp_mode = route.modes[i];
            route.modes[i] = route.modes[route.length - 1 - i];
            route.modes[route.length - 1 - i] = temp_mode;

            double temp_time = route.times[i];
            route.times[i] = route.times[route.length - 1 - i];
            route.times[route.length - 1 - i] = temp_time;
        }
    }

    free(info);
    free(visited);
    return route;
}

// Problem 5: Fastest route
Route dijkstra_fastest_route(int start, int end, double start_time)
{
    PathInfo *info = (PathInfo *)malloc(node_count * sizeof(PathInfo));
    int *visited = (int *)calloc(node_count, sizeof(int));

    for (int i = 0; i < node_count; i++)
    {
        info[i].node_id = i;
        info[i].time = INF;
        info[i].distance = 0.0;
        info[i].cost = 0.0;
        info[i].prev = -1;
        strcpy(info[i].prev_mode, "");
    }
    info[start].time = start_time;

    for (int count = 0; count < node_count; count++)
    {
        int u = -1;
        double min_time = INF;

        for (int i = 0; i < node_count; i++)
        {
            if (!visited[i] && info[i].time < min_time)
            {
                min_time = info[i].time;
                u = i;
            }
        }

        if (u == -1 || u == end)
            break;
        visited[u] = 1;

        for (int i = 0; i < edge_count; i++)
        {
            if (edges[i].from == u)
            {
                int v = edges[i].to;

                double departure_time = get_next_departure(edges[i].mode, info[u].time);
                double travel_time = get_travel_time(edges[i].distance, edges[i].mode);
                double arrival_time = departure_time + travel_time;

                if (arrival_time < info[v].time)
                {
                    info[v].time = arrival_time;
                    info[v].distance = info[u].distance + edges[i].distance;
                    info[v].cost = info[u].cost + edges[i].distance * get_mode_cost(edges[i].mode);
                    info[v].prev = u;
                    strcpy(info[v].prev_mode, edges[i].mode);
                }
            }
        }
    }

    Route route;
    route.path = (int *)malloc(node_count * sizeof(int));
    route.modes = (char **)malloc(node_count * sizeof(char *));
    route.distances = (double *)malloc(node_count * sizeof(double));
    route.costs = (double *)malloc(node_count * sizeof(double));
    route.times = (double *)malloc(node_count * sizeof(double));
    route.length = 0;
    route.total_time = info[end].time - start_time;
    route.total_distance = info[end].distance;
    route.total_cost = info[end].cost;

    if (info[end].prev != -1)
    {
        int curr = end;
        while (curr != -1)
        {
            route.path[route.length] = curr;
            route.modes[route.length] = (char *)malloc(50);
            strcpy(route.modes[route.length], info[curr].prev_mode);
            route.times[route.length] = info[curr].time;
            route.length++;
            curr = info[curr].prev;
        }

        for (int i = 0; i < route.length / 2; i++)
        {
            int temp = route.path[i];
            route.path[i] = route.path[route.length - 1 - i];
            route.path[route.length - 1 - i] = temp;

            char *temp_mode = route.modes[i];
            route.modes[i] = route.modes[route.length - 1 - i];
            route.modes[route.length - 1 - i] = temp_mode;

            double temp_time = route.times[i];
            route.times[i] = route.times[route.length - 1 - i];
            route.times[route.length - 1 - i] = temp_time;
        }
    }

    free(info);
    free(visited);
    return route;
}

// Problem 6: Cheapest route with deadline
Route dijkstra_cheapest_with_deadline(int start, int end, double start_time, double deadline,
                                      int use_metro, int use_bikolpo, int use_uttara)
{
    PathInfo *info = (PathInfo *)malloc(node_count * sizeof(PathInfo));
    int *visited = (int *)calloc(node_count, sizeof(int));

    for (int i = 0; i < node_count; i++)
    {
        info[i].node_id = i;
        info[i].cost = INF;
        info[i].distance = 0.0;
        info[i].time = INF;
        info[i].prev = -1;
        strcpy(info[i].prev_mode, "");
    }
    info[start].cost = 0.0;
    info[start].time = start_time;

    for (int count = 0; count < node_count; count++)
    {
        int u = -1;
        double min_cost = INF;

        for (int i = 0; i < node_count; i++)
        {
            if (!visited[i] && info[i].cost < min_cost && info[i].time <= deadline)
            {
                min_cost = info[i].cost;
                u = i;
            }
        }

        if (u == -1 || u == end)
            break;
        visited[u] = 1;

        for (int i = 0; i < edge_count; i++)
        {
            if (edges[i].from == u)
            {
                if (strcmp(edges[i].mode, "metro") == 0 && !use_metro)
                    continue;
                if (strcmp(edges[i].mode, "bikolpo") == 0 && !use_bikolpo)
                    continue;
                if (strcmp(edges[i].mode, "uttara") == 0 && !use_uttara)
                    continue;

                int v = edges[i].to;

                double departure_time = get_next_departure(edges[i].mode, info[u].time);
                double travel_time = get_travel_time(edges[i].distance, edges[i].mode);
                double arrival_time = departure_time + travel_time;

                if (arrival_time > deadline)
                    continue;

                double edge_cost = edges[i].distance * get_mode_cost(edges[i].mode);
                double new_cost = info[u].cost + edge_cost;

                if (new_cost < info[v].cost)
                {
                    info[v].cost = new_cost;
                    info[v].distance = info[u].distance + edges[i].distance;
                    info[v].time = arrival_time;
                    info[v].prev = u;
                    strcpy(info[v].prev_mode, edges[i].mode);
                }
            }
        }
    }

    Route route;
    route.path = (int *)malloc(node_count * sizeof(int));
    route.modes = (char **)malloc(node_count * sizeof(char *));
    route.distances = (double *)malloc(node_count * sizeof(double));
    route.costs = (double *)malloc(node_count * sizeof(double));
    route.times = (double *)malloc(node_count * sizeof(double));
    route.length = 0;
    route.total_cost = info[end].cost;
    route.total_distance = info[end].distance;
    route.total_time = info[end].time - start_time;

    if (info[end].prev != -1 && info[end].time <= deadline)
    {
        int curr = end;
        while (curr != -1)
        {
            route.path[route.length] = curr;
            route.modes[route.length] = (char *)malloc(50);
            strcpy(route.modes[route.length], info[curr].prev_mode);
            route.times[route.length] = info[curr].time;
            route.length++;
            curr = info[curr].prev;
        }

        for (int i = 0; i < route.length / 2; i++)
        {
            int temp = route.path[i];
            route.path[i] = route.path[route.length - 1 - i];
            route.path[route.length - 1 - i] = temp;

            char *temp_mode = route.modes[i];
            route.modes[i] = route.modes[route.length - 1 - i];
            route.modes[route.length - 1 - i] = temp_mode;

            double temp_time = route.times[i];
            route.times[i] = route.times[route.length - 1 - i];
            route.times[route.length - 1 - i] = temp_time;
        }
    }

    free(info);
    free(visited);
    return route;
}
