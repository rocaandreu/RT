#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


// Declaración de semáforos
sem_t sem_even, sem_odd;

// Función que será ejecutada por los hilos
void *print_id(void *thread_id) {
    
    int id = *(int *)thread_id;

    if (id == 10) 
    {
        sem_wait(&sem_even);
        printf("%d\n", id);
        sem_post(&sem_even);    
        sem_post(&sem_odd);     // El último par da paso al primer impar
    } 
    else if (id % 2 == 0) 
    {   
        sem_wait(&sem_even);    // Esperar a que el semáforo correspondiente esté disponible
        printf("%d\n", id);     // Imprimir el identificador
        sem_post(&sem_even);    // Reactivar el semaforo
    } 
    else 
    {
        sem_wait(&sem_odd);
        printf("%d\n", 10-id);
        sem_post(&sem_odd);
    }      

    pthread_exit(NULL);
}

int main() 
{
    // Inicializar los semáforos
    sem_init(&sem_even, 0, 1);
    sem_init(&sem_odd, 0, 0);

    // Declarar e inicializar los hilos
    pthread_t th[10];
    int ids[10];
    for (int i = 0; i < 10; i++) {
        ids[i] = i + 1;
        pthread_create(&th[i], NULL, print_id, (void *)&ids[i]);
    }

    // Esperar a que los hilos terminen
    for (int i = 0; i < 10; i++) {
        pthread_join(th[i], NULL);
    }

    // Destruir los semáforos
    sem_destroy(&sem_even);
    sem_destroy(&sem_odd);
    return 0;
}