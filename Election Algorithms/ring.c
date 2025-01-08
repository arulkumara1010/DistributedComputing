#include <stdio.h>
#include <stdlib.h>

typedef struct Process
{
    int id;
    struct Process *next;
    int active;
} Process;

Process *createProcess(int id, int active)
{
    Process *newProcess = (Process *)malloc(sizeof(Process));
    newProcess->id = id;
    newProcess->next = NULL;
    newProcess->active = active;
    return newProcess;
}

void insertProcess(Process **head, int id, int active)
{
    Process *newProcess = createProcess(id, active);
    if (*head == NULL)
    {
        *head = newProcess;
        newProcess->next = newProcess;
    }
    else
    {
        Process *temp = *head;
        while (temp->next != *head)
        {
            temp = temp->next;
        }
        temp->next = newProcess;
        newProcess->next = *head;
    }
}

void startElection(Process *head, int initiatorId)
{
    if (head == NULL)
    {
        printf("Error: No processes in the ring. Election cannot be started.\n");
        return;
    }

    Process *current = head;
    int maxId = initiatorId;
    Process *maxProcess = NULL;
    Process *initiator = NULL;
    do
    {
        if (current->id == initiatorId)
        {
            initiator = current;
            break;
        }
        current = current->next;
    } while (current != head);

    if (initiator == NULL)
    {
        printf("Process with ID %d not found in the ring.\n", initiatorId);
        return;
    }

    printf("Process with ID %d starts the election.\n", initiatorId);
    current = initiator->next;
    while (current != initiator)
    {
        if (current->active)
        {
            printf("Process with ID %d sends election message to process with ID %d.\n", initiatorId, current->id);
            if (current->id > maxId)
            {
                maxId = current->id;
                maxProcess = current;
            }
            initiatorId = current->id;
        }
        current = current->next;
    }

    if (maxProcess != NULL)
    {
        printf("Process with ID %d is elected as the coordinator!\n", maxProcess->id);
    }
    else
    {
        printf("No active process found for election.\n");
    }
}

void printProcesses(Process *head)
{
    if (head == NULL)
    {
        printf("No processes in the ring.\n");
        return;
    }

    Process *temp = head;
    do
    {
        printf("Process ID: %d, Active: %d\n", temp->id, temp->active);
        temp = temp->next;
    } while (temp != head);
}

void checkCoordinator(Process **head, Process **coordinator)
{
    if (*coordinator == NULL || !(*coordinator)->active)
    {
        printf("Coordinator is inactive. Starting a new election.\n");
        startElection(*head, (*coordinator)->next->id);
    }
}

int main()
{
    Process *head = NULL;
    Process *coordinator = NULL;
    int choice, id, active;

    while (1)
    {
        printf("\nMenu:\n");
        printf("1. Insert process\n");
        printf("2. Mark process as active or inactive\n");
        printf("3. Start election\n");
        printf("4. Print processes\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("Enter process ID: ");
            scanf("%d", &id);
            printf("Enter process status (1 for active, 0 for inactive): ");
            scanf("%d", &active);
            insertProcess(&head, id, active);
            if (coordinator == NULL || id > coordinator->id)
            {
                coordinator = head;
            }
            printf("Process with ID %d inserted successfully.\n", id);
            break;

        case 2:
            printf("Enter process ID to update: ");
            scanf("%d", &id);
            Process *current = head;
            int found = 0;
            do
            {
                if (current->id == id)
                {
                    found = 1;
                    printf("Enter new status for process %d (1 for active, 0 for inactive): ", id);
                    scanf("%d", &active);
                    current->active = active;
                    printf("Process %d status updated to %d.\n", id, active);

                    if (current->active == 0)
                    {
                        printf("Process %d has been marked as inactive. Starting an election.\n", id);
                        startElection(current->next, current->next->id);
                    }
                    else if (active == 1)
                    {
                        printf("Process %d reawakened. Starting an election.\n", id);
                        startElection(head, id);
                    }
                    break;
                }
                current = current->next;
            } while (current != head);
            if (!found)
            {
                printf("Process with ID %d not found.\n", id);
            }
            break;

        case 3:
            printf("Enter the ID of the process initiating the election: ");
            scanf("%d", &id);
            startElection(head, id);
            break;

        case 4:
            printProcesses(head);
            break;

        case 5:
            printf("Exiting program.\n");
            exit(0);

        default:
            printf("Invalid choice, please try again.\n");
        }

        checkCoordinator(&head, &coordinator);
    }

    return 0;
}
