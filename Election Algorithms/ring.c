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

    do
    {
        if (current->active)
        {
            if (current->id > maxId)
            {
                maxId = current->id;
                maxProcess = current;
            }
        }
        current = current->next;
    } while (current != head);

    printf("Process with ID %d is the coordinator!\n", maxProcess->id);
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

int main()
{
    Process *head = NULL;
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
    }

    return 0;
}
