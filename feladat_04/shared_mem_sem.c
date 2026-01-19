#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>

// Struktúra: A számláló és a LAKAT (szemafor) egy helyen van
typedef struct {
    int counter;
    sem_t mutex;
} SharedData;

int main() {
    // Közös memória foglalása a struktúrának
    SharedData *data = mmap(NULL, sizeof(SharedData), 
                            PROT_READ | PROT_WRITE, 
                            MAP_SHARED | MAP_ANONYMOUS, 
                            -1, 0);
    
    if (data == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }
    
    data->counter = 0;
    
    // SZEMAFOR INICIALIZÁLÁS (A LAKAT ELKÉSZÍTÉSE)
    // 1. param: a szemafor címe
    // 2. param: 1 = folyamatok között megosztott
    // 3. param: 1 = KEZDŐÉRTÉK (1 = Nyitva, be lehet lépni)
    if (sem_init(&data->mutex, 1, 1) != 0) {
        perror("sem_init failed");
        return 1;
    }

    printf("Kezdőérték: %d\n", data->counter);
    printf("Szinkronizált növelés indul...\n\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }

    if (pid == 0) {
        // GYEREK folyamat
        for (int i = 0; i < 100000; i++) {
            sem_wait(&data->mutex);  // 🔒 ZÁRÁS (Várakozás, ha foglalt)
            data->counter++;         // Kritikus szakasz (biztonságos)
            sem_post(&data->mutex);  // 🔓 NYITÁS (Jöhet a következő)
        }
        exit(0);
    } else {
        // SZÜLŐ folyamat
        for (int i = 0; i < 100000; i++) {
            sem_wait(&data->mutex);  // 🔒 ZÁRÁS
            data->counter++;         // Kritikus szakasz
            sem_post(&data->mutex);  // 🔓 NYITÁS
        }
        
        wait(NULL); // Megvárjuk a gyereket
        
        printf("=================================\n");
        printf("Várt érték:    200 000\n");
        printf("Kapott érték:  %d\n", data->counter);
        printf("=================================\n");
        
        if (data->counter == 200000) {
            printf("\n✅ SIKER! A szemafor megvédte az adatot!\n");
        }
        
        // Takarítás
        sem_destroy(&data->mutex);
        munmap(data, sizeof(SharedData));
    }

    return 0;
}
