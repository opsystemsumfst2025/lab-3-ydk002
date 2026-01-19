#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> // Ez kell a wait()-hez!

int main() {
    printf("[Szülő] Indulás... PID: %d\n", getpid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }

    if (pid == 0) {
        // GYEREK folyamat
        printf("[Gyerek] Elindultam, PID: %d\n", getpid());
        printf("[Gyerek] Dolgozom 2 másodpercig...\n");
        sleep(2);
        printf("[Gyerek] Kész vagyok, kilépek.\n");
        exit(0);
    } else {
        // SZÜLŐ folyamat
        printf("[Szülő] A gyerekem PID-je: %d\n", pid);
        
        // Itt hívjuk meg a wait()-et!
        printf("[Szülő] Megvárom, amíg a gyerek végez...\n");
        
        wait(NULL); 
        
        printf("[Szülő] A gyerek befejezte, rendesen eltakarítottam. Nincs zombi!\n");
    }

    return 0;
}
