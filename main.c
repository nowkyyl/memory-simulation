#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define RAM_SIZE 1024
#define PAGE_SIZE 64
#define NUM_PAGES (RAM_SIZE/PAGE_SIZE)
#define MAX_PROCESSES 10

typedef struct
{
    bool allocated;
    int process_id;
} PageTableEntry;

typedef struct
{
    int id;
    int size;
    bool active;
} Process;

char RAM[RAM_SIZE];
PageTableEntry page_table[NUM_PAGES];
Process process_table[MAX_PROCESSES];

void init_memory()
{
    memset(RAM, 0, RAM_SIZE);
    for (int i = 0; i < NUM_PAGES; i++)
    {
        page_table[i].allocated = false;
        page_table[i].process_id = -1;
    }

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        process_table[i].id = -1;
        process_table[i].size = 0;
        process_table[i].active = false;
    }
}

int find_free_process()
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (!process_table[i].active) return i;
    }
    return -1;
}

int allocate_memory(int process_id, int size)
{
    int pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    int allocated_pages = 0;

    int process_slot = find_free_process();
    if (process_slot == -1)
    {
        printf("process table full");
        return -1;
    }

    for (int i = 0; i < NUM_PAGES; i++)
    {
        if (!page_table[i].allocated)
        {
            page_table[i].allocated = true;
            page_table[i].process_id = process_id;
            allocated_pages++;

            if (allocated_pages == pages_needed)
            {
                process_table[process_slot].id = process_id;
                process_table[process_slot].size = size;
                process_table[process_slot].active = true;
                return i * PAGE_SIZE;
            }
        }
    }

    printf("not enough memory for process %d (needs %d pages)\n", process_id, pages_needed);
    return -1;
}

void free_memory(int process_id)
{
    bool found = false;
    for (int i = 0; i < NUM_PAGES; i++)
    {
        if (page_table[i].allocated && page_table[i].process_id == process_id)
        {
            page_table[i].allocated = false;
            page_table[i].process_id = -1;
            found = true;
        }
    }

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].id == process_id)
        {
            process_table[i].active = false;
        }
    }

    if (found) {
        printf("process %d freed from memory.\n", process_id);
    } else {
        printf("process %d not found in memory.\n", process_id);
    }
}

void display_memory()
{
    printf("\nmemory state:\n");
    for (int i = 0; i < NUM_PAGES; i++)
    {
        printf("page %d | %s | process %d\n",
            i,
            page_table[i].allocated ? "allocated" : "free",
            page_table[i].process_id
        );
    }
}

void display_process()
{
    printf("\nprocesses:\n");
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].active)
        {
            printf("process id: %d | size: %d bytes\n",
                process_table[i].id,
                process_table[i].size
            );
        }
    }
}

int main()
{
    init_memory();

    int p1 = allocate_memory(1, 120);
    int p2 = allocate_memory(2, 180);
    int p3 = allocate_memory(3, 300);

    display_memory();
    display_process();

    free_memory(1);
    printf("\nfreeing process 1:\n");
    display_memory();
    display_process();

    #ifdef _WIN32
        system("pause");
    #else
        printf("press enter to exit...");
        getchar();
    #endif

    return 0;
}