# Sistemas Operativos y Redes

## MiniTp 2 (Semaforos)

## Introducción
En la cocina de Hell’s Burgers compiten tres equipos de cocineros. ¿Quién será el primero en entregar 2 hamburguesas al jurado?
La dificultad de la competencia es que la cocina no cuenta con el equipamiento individual para cada equipo, sino que algunos de ellos son compartidos. Se tienen que ordenar para cocinar los medallones ya que solo tienen una plancha para todos los equipos y esta cocina un medallón a la vez, existe solo un horno que cocina dos panes a la vez y un solo salero.

## Implementación
Para poder llevar a cabo la tarea, se desarrollo un programa en C el cual, trabajando con hilos y semaforos, devuelve exitosamente como resultado cual fue el equipo ganador.
Para lograr esto, me vi obligado a adaptarme al uso de semaforos para validar el camino a seguir en la receta y, tambien, en el uso de los mutex para indicar las zonas criticas y evitar el uso del mismo semaforo en ciertas ocasiones (Como por ejemplo salar, cocinar o hornear pan)

Para compilar:   gcc HellsBurgers.c -o ejecutable -lpthread
Para ejecutar:   ./ejecutable

## Psuedocodigo
```	
mutex1  //Semaforo mutex compartidos 
mutex2  //Semaforo mutex compartidos
mutex3  //Semaforo mutex compartidos 
	

main();{

Thread equipo 1 (ejecutarReceta)
Thread equipo 2 (ejecutarReceta)
Thread equipo 3 (ejecutarReceta)
  mostrarganador();
}


ejecutarReceta(){
Thread 1 (cortar)
Thread 2 (mezclar)
Thread 3 (salar)
Thread 4 (armarmedallon)
Thread 5 (cocinar)
Thread 6 (hornearpan)
Thread 7 (cortarextra)
Thread 8 (armarhamburguesa)

}


Cortar() {
sem_wait (Cortar)
  imprimirAccion(Cortar)
sem_post (mezclar)
}

mezclar(){
sem_wait (mezclar)
  imprimirAccion(mezclar)
sem_post (salar)
}

salar(){
sem_wait (salar)
mutex1_lock()
  imprimirAccion(salar)
mutex1_unlock()
sem_post (armarmedallon)
}

armarmedallon(){
sem_wait (armarmedallon)
  imprimirAccion(armarmedallon)
sem_post (cocinar)
}

cocinar(){
sem_wait (cocinar)
mutex2_lock()
  imprimirAccion(cocinar)
mutex2_unlock()
sem_post (armarhamburguesa)
}

hornearpan{
sem_wait (hornearpan)
mutex3_lock()
  imprimirAccion(hornearpan)
mutex3_unlock()
sem_post (armarhamburguesa_aux)
}

cortarextra(){
sem_wait (cortarextra)
  imprimirAccion(cortarextra)
sem_post (armarhamburguesa_aux2)
}

armarhamburguesa(){
sem_wait (armarhamburguesa)
sem_wait (armarhamburguesa_aux)
sem_wait (armarhamburguesa_aux2)
  imprimirAccion(armarhamburguesa)
}	```

	
  
## Problemas Encontrados
Me encontre con distintos problemas a lo largo del desarrollo.
### Llamada a la acción "armarhamburguesa" previo a terminar los requisitos: 
Me surgió la duda de como hacer el sem_post a llamar hamburguesa, ya que previo debe cumplir con los los requisitos de tener el medallon armado, lechuga y tomate cortados y el pan horneado. Para lograr esto, agregue 2 semafotos auxiliares 
	sem_t sem_armarhamburguesa_aux;
	sem_t sem_armarhamburguesa_aux2;
  
y en la funcion armarhamburguesa, agregue:

sem_wait(&mydata->semaforos_param.sem_armarhamburguesa);   //esta viene derivada de cocinar 
sem_wait(&mydata->semaforos_param.sem_armarhamburguesa_aux);  //esta viene derivada de hornearpan   
sem_wait(&mydata->semaforos_param.sem_armarhamburguesa_aux2);  //esta viene derivada de cortarextra  

Esto obliga al programa a aguardar que se completen los 3 requisitos.

### Uso de los semaforos mutex: 
Me surgió un inconveniente que por mas que inicializaba los semaforos mutex, no se respetaba su uso. El error estaba en que los declaraba dentro del Struct Semaforo y se inicializaban en ejecutarReceta(). Lo resolví declarandolos al principio del codigo e inicializandolos en el main(). De esta forma, todos los equipos compartian los mismos semaforos mutex.
  

## Conclusión
Se pudo desarrollar el programa sin grandes inconvenientes y logre adquirir los conocimientos para lograr lo propuesto utilizando hilos y semaforos. Además, aprendi como utilizar archivos externos tanto para lectura como escritura con el desarrollo.

