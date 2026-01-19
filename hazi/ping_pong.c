// Házi feladat: Ping-Pong
// 
// Feladat: A szülő és a gyerek felváltva írja ki a PING és PONG szavakat.
// Használj két szemafort a szinkronizációhoz!

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>

#define ROUNDS 10  // Hányszor ismétlődjön a ping-pong?

typedef struct {
    sem_t sem_parent;
    sem_t sem_child;
} SharedSems;

int main() {
    // Közös memória a szemaforoknak
    SharedSems *sems = mmap(NULL, sizeof(SharedSems),
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS,
                            -1, 0);
    
    if (sems == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }
    
    // MEGOLDÁS: Inicializálás
    // A szülő indulhat (1 = nyitva), a gyerek vár (0 = zárva)
    sem_init(&sems->sem_parent, 1, 1);
    sem_init(&sems->sem_child, 1, 0);
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }
    
    if (pid == 0) {
        // ========== GYEREK ==========
        
        for (int i = 0; i < ROUNDS; i++) {
            // MEGOLDÁS: A gyerek a SAJÁT szemaforjára vár
            sem_wait(&sems->sem_child);
            
            printf("PONG\n");
            
            // MEGOLDÁS: A gyerek a SZÜLŐ szemaforját engedi el
            sem_post(&sems->sem_parent);
        }
        
        exit(0);
    } else {
        // ========== SZÜLŐ ==========
        
        for (int i = 0; i < ROUNDS; i++) {
            // MEGOLDÁS: A szülő a SAJÁT szemaforjára vár
            sem_wait(&sems->sem_parent);
            
            printf("PING\n");
            
            // MEGOLDÁS: A szülő a GYEREK szemaforját engedi el
            sem_post(&sems->sem_child);
        }
        
        wait(NULL);
        
        // Takarítás
        sem_destroy(&sems->sem_parent);
        sem_destroy(&sems->sem_child);
        munmap(sems, sizeof(SharedSems));
    }
    
    return 0;
}
