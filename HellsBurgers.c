
#include <stdio.h>      // libreria estandar
#include <stdlib.h>     // para usar exit y funciones de la libreria standard
#include <string.h>
#include <pthread.h>    // para usar threads
#include <semaphore.h>  // para usar semaforos
#include <unistd.h>


#define LIMITE 50

	pthread_mutex_t mutex1;
	pthread_mutex_t mutex2;
	pthread_mutex_t mutex3;

int equipoganador= 0;

//creo estructura de semaforos 
struct semaforos {

	sem_t sem_cortarextra;
	sem_t sem_hornearpan;
	sem_t sem_cortar;
    sem_t sem_mezclar;
    sem_t sem_salar;	
	sem_t sem_armarmedallon;
	sem_t sem_cocinar;
	sem_t sem_armarhamburguesa;
	sem_t sem_armarhamburguesa_aux;
	sem_t sem_armarhamburguesa_aux2;
	
	//poner demas semaforos aqui
};

//creo los pasos con los ingredientes
struct paso {
   char accion [LIMITE];
   char ingredientes[4][LIMITE];
   
};

//creo los parametros de los hilos 
struct parametro {
 int equipo_param;
 struct semaforos semaforos_param;
 struct paso pasos_param[8];
};

//funcion para imprimir las acciones y los ingredientes de la accion
void* imprimirAccion(void *data, char *accionIn) {

	
	FILE* resul;
	resul = fopen("resultado.txt", "a");

	struct parametro *mydata = data;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos 
	int i;
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){
		printf("\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		fprintf(resul,"\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);

		//calculo la longitud del array de ingredientes
		int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
		//indice para recorrer array de ingredientes
		int h;
		//printf("\tEquipo %d -----------ingredientes : ----------\n",mydata->equipo_param); 
			for(h = 0; h < sizeArrayIngredientes; h++) {
				//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
				if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
							printf("\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
							fprintf(resul,"\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
				}
			}
		}
	}
	fclose(resul);
}

//funcion para tomar de ejemplo
void* armarhamburguesa(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "armarhamburguesa";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
    sem_wait(&mydata->semaforos_param.sem_armarhamburguesa);    //representa P(s_ma).
    sem_wait(&mydata->semaforos_param.sem_armarhamburguesa_aux);    
    sem_wait(&mydata->semaforos_param.sem_armarhamburguesa_aux2);    
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 1000000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
	
	if(equipoganador==0){
		equipoganador=mydata->equipo_param;
	}
    pthread_exit(NULL);
}

//funcion para tomar de ejemplo
void* cocinar(void *data) {
	pthread_mutex_lock(&mutex2);               //entrada a la sección crítica.
	//creo el nombre de la accion de la funcion 
	char *accion = "cocinar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_cocinar);    //representa P(s_ma).
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 1000000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_armarhamburguesa);
	pthread_mutex_unlock(&mutex2);              //salida sección crítica.
    pthread_exit(NULL);
}

//funcion para tomar de ejemplo
void* armarmedallon(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "armarmedallon";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
    sem_wait(&mydata->semaforos_param.sem_armarmedallon);    //representa P(s_ma).	
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 1000000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_cocinar);
    pthread_exit(NULL);
}

//funcion para tomar de ejemplo
void* salar(void *data) {
	pthread_mutex_lock(&mutex1);               //entrada a la sección crítica.
	//creo el nombre de la accion de la funcion 
	char *accion = "salar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
    sem_wait(&mydata->semaforos_param.sem_salar);    //representa P(s_ma).
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 1000000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_armarmedallon);
    pthread_mutex_unlock(&mutex1);              //salida sección crítica.
    pthread_exit(NULL);
}

//funcion para tomar de ejemplo
void* mezclar(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "mezclar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
    sem_wait(&mydata->semaforos_param.sem_mezclar);    //representa P(s_ma).
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 1000000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    //sem_post(&mydata->semaforos_param.sem_salar);
    sem_post(&mydata->semaforos_param.sem_salar);
    pthread_exit(NULL);
}


//funcion para tomar de ejemplo
void* cortar(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "cortar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
    sem_wait(&mydata->semaforos_param.sem_cortar);    //representa P(s_ma).
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 1000000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_mezclar);
    pthread_exit(NULL);
}

//funcion para tomar de ejemplo
void* cortarextra(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "cortarextra";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
    sem_wait(&mydata->semaforos_param.sem_cortarextra);    //representa P(s_ma).
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 1000000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_armarhamburguesa_aux);
    pthread_exit(NULL);
}




