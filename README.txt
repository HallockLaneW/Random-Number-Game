a simple player/Server using semaphores and threads

The player first waits for the game to be created,
Then joins the game posting that it joined,
Then generates a random number and posts it
Then waits to recieve result before terminating




to run:

./a.out 3900

To generate executable:

gcc sem_fin.c -pthread
