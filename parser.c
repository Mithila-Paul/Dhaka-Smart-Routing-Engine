#include "dhaka_routing.h"

void load_roadmap(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        exit(1);
    }

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), file))
    {
        char *token = strtok(line, ",");
        if (!token || strcmp(token, "DhakaStreet") != 0)
            continue;

        Coordinate coords[256];
        int coord_count = 0;

        while (1)
        {
            char *lon_str = strtok(NULL, ",");
            char *lat_str = strtok(NULL, ",");

            if (!lon_str || !lat_str)
                break;

            double lon = atof(lon_str);
            double lat = atof(lat_str);

            if (lat < 23.0 || lat > 24.5 || lon < 90.0 || lon > 91.0)
                continue;

            coords[coord_count].lat = lat;
            coords[coord_count].lon = lon;
            coord_count++;

            if (coord_count >= 255)
                break;
        }

        for (int i = 0; i < coord_count - 1; i++)
        {
            int from = find_or_add_node(coords[i], "road");
            int to = find_or_add_node(coords[i + 1], "road");
            double dist = haversine_distance(coords[i], coords[i + 1]);

            add_edge(from, to, dist, "car");
            add_edge(to, from, dist, "car");
        }
    }

    fclose(file);
    printf("Loaded roadmap: %d nodes, %d edges\n", node_count, edge_count);
}

void load_routemap(const char *filename, const char *mode)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        exit(1);
    }

    char line[MAX_LINE];
    int initial_edges = edge_count;

    while (fgets(line, sizeof(line), file))
    {
        char *token = strtok(line, ",");
        if (!token)
            continue;

        Coordinate coords[256];
        int coord_count = 0;

        while (1)
        {
            char *lon_str = strtok(NULL, ",");
            char *lat_str = strtok(NULL, ",");

            if (!lon_str || !lat_str)
                break;

            double lon = atof(lon_str);
            double lat = atof(lat_str);

            if (lat < 23.0 || lat > 24.5 || lon < 90.0 || lon > 91.0)
                continue;

            coords[coord_count].lat = lat;
            coords[coord_count].lon = lon;
            coord_count++;

            if (coord_count >= 255)
                break;
        }

        for (int i = 0; i < coord_count - 1; i++)
        {
            int from = find_or_add_node(coords[i], mode);
            int to = find_or_add_node(coords[i + 1], mode);
            double dist = haversine_distance(coords[i], coords[i + 1]);

            add_edge(from, to, dist, mode);
            add_edge(to, from, dist, mode);
        }
    }

    fclose(file);
    printf("Loaded %s routes: added %d edges\n", mode, edge_count - initial_edges);
}
