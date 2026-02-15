#include "dhaka_routing.h"

void print_route_output(Route route, Coordinate src, Coordinate dst, int problem_num,
                        double start_time, double deadline)
{
    if (route.length == 0)
    {
        printf("No path found!\n");
        return;
    }

    printf("\nProblem No: %d\n", problem_num);
    printf("Source: (%.6f, %.6f)\n", src.lon, src.lat);
    printf("Destination: (%.6f, %.6f)\n", dst.lon, dst.lat);

    if (start_time >= 0)
    {
        printf("Starting time at source: %s\n", format_time(start_time));
    }
    if (deadline >= 0)
    {
        printf("Destination reaching time: %s\n", format_time(deadline));
    }

    double walk_start_dist = haversine_distance(src, nodes[route.path[0]].coord);
    double walk_end_dist = haversine_distance(dst, nodes[route.path[route.length - 1]].coord);

    double current_time = start_time >= 0 ? start_time : 0.0;

    if (walk_start_dist > 0.01)
    { // More than 10 meters
        double walk_time = (walk_start_dist / WALKING_SPEED) * 60.0;
        printf("%s - %s, Cost: ৳0.00: Walk from Source (%.6f, %.6f) to (%.6f, %.6f).\n",
               format_time(current_time),
               format_time(current_time + walk_time),
               src.lon, src.lat,
               nodes[route.path[0]].coord.lon, nodes[route.path[0]].coord.lat);
        current_time += walk_time;
    }

    char current_mode[50] = "";
    int segment_start = 0;
    double segment_cost = 0.0;
    double segment_start_time = current_time;

    for (int i = 0; i < route.length - 1; i++)
    {
        double edge_dist = 0.0;
        char edge_mode[50] = "car";

        for (int j = 0; j < edge_count; j++)
        {
            if (edges[j].from == route.path[i] && edges[j].to == route.path[i + 1])
            {
                edge_dist = edges[j].distance;
                strcpy(edge_mode, edges[j].mode);
                break;
            }
        }

        if (strlen(current_mode) == 0)
        {
            strcpy(current_mode, edge_mode);
            segment_start = i;
            segment_start_time = current_time;
        }

        if (strcmp(current_mode, edge_mode) != 0 || i == route.length - 2)
        {
            double seg_dist = 0.0;
            for (int k = segment_start; k <= i; k++)
            {
                if (k < route.length - 1)
                {
                    for (int j = 0; j < edge_count; j++)
                    {
                        if (edges[j].from == route.path[k] && edges[j].to == route.path[k + 1])
                        {
                            seg_dist += edges[j].distance;
                            break;
                        }
                    }
                }
            }

            if (i == route.length - 2 && strcmp(current_mode, edge_mode) == 0)
            {
                seg_dist += edge_dist;
            }

            double seg_cost = seg_dist * get_mode_cost(current_mode);

            double departure_time = current_time;
            if (start_time >= 0)
            {
                departure_time = get_next_departure(current_mode, current_time);
            }
            double travel_time = get_travel_time(seg_dist, current_mode);
            double arrival_time = departure_time + travel_time;

            char mode_name[50];
            if (strcmp(current_mode, "car") == 0)
                strcpy(mode_name, "Car");
            else if (strcmp(current_mode, "metro") == 0)
                strcpy(mode_name, "Metro");
            else if (strcmp(current_mode, "bikolpo") == 0)
                strcpy(mode_name, "Bikolpo Bus");
            else if (strcmp(current_mode, "uttara") == 0)
                strcpy(mode_name, "Uttara Bus");
            else
                strcpy(mode_name, "Car");

            printf("%s - %s, Cost: ৳%.2f: Ride %s from (%.6f, %.6f) to (%.6f, %.6f).\n",
                   format_time(departure_time),
                   format_time(arrival_time),
                   seg_cost,
                   mode_name,
                   nodes[route.path[segment_start]].coord.lon,
                   nodes[route.path[segment_start]].coord.lat,
                   nodes[route.path[i]].coord.lon,
                   nodes[route.path[i]].coord.lat);

            current_time = arrival_time;

            if (i < route.length - 2)
            {
                strcpy(current_mode, edge_mode);
                segment_start = i;
                segment_start_time = current_time;
            }
        }
    }

    if (walk_end_dist > 0.01)
    { // More than 10 meters
        double walk_time = (walk_end_dist / WALKING_SPEED) * 60.0;
        printf("%s - %s, Cost: ৳0.00: Walk from (%.6f, %.6f) to Destination (%.6f, %.6f).\n",
               format_time(current_time),
               format_time(current_time + walk_time),
               nodes[route.path[route.length - 1]].coord.lon,
               nodes[route.path[route.length - 1]].coord.lat,
               dst.lon, dst.lat);
    }

    printf("\n");
}

// KML file
void create_kml_file(Route route, Coordinate src, Coordinate dst, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Error: Cannot create KML file %s\n", filename);
        return;
    }

    fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(file, "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n");
    fprintf(file, "<Document>\n");
    fprintf(file, "<Placemark>\n");
    fprintf(file, "<name>route.kml</name>\n");
    fprintf(file, "<LineString>\n");
    fprintf(file, "<tessellate>1</tessellate>\n");
    fprintf(file, "<coordinates>\n");

    double start_dist = haversine_distance(src, nodes[route.path[0]].coord);
    if (start_dist > 0.01)
    {
        fprintf(file, "%.6f,%.6f,0\n", src.lon, src.lat);
    }

    for (int i = 0; i < route.length; i++)
    {
        fprintf(file, "%.6f,%.6f,0\n",
                nodes[route.path[i]].coord.lon,
                nodes[route.path[i]].coord.lat);
    }

    double end_dist = haversine_distance(dst, nodes[route.path[route.length - 1]].coord);
    if (end_dist > 0.01)
    {
        fprintf(file, "%.6f,%.6f,0\n", dst.lon, dst.lat);
    }

    fprintf(file, "</coordinates>\n");
    fprintf(file, "</LineString>\n");
    fprintf(file, "</Placemark>\n");
    fprintf(file, "</Document>\n");
    fprintf(file, "</kml>\n");

    fclose(file);
    printf("KML file created: %s\n", filename);
}
