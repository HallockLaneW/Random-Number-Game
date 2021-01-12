/* a simple player/Server using semaphores and threads

   usage on Solaris:
     gcc thisfile.c -lpthread -lposix4
     a.out numIters

*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#define SHARED 1
#define NUM_THREADS	2

char* messages[NUM_THREADS];

struct thread_data
{
    int	thread_id;
    int  sum;
};

struct thread_data thread_data_array[NUM_THREADS];

void* player(void*);  /* the two threads */
void* Server(void*);

//join to wait until both players join
//sub is to check for submissions
//res to have results being given
sem_t game, join1, join2, sub1, sub2, res1, res2;    /* the global semaphores */
int data;             /* shared buffer         */
int numIters;
int rands[2];

/* main() -- setup the stuff and print when it is done*/

int main(int argc, char* argv[]) {
    /* thread ids and attributes */
    pthread_t pid1, sid, pid2;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    numIters = atoi(argv[1]);

    //game, join1, join2, sub1, sub2, res1, res2;
    sem_init(&game, SHARED, 1);  /* sem empty = 1 */
    sem_init(&join1, SHARED, 0);   /* sem full = 0  */
    sem_init(&join2, SHARED, 0);  /* sem empty = 0 */
    sem_init(&sub1, SHARED, 0);   /* sem full = 0  */
    sem_init(&sub2, SHARED, 0);  /* sem empty = 0 */
    sem_init(&res1, SHARED, 0);   /* sem full = 0  */
    sem_init(&res2, SHARED, 0);   /* sem full = 0  */

    rands[0] = 10;
    rands[1] = 15;

    
    pthread_create(&sid, &attr, Server, NULL);
    printf("Main thread creates the server thread.\n");
    pthread_create(&pid1, &attr, player, (void*)
        &thread_data_array[0]);
    printf("Main thread creates the player0 thread.\n");
    thread_data_array[0].thread_id = 0;
    
    //thread_data_array[0].sum = 20;
    

    pthread_create(&pid2, &attr, player, (void*)
        &thread_data_array[1]);
    printf("Main thread creates the player1 thread.\n");
    thread_data_array[1].thread_id = 1;

    //thread_data_array[1].sum = 15;

    pthread_join(sid, NULL);
    pthread_join(pid1, NULL);
    pthread_join(pid2, NULL);
    printf("Game Over.\n");
}

/* Player stuff */
void *player(void *threadarg) {

   

   struct thread_data* my_data; 
    int sum, taskid;
    sleep(1);
    my_data = (struct thread_data *)threadarg;
    taskid = my_data->thread_id;
    

    //sem to wait for game creation

    
    /*
    The player first waits for the game to be created,
    Then joins the game posting that it joined,
    Then generates a random number and posts it
    Then waits to recieve result before terminating
    */
    if (taskid == 1) {

        
        //sem_wait(&join2);
        sem_post(&join1);
        sleep(1);
        //sem to wait for game creation
        sem_wait(&game);
        sleep(1);
        
        printf("Player %d joins the game\n", taskid);
        sleep(1);

        sum = rand() % 50;
        thread_data_array[1].thread_id = 0;
        thread_data_array[1].sum = sum;
        printf("player 1 generates %d\n", thread_data_array[1].sum);
        sleep(1);

        //submitting number
        //sem_wait(&sub2);
        sem_post(&sub1);
            printf("Player %d sends the number %d\n", taskid, thread_data_array[taskid].sum);

        //wait for result from server
        sem_wait(&res1);
        sleep(1);
    }
    else {
        
        
        sem_post(&join2);
        sleep(1);
        //sem_wait(&join1);
        //sem to wait for game creation
        sem_wait(&game);
        sleep(1);
        
        
        printf("Player %d joins the game\n", taskid);
        sleep(1);

        sum = rand() % 50;
        thread_data_array[0].thread_id = 0;
        thread_data_array[0].sum = sum;
        printf("player 0 generates %d\n", thread_data_array[0].sum);
        sleep(1);

        //submitting number
        sem_post(&sub2);
        //sem_wait(&sub1);
        printf("Player %d sends the number %d\n", taskid, thread_data_array[taskid].sum);

        //wait for result from server
        sem_wait(&res2);

        sleep(1);
        
    }



  
    printf("Player %d recieves result\n", taskid);
    pthread_exit(NULL);
}

/* run the game */
void *Server(void* arg) {
    int total = 0, consumed;

    sem_wait(&join1);
    sem_wait(&join2);
    
    //game is created, semaphore posted
    printf("Game is created\n");
    
    sem_post(&game);


    //wait for both semaphors of threads to be made
    

    

    //wait for number to be submitted from player 0
    sem_wait(&sub1);
    printf("Server recieves %d from player 0 \n", thread_data_array[0].sum);

    
    //wait for the number to be submitted from player 1
    sem_wait(&sub2);
    printf("Server recieves %d from player 1\n", thread_data_array[1].sum);

    wait(1);

    //compare the numbers and give which one is highest
    //wait
    if (thread_data_array[0].sum > thread_data_array[1].sum) {
        printf("The winner is player 0\n");
    }
    else {
        printf("The winner is player 1\n");
    }
    //post
    sem_post(&res1);
    //post
    sem_post(&res2);


    /*
    for (consumed = 0; consumed < numIters; consumed++) {
        sem_wait(&full);
        total = total + data;
        sem_post(&empty);
    }
    printf("for %d iterations, the total is %d\n", numIters, total);
    */
}
