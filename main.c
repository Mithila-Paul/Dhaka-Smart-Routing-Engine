#include "dhaka_routing.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Welcom To Dhaka Routing System - Mr. Efficient HAHA!\n");

    init_graph();

    printf("Loading datasets...\n");
    printf("Loading roadmap...\n");
    load_roadmap("Roadmap-Dhaka.csv");
    printf("Readmap loaded\n");
    
    printf("Loading metro...\n");
    load_routemap("Routemap-DhakaMetroRail.csv", "metro");
    printf("Readmap loaded\n");

    printf("Loading BikolpoBus...\n");
    load_routemap("Routemap-BikolpoBus.csv", "bikolpo");
    printf("Readmap loaded\n");

    printf("Loading UttaraBus...\n");
    load_routemap("Routemap-UttaraBus.csv", "uttara");
    printf("Readmap loaded\n");

    printf("Datasets Loaded Successfully!\n");
    printf("Total nodes: %d\n", node_count);
    printf("Total edges: %d\n\n", edge_count);

    while (1)
    {
        Coordinate source, destination;
        int choice;
        double start_time = -1;
        double deadline = -1;

        printf("Enter Source Coordinates\n");
        printf("Latitude: ");
        scanf("%lf", &source.lat);

        printf("Longitude: ");
        scanf("%lf", &source.lon);

        printf("\nEnter Destination Coordinates\n");
        printf("Latitude: ");
        scanf("%lf", &destination.lat);

        printf("Longitude: ");
        scanf("%lf", &destination.lon);

        int start_node = find_nearest_node(source);
        int end_node = find_nearest_node(destination);

        if (start_node == -1 || end_node == -1)
        {
            printf("Error: Could not find nearest nodes.\n");
            continue;
        }

        printf("\nChoose Routing Option:\n");
        printf("1. Shortest Car Route\n");
        printf("2. Cheapest Route (Car + Metro)\n");
        printf("3. Cheapest Route (All Modes)\n");
        printf("4. Cheapest Route with Time Schedule\n");
        printf("5. Fastest Route\n");
        printf("6. Cheapest Route with Deadline\n");
        printf("0. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 0)
        {
            printf("Exiting system...\n");
            break;
        }

        Route route;

        switch (choice)
        {
        case 1:
            printf("\nSolving Shortest Car Route...The 1st problem..\n");
            route = dijkstra_shortest_distance(start_node, end_node);
            break;

        case 2:
            printf("\nSolving Cheapest Route (Car + Metro)...The 2nd problem..\n");
            route = dijkstra_cheapest_route(start_node, end_node, 1, 0, 0);
            break;

        case 3:
            printf("\nSolving Cheapest Route (All Modes)...The 3rd problem..\n");
            route = dijkstra_cheapest_route(start_node, end_node, 1, 1, 1);
            break;

        case 4:
        {
            char time_str[20];
            printf("Enter start time (HH:MM AM/PM): ");
            scanf(" %[^\n]", time_str);
            start_time = parse_time(time_str);

            printf("\nSolving Cheapest Route with Time Schedule...The 4th problem..\n");
            route = dijkstra_cheapest_with_time(
                start_node, end_node, start_time, 1, 1, 1);
            break;
        }

        case 5:
        {
            char time_str[20];
            printf("Enter start time (HH:MM AM/PM): ");
            scanf(" %[^\n]", time_str);
            start_time = parse_time(time_str);

            printf("\nSolving Fastest Route...The 5th problem..\n");
            route = dijkstra_fastest_route(
                start_node, end_node, start_time);
            break;
        }

        case 6:
        {
            char time_str[20];
            char deadline_str[20];

            printf("Enter start time (HH:MM AM/PM): ");
            scanf(" %[^\n]", time_str);

            printf("Enter deadline time (HH:MM AM/PM): ");
            scanf(" %[^\n]", deadline_str);

            start_time = parse_time(time_str);
            deadline = parse_time(deadline_str);

            printf("\nSolving Cheapest Route with Deadline...The 6th problem...\n");
            route = dijkstra_cheapest_with_deadline(
                start_node, end_node,
                start_time, deadline,
                1, 1, 1);
            break;
        }

        default:
            printf("Invalid choice! Try again.\n");
            continue;
        }

        print_route_output(route, source, destination,
                           choice, start_time, deadline);

        // KML here!
        create_kml_file(route, source, destination,
                        "outputs/route.kml");

        free_route(&route);

        printf("\nQuery completed successfully!\n");
        int x;
        printf("Run another query? (1 = Yes, 0 = No): ");
        scanf("%d", &x);
        if (!x)
            break;
    }

    free_graph();

    printf("\nSystem shutdown complete.\n");
    return 0;
}