//funcion para tomar de ejemplo
void* hornearpan(void *data) {
	pthread_mutex_lock(&mutex3);               //entrada a la sección crítica.
	//creo el nombre de la accion de la funcion 
	char *accion = "hornearpan";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
    sem_wait(&mydata->semaforos_param.sem_hornearpan);    //representa P(s_ma).
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 1000000 );
    pthread_mutex_unlock(&mutex3);              //salida sección crítica.
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_armarhamburguesa_aux2);
	
    pthread_exit(NULL);
}

void* mostrar_ganador() {
	FILE* resul;
	resul = fopen("resultado.txt", "a");
	printf("\n ----------------Equipo Ganador: %d ---------------- \n",equipoganador);
	fprintf(resul,"\nEquipo Ganador: %d \n",equipoganador);
	fclose(resul);
}



void* ejecutarReceta(void *i) {
	
	//variables semaforos
	sem_t sem_cortarextra;
	sem_t sem_hornearpan;
	sem_t sem_cortar;
    sem_t sem_mezclar;
    sem_t sem_salar;	
	sem_t sem_armarmedallon;
	sem_t sem_cocinar;
	sem_t sem_armarhamburguesa;
	sem_t sem_armarhamburguesa_aux;
	sem_t sem_armarhamburguesa_aux2;

	//crear variables semaforos aqui
	
	//variables hilos
	pthread_t p1; 
	pthread_t p2; 
	pthread_t p3; 
	pthread_t p4; 
	pthread_t p5; 
	pthread_t p6; 
	pthread_t p7; 
	pthread_t p8; 
	//crear variables hilos aqui
	
	//numero del equipo (casteo el puntero a un int)
	int p = *((int *) i);
	
	printf("Ejecutando equipo %d \n", p);

	//reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	//seteo los valores al struct
	
	//seteo numero de grupo
	pthread_data->equipo_param = p;

	//seteo semaforos
	pthread_data->semaforos_param.sem_mezclar = sem_cortarextra;
	pthread_data->semaforos_param.sem_mezclar = sem_hornearpan;
	pthread_data->semaforos_param.sem_mezclar = sem_cortar;
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
	pthread_data->semaforos_param.sem_salar = sem_salar;
	pthread_data->semaforos_param.sem_cocinar = sem_cocinar;
	pthread_data->semaforos_param.sem_armarmedallon = sem_armarmedallon;
	pthread_data->semaforos_param.sem_armarhamburguesa = sem_armarhamburguesa;
	pthread_data->semaforos_param.sem_armarhamburguesa = sem_armarhamburguesa_aux;
	pthread_data->semaforos_param.sem_armarhamburguesa = sem_armarhamburguesa_aux2;
	//setear demas semaforos al struct aqui
	

	FILE *archivo;
	archivo = fopen("pasos.txt", "r");
	if (archivo == NULL)
		{
    	exit(EXIT_FAILURE); 
	}
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	int linea=0;
	while ((read = getline(&line, &len, archivo)) != -1) { //-1 es que se termino 
	int init_size = strlen(line);
	int i = 0;
	char *ptr = strtok(line,";");
		while (ptr != NULL)
		{
			if(i ==0){
				strcpy(pthread_data->pasos_param[linea].accion, ptr);
				ptr = strtok(NULL, ";");
			}else{
				strcpy(pthread_data->pasos_param[linea].ingredientes[i], ptr);
				ptr = strtok(NULL, ";");
			}
			i++;
		}	
			linea++;
    }
    fclose(archivo);

	//seteo las acciones y los ingredientes (Faltan acciones e ingredientes) ¿Se ve hardcodeado no? ¿Les parece bien?
   /* strcpy(pthread_data->pasos_param[0].accion, "cortar");
	strcpy(pthread_data->pasos_param[0].ingredientes[0], "ajo");
    strcpy(pthread_data->pasos_param[0].ingredientes[1], "perejil");
 	strcpy(pthread_data->pasos_param[0].ingredientes[2], "cebolla");

	strcpy(pthread_data->pasos_param[1].accion, "mezclar");
	strcpy(pthread_data->pasos_param[1].ingredientes[0], "ajo");
    strcpy(pthread_data->pasos_param[1].ingredientes[1], "perejil");
 	strcpy(pthread_data->pasos_param[1].ingredientes[2], "cebolla");
	strcpy(pthread_data->pasos_param[1].ingredientes[3], "carne picada");
	
	strcpy(pthread_data->pasos_param[3].accion, "salar");
	
	strcpy(pthread_data->pasos_param[4].accion, "armarmedallon");

	strcpy(pthread_data->pasos_param[4].accion, "cocinar");
	
	strcpy(pthread_data->pasos_param[5].accion, "hornearpan");

	strcpy(pthread_data->pasos_param[6].accion, "cortarextra");

	strcpy(pthread_data->pasos_param[7].accion, "armarhamburguesa");
*/
	
	
	//inicializo los semaforos
	sem_init(&(pthread_data->semaforos_param.sem_cortarextra),0,1);
	sem_init(&(pthread_data->semaforos_param.sem_hornearpan),0,1);
	sem_init(&(pthread_data->semaforos_param.sem_cortar),0,1);
	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_salar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarmedallon),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_cocinar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarhamburguesa),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarhamburguesa_aux),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarhamburguesa_aux2),0,0);
	//inicializar demas semaforos aqui


	//creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos 
    int rc;
    rc = pthread_create(&p1,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortarextra,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	//crear demas hilos aqui
	rc = pthread_create(&p2,                           //identificador unico
                            NULL,                          //atributos del thread
                                hornearpan,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia

	rc = pthread_create(&p3,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
    rc = pthread_create(&p4,                           //identificador unico
                            NULL,                          //atributos del thread
                                mezclar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	//crear demas hilos aqui
	rc = pthread_create(&p5,                           //identificador unico
                            NULL,                          //atributos del thread
                                salar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia

	rc = pthread_create(&p6,                           //identificador unico
                            NULL,                          //atributos del thread
                                armarmedallon,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
    rc = pthread_create(&p7,                           //identificador unico
                            NULL,                          //atributos del thread
                                cocinar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	//crear demas hilos aqui
	rc = pthread_create(&p8,                           //identificador unico
                            NULL,                          //atributos del thread
                                armarhamburguesa,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
																
								
																
	
	
	//join de todos los hilos
	pthread_join (p1,NULL);
	pthread_join (p2,NULL);
	pthread_join (p3,NULL);
	pthread_join (p4,NULL);
	pthread_join (p5,NULL);
	pthread_join (p6,NULL);
	pthread_join (p7,NULL);
	pthread_join (p8,NULL);
	//crear join de demas hilos


	//valido que el hilo se alla creado bien 
    if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     }

	 
	//destruccion de los semaforos 
	sem_destroy(&sem_cortarextra);
	sem_destroy(&sem_hornearpan);
	sem_destroy(&sem_cortar);
	sem_destroy(&sem_mezclar);
	sem_destroy(&sem_salar);
	sem_destroy(&sem_armarmedallon);
	sem_destroy(&sem_cocinar);
	sem_destroy(&sem_armarhamburguesa);
	sem_destroy(&sem_armarhamburguesa_aux);
	sem_destroy(&sem_armarhamburguesa_aux2);

	//destruir demas semaforos 
	
	//salida del hilo
	 pthread_exit(NULL);
}


int main ()
{
	//creo los nombres de los equipos 
	int rc;
	int *equipoNombre1 =malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 =malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 =malloc(sizeof(*equipoNombre3));
	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;

      pthread_mutex_init ( &mutex1, NULL);                         //se inicializa el mutex.
      pthread_mutex_init ( &mutex2, NULL);                         //se inicializa el mutex.
      pthread_mutex_init ( &mutex3, NULL);                         //se inicializa el mutex.


	//creo las variables los hilos de los equipos
	pthread_t equipo1; 
	pthread_t equipo2;
	pthread_t equipo3;
 
	//inicializo los hilos de los equipos
    rc = pthread_create(&equipo1,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre1); 

    rc = pthread_create(&equipo2,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre2);

    rc = pthread_create(&equipo3,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre3);

   if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     } 

	//join de todos los hilos
	pthread_join (equipo1,NULL);
	pthread_join (equipo2,NULL);
	pthread_join (equipo3,NULL);

	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);
	pthread_mutex_destroy(&mutex3);

	mostrar_ganador();

    pthread_exit(NULL);
}


//Para compilar:   gcc HellsBurgers.c -o ejecutable -lpthread
//Para ejecutar:   ./ejecutable
