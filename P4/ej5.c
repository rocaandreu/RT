#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t tenedor[5];
pthread_t filosofo[5];

void *comer(void *i)
{   int id = *(int *)i; //Pasamos el puntero void "i" a puntero integer e igualamos "id" a su contenido
    int left = id;
    int right = (id + 1) % 5;
    int tryL = 1;
    int tryR = 1;

    //Conseguir dos tenedores
    while (tryL != 0 || tryR != 0)
    {
        //Pedir los tenedores de la izquierda y derecha
        tryL = pthread_mutex_trylock(&tenedor[left]);
        tryR = pthread_mutex_trylock(&tenedor[right]);

        //Desbloquearlos si no ha podido coger los dos para comer
        if (tryR == 0 && tryL != 0) pthread_mutex_unlock(&tenedor[right]);
        if (tryR != 0 && tryL == 0) pthread_mutex_unlock(&tenedor[left]);
    }

    //Filósofo comiendo
    usleep(10000);
    printf("\nFilósofo %d esta comiendo\n", id);
    
    //Deja los tenedores
    pthread_mutex_unlock(&tenedor[left]);
    pthread_mutex_unlock(&tenedor[right]);

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