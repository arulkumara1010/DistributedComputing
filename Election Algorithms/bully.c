#include <stdio.h>
#include <stdlib.h>

#define MAX_NODES 10

typedef struct Node
{
    int id;
    int isAlive;
} Node;

Node nodes[MAX_NODES];
int n;
int leader = -1;

void startElection(int initiator)
{
    printf("\nNode %d initiated an election.\n", nodes[initiator].id);
    int highestID = nodes[initiator].id;
    int foundHigher = 0;

    for (int i = initiator + 1; i < n; i++)
    {
        if (nodes[i].isAlive && nodes[i].id > highestID)
        {
            printf("Node %d sent a message to Node %d\n", nodes[initiator].id, nodes[i].id);
            foundHigher = 1;
            highestID = nodes[i].id;
        }
    }

    if (!foundHigher)
    {
        leader = nodes[initiator].id;
        printf("Node %d is elected as the leader.\n", leader);
    }
    else
    {
        for (int i = initiator + 1; i < n; i++)
        {
            if (nodes[i].isAlive && nodes[i].id == highestID)
            {
                startElection(i);
                return;
            }
        }
    }
}

void declareLeader()
{
    printf("\nLeader is Node %d\n", leader);
}

void markNodeDead(int id)
{
    for (int i = 0; i < n; i++)
    {
        if (nodes[i].id == id)
        {
            nodes[i].isAlive = 0;
            printf("\nNode %d is now dead.\n", id);
            if (leader == id)
            {
                printf("Leader Node %d has failed. Starting re-election...\n", id);
                for (int j = 0; j < n; j++)
                {
                    if (nodes[j].isAlive)
                    {
                        startElection(j);
                        return;
                    }
                }
                printf("No nodes are alive to elect a leader.\n");
                leader = -1;
            }
            return;
        }
    }
    printf("Node %d not found.\n", id);
}

void markNodeAlive(int id)
{
    for (int i = 0; i < n; i++)
    {
        if (nodes[i].id == id)
        {
            nodes[i].isAlive = 1;
            printf("\nNode %d is now alive.\n", id);

            if (leader < id)
            {
                printf("Leader Node %d has been revived. Starting re-election...\n", id);
                for (int j = 0; j < n; j++)
                {
                    if (nodes[j].isAlive)
                    {
                        startElection(j);
                        return;
                    }
                }
            }
            return;
        }
    }
    printf("Node %d not found.\n", id);
}

int main()
{
    printf("Enter the number of nodes: ");
    scanf("%d", &n);

    printf("Enter the IDs of the nodes:\n");
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &nodes[i].id);
        nodes[i].isAlive = 1;
    }

    int choice;
    do
    {
        printf("\n--- Menu ---\n");
        printf("1. Start Election\n");
        printf("2. Mark Node as Dead\n");
        printf("3. Mark Node as Alive\n");
        printf("4. Display Current Leader\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        {
            int initiator;
            printf("Enter the ID of the node initiating the election: ");
            scanf("%d", &initiator);

            int initiatorIndex = -1;
            for (int i = 0; i < n; i++)
            {
                if (nodes[i].id == initiator)
                {
                    initiatorIndex = i;
                    break;
                }
            }

            if (initiatorIndex == -1 || !nodes[initiatorIndex].isAlive)
            {
                printf("Invalid or dead initiator ID.\n");
            }
            else
            {
                startElection(initiatorIndex);
            }
            break;
        }
        case 2:
        {
            int id;
            printf("Enter the ID of the node to mark as dead: ");
            scanf("%d", &id);
            markNodeDead(id);
            break;
        }
        case 3:
        {
            int id;
            printf("Enter the ID of the node to mark as alive: ");
            scanf("%d", &id);
            markNodeAlive(id);
            break;
        }
        case 4:
        {
            if (leader == -1)
            {
                printf("\nNo leader is currently elected.\n");
            }
            else
            {
                declareLeader();
            }
            break;
        }
        case 5:
            printf("\nExiting program.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (choice != 5);

    return 0;
}
