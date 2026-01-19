#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    // Kiírjuk a kezdeti állapotot
    printf("Indulás előtt: Egy folyamat vagyok (PID: %d)\n", getpid());

    // ITT TÖRTÉNIK A VARÁZSLAT: Kettéágazik a folyamat
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed"); // Hiba történt (pl. elfogyott a memória)
        return 1;
    }

    if (pid == 0) {
        // Ez a kód csak a GYEREK folyamatban fut le
        printf("👶 Én vagyok a Gyerek! A PID-em: %d, a Szülőm PID-je: %d\n", 
               getpid(), getppid());
    } else {
        // Ez a kód csak a SZÜLŐ folyamatban fut le
        printf("👨 Én vagyok a Szülő! A PID-em: %d, a Gyerekem PID-je: %d\n", 
               getpid(), pid);
    }

    return 0;
}
