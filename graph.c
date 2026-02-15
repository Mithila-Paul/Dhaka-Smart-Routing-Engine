#include "dhaka_routing.h"

Node *nodes = NULL;
Edge *edges = NULL;
int node_count = 0;
int edge_count = 0;

void init_graph()
{
    nodes = (Node *)malloc(MAX_NODES * sizeof(Node));
    edges = (Edge *)malloc(MAX_EDGES * sizeof(Edge));
    node_count = 0;
    edge_count = 0;

    if (!nodes || !edges)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
}

void free_graph()
{
    if (nodes)
        free(nodes);
    if (edges)
        free(edges);
    nodes = NULL;
    edges = NULL;
    node_count = 0;
    edge_count = 0;
}

int find_or_add_node(Coordinate coord, const char *type)
{
    for (int i = 0; i < node_count; i++)
    {
        double dist = haversine_distance(nodes[i].coord, coord);
        if (dist < 0.01)
        { // 10 meters
            return i;
        }
    }

    if (node_count >= MAX_NODES)
    {
        fprintf(stderr, "Warning: Maximum nodes reached\n");
        return node_count - 1;
    }

    nodes[node_count].id = node_count;
    nodes[node_count].coord = coord;
    strncpy(nodes[node_count].type, type, sizeof(nodes[node_count].type) - 1);
    nodes[node_count].type[sizeof(nodes[node_count].type) - 1] = '\0';

    return node_count++;
}

int find_nearest_node(Coordinate coord)
{
    if (node_count == 0)
        return -1;

    int nearest = 0;
    double min_dist = haversine_distance(nodes[0].coord, coord);

    for (int i = 1; i < node_count; i++)
    {
        double dist = haversine_distance(nodes[i].coord, coord);
        if (dist < min_dist)
        {
            min_dist = dist;
            nearest = i;
        }
    }

    return nearest;
}

void add_edge(int from, int to, double distance, const char *mode)
{
    if (edge_count >= MAX_EDGES)
    {
        fprintf(stderr, "Warning: Maximum edges reached\n");
        return;
    }

    edges[edge_count].from = from;
    edges[edge_count].to = to;
    edges[edge_count].distance = distance;
    strncpy(edges[edge_count].mode, mode, sizeof(edges[edge_count].mode) - 1);
    edges[edge_count].mode[sizeof(edges[edge_count].mode) - 1] = '\0';

    edge_count++;
}
