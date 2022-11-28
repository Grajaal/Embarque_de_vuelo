#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#define RED "\e[0;31m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"

void tec_handler(int);
void enc_handler(int);
void asi_handler(int);
int calculatesRandom(int, int);

int main(int argc, char const *argv[]){

        if(argv[1]<=0){
            perror("El numero de asistentes no es correcto\n");
            exit(-1);
        }

        srand(time(NULL)); //Iniciacion de la semilla de numeros aleatorios 

        int numAsistentes=atoi(argv[1]);
        int numTrabajadores=numAsistentes+2; //numTrabajadores=todos menos el coordinador=tecnico+encargado+numAsistentes=2+numAsistentes
        int status;
        pid_t  *pid_trabajadores=(pid_t*)malloc(numTrabajadores*sizeof(pid_t));       
        struct sigaction tec;        
        struct sigaction enc;
        struct sigaction asi;
        int* pasajerosEmbarcados;
        pasajerosEmbarcados=(int*)malloc(sizeof(int)*numAsistentes);
        int vueloPosible, hayOverbooking, pasajeros=0;


        pid_t pid;  
        
        printf(RED"Embarque de vuelo: \n\n");
        printf("\tEl coordinador del vuelo, %d, se pone a trabajar.\n", getpid());
        printf("\tEl coordinador del vuelo manda trabajo al tecnico del avion.\n");   

        for(int i=0;i<numTrabajadores;i++){
            pid=fork();
            if(pid==-1) printf("Error");
            else if(pid==0){
                switch(i){
                    case 0:         
                        tec.sa_handler=tec_handler; //Enmascaro la sennal
                        if(-1==sigaction(SIGUSR1, &tec, NULL)){
                            perror("Error: sigaction tecnico\n");
                            return -1;
                        }
                        pause();
                    case 1: 
                        enc.sa_handler=enc_handler; //Enmascaro la sennal
                        if(-1==sigaction(SIGUSR1, &enc, NULL)){
                            perror("Error: sigacion encargado");
                            return -1;
                        }
                        pause();
                    default: 
                        asi.sa_handler=asi_handler; //Enmascaro la sennal 
                        if(-1==sigaction(SIGUSR2, &asi, NULL)){
                            perror("Error: sigaction asistentes\n");
                            return -1;
                        }
                        pause();
                }
            }else{
                *(pid_trabajadores+i)=pid;
            }
        }

        sleep(2);

        kill(*pid_trabajadores, SIGUSR1); //*(pid_trabajadores+0)=*pid_trabajadores=pid del tecnico
        if(-1==wait(&status)){
            perror("Error: wait tecnico\n");
            return -1;
        }
        vueloPosible=WEXITSTATUS(status);

        if(vueloPosible==0){
            printf(RED"\tEl vuelo NO es viable por razones meteorologicas\n");
            kill(*(pid_trabajadores), SIGTERM); //Matamos al proceso del tecnico
            kill(*(pid_trabajadores+1), SIGTERM); //Matamos al proceso del encargado 
            return -1;
        } 

        printf(RED"\tEl coordinador, tras recibir por parte del tecnico que el vuelo es posible, pide al encargado que compruebe si existe overbooking en el vuelo.\n");

        kill(*(pid_trabajadores+1), SIGUSR1);
        if(-1==wait(&status)){
            perror("Error: wait encargado\n");
            return -1;
        }
        hayOverbooking=WEXITSTATUS(status);
        if(hayOverbooking==1) printf(RED"\tHay overbooking en el vuelo\n");
        
        for(int i=2;i<numTrabajadores;i++){
            kill(*(pid_trabajadores+i), SIGUSR2);
        } 


        for(int i=2;wait(&status)!=-1;i++){
            *(pasajerosEmbarcados+i-2)=WEXITSTATUS(status);
        }

        for(int i=2;i<numTrabajadores;i++){
            pasajeros+=*(pasajerosEmbarcados+i-2);
        }

        if(hayOverbooking==1) pasajeros-=10;
    
        printf(RED"\tEl coordinador recibe que el numero de pasajeros es de %d\n", pasajeros);

        free(pasajerosEmbarcados);
        free(pid_trabajadores);
        return 0;
        
}

void tec_handler(int s){
    printf(YEL"\t\tEl tecnico del avion, %d, se pone a trabajar.\n", getpid());
    sleep(calculatesRandom(3, 6));
    //if(calculatesRandom(0, 1)==1) exit(1);
    //else exit(0);      
    exit(1);
}

int calculatesRandom(int min, int max){
    return rand() % (max-min+1) + min;
}

void enc_handler(int s){
    printf(YEL"\t\tEl encargado, %d, se pone a trabajar.\n", getpid());
    int hayOverbooking=calculatesRandom(0,1);
    if(hayOverbooking==0) printf(YEL"\t\tEl encargado dice que el vuelo NO tiene overbooking\n");
    else printf(YEL"\t\tEl encargado dice que el vuelo SI tiene overbooking.\n");
    sleep(2);
    exit(hayOverbooking);
}

void asi_handler(int s){
    srand(getpid());
    sleep(calculatesRandom(3, 6));
    printf(BLU"\t\t\tAsistente %d: embarcando\n", getpid());
    exit(calculatesRandom(20, 30));
}
