#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define _USE_MATH_DEFINES
#define frand() (rand()/(double)RAND_MAX)
#define nrand() (sqrt(-2*log(frand()))*cos(2*M_PI*frand()))

double avg_utilization = 0;
double number_of_examined_holes = 0;
// Head of the linked list of holes, initialized to -1 indicating no holes
int head = -1;
int current_hole = 0;

void initialize(int n, int d, int v, int** mem, int** blocks) {
    srand(time(NULL));
    int max_blocks = n / (d - v);
    *mem = (int*)malloc(sizeof(int) * n);
    if (!*mem) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    (*mem)[0] = (*mem)[n - 1] = -n + 2;
    (*mem)[1] = (*mem)[2] = 0;
    head = 0;
    *blocks = (int*)malloc(max_blocks * sizeof(int));
    if (!*blocks) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

int nrand_gen(int d, double v, int n) {
    double rv;
    do {
        rv = nrand() * v + d;
    } while (rv < 3 || rv > n - 4);
    return (int)rv;
}

int request_best_fit(int* blocks, int* block_count, int* mem, int d, double v, int n) {
    // No hole is found
    if (head == -1)
        return 0;

    int size = nrand_gen(d, v, n);
    int iterator = head, pred, succ, new_size, block_address;

    while (size > -mem[iterator]) {
        number_of_examined_holes++;
        // Iterator = iterator->next
        iterator = mem[iterator + 2];
        if (head == iterator)
            // Unsuccessful
            return 0;
    }

    int small = iterator;
    do {
        number_of_examined_holes++;
        if (mem[iterator] > mem[small] && size < -mem[iterator])
            small = iterator;
        // Iterator = iterator->next
        iterator = mem[iterator + 2];
    } while (iterator != head);

    iterator = small;

    // Fill completely
    if (abs(size + mem[iterator]) <= 4) {
        // Block fills hole completely
        size = -mem[iterator];
        mem[iterator] = mem[iterator + size + 1] = size;
        pred = mem[iterator + 1];
        succ = mem[iterator + 2];
        // Current hole is the only hole
        if (pred == iterator)
            head = -1;
        else {
            // Current->prev->next = current->next
            mem[pred + 2] = succ;
            // Current->next->prev = current->prev
            mem[succ + 1] = pred;
        }
        block_address = iterator;
    }
        // Fill partially
        else {
        mem[iterator] += (size + 2);
        new_size = -mem[iterator];
        mem[iterator + new_size + 1] = mem[iterator];
        block_address = iterator + new_size + 2;
        mem[block_address] = mem[block_address + size + 1] = size;
    }

    blocks[*block_count] = block_address;
    (*block_count)++;
    // Successful
    return 1;
}

int request_first_fit(int* blocks, int* block_count, int* mem, int d, double v, int n) {
    // No hole found
    if (head == -1)
        return 0;

    int size = nrand_gen(d, v, n);
    int iterator = head, pred, succ, new_size, block_address;

    while (size > -mem[iterator]) {
        number_of_examined_holes++;
        // Iterator = iterator->next
        iterator = mem[iterator + 2]; 
        if (head == iterator)
            // Unsuccessful
            return 0; 
    }

    // Fill completely
    if (abs(size + mem[iterator]) <= 4) {
        // Block fills hole completely
        size = -mem[iterator]; 
        mem[iterator] = mem[iterator + size + 1] = size;
        pred = mem[iterator + 1];
        succ = mem[iterator + 2];
        // Current hole is the only hole
        if (pred == iterator) 
            head = -1;
        else {
            // Current->prev->next = current->next
            mem[pred + 2] = succ;
            // Current->next->prev = current->prev
            mem[succ + 1] = pred; 
        }
        block_address = iterator;
        // Fill partially
    } else { 
        mem[iterator] += (size + 2);
        new_size = -mem[iterator];
        mem[iterator + new_size + 1] = mem[iterator];
        block_address = iterator + new_size + 2;
        mem[block_address] = mem[block_address + size + 1] = size;
    }

    blocks[*block_count] = block_address;
    (*block_count)++;
    // Successful
    return 1; 
}

int request_next_fit(int* blocks, int* block_count, int* mem, int d, double v, int n) {
    if (head == -1) // no hole is found!
        return 0;

    int size = nrand_gen(d, v, n);
    int iterator = head, pred, succ, new_size, block_address;

    // Setting the iterator to the hole we last allocated
    iterator = current_hole; 

    while (size > -mem[iterator]) {
        number_of_examined_holes++;
        // Iterator = iterator->next
        iterator = mem[iterator + 2]; 
        if (head == iterator)
            // Unsuccessful
            return 0; 
    }

    // Fill completely
    if (abs(size + mem[iterator]) <= 4) {
        // Block fills hole completely
        size = -mem[iterator]; 
        mem[iterator] = mem[iterator + size + 1] = size;
        pred = mem[iterator + 1];
        succ = mem[iterator + 2];
        // Current hole is the only hole
        if (pred == iterator) 
            head = -1;
        else {
            // Current->prev->next = current->next
            mem[pred + 2] = succ;
            // Current->next->prev = current->prev
            mem[succ + 1] = pred; 
        }
        block_address = iterator;
        // Fill partially
    } else { 
        mem[iterator] += (size + 2);
        new_size = -mem[iterator];
        mem[iterator + new_size + 1] = mem[iterator];
        block_address = iterator + new_size + 2;
        mem[block_address] = mem[block_address + size + 1] = size;
    }

    blocks[*block_count] = block_address;
    // Saving last allocated hole index.
    current_hole = iterator; 
    (*block_count)++;
    // Successful
    return 1; 
}

int request_worst_fit(int* blocks, int* block_count, int* mem, int d, double v, int n) {
    // No hole found
    if (head == -1) 
        return 0;

    int size = nrand_gen(d, v, n);
    int iterator = head, pred, succ, new_size, block_address;
    int largest_hole_size = -mem[iterator];

    // Keep checking to find the largest hole
    while (-mem[iterator] >= largest_hole_size) { 
        number_of_examined_holes++;
        largest_hole_size = -mem[iterator];
        // Iterator = iterator->next
        iterator = mem[iterator + 2]; 
        if (head == iterator)
            // Unsuccessful
            return 0; 
    }

    int large = iterator;
    do {
        number_of_examined_holes++;
        if (mem[iterator] > mem[large] && size < -mem[iterator])
            large = iterator;
        // Iterator = iterator->next
        iterator = mem[iterator + 2]; 
    } while (iterator != head);

    iterator = large;

    // Fill completely
    if (abs(size + mem[iterator]) <= 4) {
        // Block fills hole completely
        size = -mem[iterator]; 
        mem[iterator] = mem[iterator + size + 1] = size;
        pred = mem[iterator + 1];
        succ = mem[iterator + 2];
        // Current hole is the only hole
        if (pred == iterator) 
            head = -1;
        else {
            // Current->prev->next = current->next
            mem[pred + 2] = succ;
            // Current->next->prev = current->prev
            mem[succ + 1] = pred; 
        }
        block_address = iterator;
    } else {
        // Fill partially
        mem[iterator] += (size + 2);
        new_size = -mem[iterator];
        mem[iterator + new_size + 1] = mem[iterator];
        block_address = iterator + new_size + 2;
        mem[block_address] = mem[block_address + size + 1] = size;
    }

    blocks[*block_count] = block_address;
    (*block_count)++;
    // Successful
    return 1; 
}

void release(int* blocks, int* block_count, int* mem) {
    if (!*block_count)
        return;

    int to_be_released = rand() % (*block_count);
    printf("Releasing block at location (address) %d\n", blocks[to_be_released]);
    int hole_start_index;

    if (mem[blocks[to_be_released] - 1] < 0 && mem[blocks[to_be_released] + mem[blocks[to_be_released]] + 1] < 0) {
        // Case 4: Both left and right neighbors are holes
        hole_start_index = blocks[to_be_released] + mem[blocks[to_be_released] - 1] - 2;
        int hole_end_index = blocks[to_be_released] + mem[blocks[to_be_released]] + 2 + mem[blocks[to_be_released] + mem[blocks[to_be_released]] + 2] - 2;
        mem[hole_start_index] = mem[hole_end_index] = -1 * (hole_end_index - hole_start_index + 1);
        mem[mem[hole_start_index + 2] + 1] = hole_start_index;
        mem[mem[hole_start_index + 1] + 2] = hole_start_index;
    } else if (mem[blocks[to_be_released] - 1] < 0) {
        // Case 3: Only left neighbor is a hole
        hole_start_index = blocks[to_be_released] + mem[blocks[to_be_released] - 1] - 2;
        mem[hole_start_index] = mem[blocks[to_be_released] + mem[blocks[to_be_released]] + 1] = -1 * (mem[blocks[to_be_released]] + 2 + (-mem[blocks[to_be_released] - 1]));
    } else if (mem[blocks[to_be_released] + mem[blocks[to_be_released]] + 1] < 0) {
        // Case 2: Only right neighbor is a hole
        hole_start_index = blocks[to_be_released];
        mem[hole_start_index] = mem[blocks[to_be_released] + mem[blocks[to_be_released]] + 1] = -1 * (mem[blocks[to_be_released]] + 2 + (-mem[blocks[to_be_released] + mem[blocks[to_be_released]] + 1]));
    } else {
        // Case 1: No neighbors are holes
        hole_start_index = blocks[to_be_released];
        mem[hole_start_index] = mem[hole_start_index + mem[blocks[to_be_released]] + 1] = -mem[blocks[to_be_released]] - 2;
        mem[hole_start_index + 1] = mem[head + 1];
        mem[hole_start_index + 2] = head;
        mem[mem[head + 1] + 2] = hole_start_index;
        mem[head + 1] = hole_start_index;
    }

    blocks[to_be_released] = blocks[(*block_count) - 1];
    (*block_count)--;
}

void update_memory_utilization(int* blocks, int block_count, int* mem, int n, int x) {
    double utilization = 0;
    for (int i = 0; i < block_count; i++)
        utilization += mem[blocks[i]];
    utilization /= n;
    avg_utilization += utilization / x;
}

int main(int argc, char** argv) {
    int x, n, d;
    double v;
    while (*++argv) {
        if (**argv != '-')
            return 1;
        switch ((*argv)[1]) {
            case 'x':
                x = atoi(*++argv);
                break;
            case 'n':
                n = pow(2, atoi(*++argv));
                break;
            case 'd':
                d = pow(2, atoi(*++argv));
                break;
            case 'v':
                v = pow(2, atoi(*++argv));
                break;
            default:
                // Error
                return 1;
        }
    }

    // Memory
    int* mem;
    int* blocks;
    int block_count = 0;

    printf("Running the simulation with x=%d n=%d d=%d v=%.2f\n", x, n, d, v);
    initialize(n, d, v, &mem, &blocks);

    int originalX = x;
    while (x > 0) {
        x--;
        while (request_first_fit(blocks, &block_count, mem, d, v, n));
        update_memory_utilization(blocks, block_count, mem, n, originalX);
        release(blocks, &block_count, mem);
    }

    printf("Average utilization is %.3f\n", avg_utilization);
    printf("Examined holes are %.3f\n", number_of_examined_holes / originalX);

    free(mem);
    free(blocks);

    return 0;
}
