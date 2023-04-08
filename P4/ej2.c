#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t tenedor[5];
pthread_t filosofo[5];

void *comer(void *i)
{
    int id = *(int *)i; //Pasamos "i" a un puntero tipo int e igulamos "id" a su contenido
    int iz = id;
    int der = (id + 1) % 5;
    int tryL = 1;
    int tryR = 1;

    //Pedir los tenedores de la izquierda y derecha
    tryL = pthread_mutex_trylock(&tenedor[iz]);
    tryR = pthread_mutex_trylock(&tenedor[der]);

    if (tryL == 0 && tryR == 0)
    {
        //Filósofo comiendo
        usleep(10000);
        printf("Filósofo %lld esta comiendo\n", id);
    }
    else if (tryL*tryR == 0)
    {
        printf("Filósofo %d ha fallado 1 vez\n", id);
    }
    else
    {
        printf("Filósofo %d ha fallado 2 veces\n", id);
    }
    

    //Deja los tenedores
    pthread_mutex_unlock(&tenedor[iz]);
    pthread_mutex_unlock(&tenedor[der]);

    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    //Create threads
    for (int i = 0; i < 5; i++)
        pthread_create(&filosofo[i], NULL, comer, (void*)&i);

    //Wait for all threads to finish and then print results
    for (int i = 0; i < 5; i++)
        pthread_join(filosofo[i], NULL);

    //Destroy mutexes
    for (int i = 0; i < 5; i++)
        pthread_mutex_destroy(&tenedor[i]);

    return 0;
}

/* Usando _lock si que comen todos porque se espera a que los dos tenedores esten libres, mientras que si usamos
   _trylock pasa de largo si el recurso no esta disponible, por lo que solo comen dos filosofos que esten en 
   sillas opuestas de la mesa */