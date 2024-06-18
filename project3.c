#include <stdio.h>
#include <stdlib.h>

#define MAX_JOBS 100

// Job structure
struct Job {
    char name;
    int start;
    int duration;
    int id;
};


// Job queue structure for RR
struct JobQueue {
    struct Job* jobs;
    int front;
    int rear;
    int capacity;
    int size;
};

struct JobQueue* createQueue(int capacity) {
    struct JobQueue* q = (struct JobQueue*)malloc(sizeof(struct JobQueue));
    q->jobs = (struct Job*)malloc(capacity * sizeof(struct Job));
    q->front = 0;
    q->rear = -1;
    q->capacity = capacity;
    q->size = 0;
    return q;
}

int isFull(struct JobQueue* queue) {
    return queue->size == queue->capacity;
}

int isEmpty(struct JobQueue* queue) {
    return queue->size == 0;
}

void enqueue(struct JobQueue* queue, struct Job job) {
    if (isFull(queue)) {
        fprintf(stderr, "Queue is full\n");
        exit(1);
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->jobs[queue->rear] = job;
    queue->size++;
}

struct Job dequeue(struct JobQueue* queue) {
    if (isEmpty(queue)) {
        fprintf(stderr, "Queue is empty\n");
        exit(1);
    }
    struct Job job = queue->jobs[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return job;
}


// Read jobs from a file and set values in the structure
void readJobs(const char* filename, struct Job* jobs, int* numJobs) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }
    char name;
    int start, duration;
    *numJobs = 0;
    while (fscanf(file, " %c %d %d", &name, &start, &duration) == 3) {
        jobs[*numJobs].name = name;
        jobs[*numJobs].start = start;
        jobs[*numJobs].duration = duration;
        jobs[*numJobs].id = (*numJobs);
        (*numJobs)++;
    }
    fclose(file);
}


// First-Come, First-Served scheduling algorithm
void scheduleFCFS(const struct Job* jobs, int numJobs) {
    int minStartTime = jobs[0].start;
    //assign the smallest start time of jobs to maxEndTime
    for (int i = 0; i < numJobs; i++) {
        if (jobs[i].start < minStartTime) {
            minStartTime = jobs[i].start;
        }
    }
    // Find the maximum end time to determine the width of the graph
    int maxEndTime = minStartTime;
    for (int i = 0; i < numJobs; i++) {
        maxEndTime += jobs[i].duration;
    }

    // Create and initialize the graph
    char graph[numJobs][maxEndTime];
    for (int i = 0; i < numJobs; i++) {
        for (int j = 0; j < maxEndTime; j++) {
            graph[i][j] = ' ';
        }
    }


    // Create a new array to store the jobs in the order of their start time
    struct Job array[numJobs];
    // put all the jobs in the array
    for (int i = 0; i < numJobs; i++) {
        array[i] = jobs[i];
    }
    // sort the array based on the start time
    for (int i = 0; i < numJobs - 1; i++) {
        for (int j = i + 1; j < numJobs; j++) {
            if (array[j].start < array[i].start) {
                struct Job temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }
    

    // Fill the graph with job execution markers for FCFS
    // Execute job in order array checking if starting time is less or equal to time
    int time = 0;
    for (int i = 0; i < numJobs; i++) {
        if (array[i].start <= time) {
            for (int j = 0; j < array[i].duration; j++) {
                graph[array[i].id][time + j] = 'X';
            }
            time += array[i].duration;
        }
        else {
            time = array[i].start;
            for (int j = 0; j < array[i].duration; j++) {
                graph[array[i].id][time + j] = 'X';
            }
            time += array[i].duration;
        }
    }
    

    // Print the graph representation for FCFS
    printf("FCFS\n");
    for (int i = 0; i < numJobs; i++) {
        printf("%c ", jobs[i].name);
        for (int j = 0; j < maxEndTime; j++) {
            printf("%c", graph[i][j]);
        }
        printf("\n");
    }
}


// Round Robin scheduling algorithm
void scheduleRR(struct Job* jobs, int numJobs, int quantum) {
    // Find the maximum end time to determine the width of the graph
    int minStartTime = jobs[0].start;
    //assign the smallest start time of jobs to maxEndTime
    for (int i = 0; i < numJobs; i++) {
        if (jobs[i].start < minStartTime) {
            minStartTime = jobs[i].start;
        }
    }
    int maxEndTime = minStartTime;


    for (int i = 0; i < numJobs; i++) {
        maxEndTime += jobs[i].duration;
    }

    // Create and initialize the graph
    char graph[numJobs][maxEndTime];
    for (int i = 0; i < numJobs; i++) {
        for (int j = 0; j < maxEndTime; j++) {
            graph[i][j] = ' ';
        }
    }

    // Create a queue of jobs
    struct JobQueue* queue = createQueue(MAX_JOBS);

    // Initialize time
    int time = minStartTime;

    // Execute RR scheduling
    while (time < maxEndTime) {
        if (isEmpty(queue)) {
            for (int i = 0; i < numJobs; i++) {
                if (jobs[i].start == time) {
                    enqueue(queue, jobs[i]);
                }
            }
        }

        // Dequeue a job and execute it for the quantum time
        else {
            struct Job job = dequeue(queue);
            // Execute the job for the quantum size or until it finishes
            int executeTime = job.duration < quantum ? job.duration : quantum;
            for (int i = 0; i < executeTime; i++) {
                graph[job.id][time] = 'X';
                time++;

                //If there was a new job during the execution, enqueue it
                for (int j = 0; j < numJobs; j++) {
                    if (jobs[j].start == time) {
                        enqueue(queue, jobs[j]);
                    }
                }
            }
            // Update the job's duration
            job.duration -= executeTime;
            
            // If the job is not finished, enqueue it again
            if (job.duration > 0) {
                enqueue(queue, job);
            }
        } 
    }

    // Print the graph representation for RR
    printf("RR\n");
    for (int i = 0; i < numJobs; i++) {
        printf("%c ", jobs[i].name);
        for (int j = 0; j < maxEndTime; j++) {
            printf("%c", graph[i][j]);
        }
        printf("\n");
    }
}



int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    struct Job jobs[MAX_JOBS];
    int numJobs;
    readJobs(argv[1], jobs, &numJobs);

    // Execute FCFS
    scheduleFCFS(jobs, numJobs);

    // Execute RR 
    int quantum = 1;
    scheduleRR(jobs, numJobs, quantum);

    return 0;
}
