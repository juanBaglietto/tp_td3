/*
 * main.c
 *
 *  Created on: 27/08/2016
 *      Author: juan
 */

#include "main.h"


Lista lista_hijos = NULL;
Lista lista_revivir_hijos = NULL;
Lista lista_tiempo_hijos= NULL;
struct timeval ti;
struct timeval tf;

pid_t pid_padre,aux,aux_h;
int cant_procesos = 0;
int  t_vida = 0, t_demora = 0;
int fd;
/*Handler*/

void sigchld_handler() {

	int status;
	Info_hijo *encontrado;
	Info_hijo nuevo_hijo;

	pid_t rx_pid;
	do
	{
		rx_pid = waitpid(-1, &status, WNOHANG);
		if(rx_pid>0){
			if((encontrado=Buscar_PID(lista_hijos,rx_pid))!=NULL){
				if(encontrado->tiempo_vida==0){
					TRACE_MID("(%d) Murio con dignidad mi hijo (%d)\n",pid_padre,rx_pid);
					//printf("(%d) Murio con dignidad mi hijo (%d)\n",pid_padre,rx_pid);
					Borrar_PID(&lista_hijos,rx_pid);
				}
				else{
					TRACE_MID("(%d) Murio antes de tiempo mi hijo (%d) le falta vivir (%d)\n",pid_padre,rx_pid,encontrado->tiempo_vida);
					printf("ALERTA: Hijo <%d> finalizó <%d> mseg antes de lo especificado\n",rx_pid,encontrado->tiempo_vida);
					nuevo_hijo.pid_hijo = 0;
					nuevo_hijo.demora = 0;
					nuevo_hijo.tiempo_vida =encontrado->tiempo_vida;
					Insertar_al_final(&lista_revivir_hijos,nuevo_hijo);
					Borrar_PID(&lista_hijos,rx_pid);
				}
			}
		}
	}while (rx_pid >0);

}

void  sigalrm_handler(){


	Decremetar_tiempo(lista_hijos);

}

void sigusr1_handler(){
//	pid_t mi_pid;
//
//	mi_pid=getpid();
//	printf("(%d) Adios mundo cruel!\n", mi_pid);

	exit(1);

}
/*Funciones*/

void int_signals(){

	/*Reemplazo del controlador de SIGCHLD*/

	struct sigaction ctrl_nuevo_sigchld;
	struct sigaction ctrl_viejo_sigchld;


		if (sigemptyset(&ctrl_nuevo_sigchld.sa_mask) == ERROR) {
			perror("sigemptyset-sig_block_mask");

			exit(EXIT_FAILURE);
		}
		if (sigaddset(&ctrl_nuevo_sigchld.sa_mask, SIGSTOP) == ERROR) {
			perror("sigaddset-sig_block_mask");

			exit(EXIT_FAILURE);
		}
		ctrl_nuevo_sigchld.sa_flags = SA_SIGINFO | SA_NOCLDSTOP | SA_RESTART;
		ctrl_nuevo_sigchld.sa_sigaction = sigchld_handler;

		if (sigaction(SIGCHLD, &ctrl_nuevo_sigchld, &ctrl_viejo_sigchld) == ERROR) {
			perror("signal-SIGCHLD, sigchld_handler");

			exit(EXIT_FAILURE);
		}

}

//validacion de argumentos
void validar_arg(int cant_arg, char const *arg_v[]){

		if (cant_arg!= 4) {
			fprintf(stderr, "Argumento insuficiente \n");
			exit(1);
		}
		if (arg_v[1] < 0) {
			fprintf(stderr, "Error en la cantidad de procesos ingresados\n");
			exit(1);
		}
		if (atoi(arg_v[2]) < 100) {
			fprintf(stderr, "Debe ingresar un tiempo de vida mayor a 100 ms\n");
			exit(1);
		}
		if (atoi(arg_v[3]) < 100 && atoi(arg_v[3]) != 0) {
			fprintf(stderr, "Debe ingresar una demora mayor a 100 ms o igual a 0\n");
			exit(1);
		}
}

pid_t crear_hijo(int tiempo_vida){

	pid_t pid_hijo;
	pid_t mi_pid_hijo;
	pid_t padre;
	Info_hijo nuevo_hijo;
    char HoraAct[128];
	struct sigaction ctrl_nuevo_sigusr1;
	struct sigaction ctrl_viejo_sigusr1;
	time_t  hora;
	struct tm *ts;
	int tdiff,cant;
	int sem_archivo;
	char *mi_string;

	pid_hijo=fork();

	if(pid_hijo!=0) /*codigo del padre */
	{

		nuevo_hijo.pid_hijo = pid_hijo;
		nuevo_hijo.demora = t_demora;
		nuevo_hijo.tiempo_vida = tiempo_vida;
		Insertar_al_final(&lista_hijos,nuevo_hijo);



	}
	else  			  /*codigo del hijo */
	{
		gettimeofday(&ti,NULL);
		hora = time(0);
		ts = localtime(&hora);
		strftime(HoraAct, sizeof(HoraAct), "%H:%M:%S", ts);
		//printf("%s\n", HoraAct);

		mi_pid_hijo=getpid();
		padre=getppid();
		/*Reemplazo del controlador de SIGUSR1*/
		ctrl_nuevo_sigusr1.sa_flags = SA_SIGINFO  | SA_RESTART;
		ctrl_nuevo_sigusr1.sa_sigaction = sigusr1_handler;
		if (sigaction(SIGUSR1, &ctrl_nuevo_sigusr1, &ctrl_viejo_sigusr1) == ERROR) {
			perror("signal-SIGCHLD, sigchld_handler");

			exit(EXIT_FAILURE);
		}

		sem_archivo=abre_sem(key); //abro el semaforo creado por el padre
		mi_string= malloc(BUFF_SIZE);

		while(1)
		{
			gettimeofday(&tf,NULL);
			tdiff=((tf.tv_sec*1000000)+tf.tv_usec)-((ti.tv_sec*1000000)+ti.tv_usec);
			sprintf(mi_string,"Hijo <%d> de <%d>, Vivo hace <%d> mseg [%s.%ld]\n" ,mi_pid_hijo,padre,tdiff/1000,HoraAct,ti.tv_usec/1000);

			sem_P(sem_archivo);

			if((cant=write(fd,mi_string, strlen(mi_string)))==-1)
			{
				perror("write: ");

			}


			//printf("Hijo <%d> de <%d>, Vivo hace <%d> mseg\n", mi_pid_hijo,padre,tdiff/1000);

			sem_V(sem_archivo);
			usleep(50*1000); //imprimo el msj cada 50ms

		}
	}
	return	pid_hijo;
}
/*funcion encargada de relazar los proceos que se terminaron antes de tiempo
 * devuelve 0 si no se relanzo ningun proceoso o la contidad de proceoso relanzados */

void relanzar_procesos(){

	pid_t pid_hijo_revivido;
	pNodo aux=lista_revivir_hijos;

	if((ListaVacia(lista_revivir_hijos))==0){
		do{
			pid_hijo_revivido=crear_hijo(aux->hijo.tiempo_vida);
			TRACE_MID("(%d) revivi a un hijo con pid %d y tiempo de vida %d \n",pid_padre,pid_hijo_revivido,aux->hijo.tiempo_vida);
			//printf("(%d) revivi a un hijo con pid %d y tiempo de vida %d \n",pid_padre,pid_hijo_revivido,aux->hijo.tiempo_vida);
			aux = aux->siguiente;
		}while(aux!=NULL && pid_hijo_revivido!=0);
		if(pid_hijo_revivido!=0){
			BorrarLista(&lista_revivir_hijos);
		}
	}
}

void time_stamp(unsigned long seg_convertir, int hora,int minutos,int segundos){

	hora= (int)(seg_convertir/3600);
	minutos=(int)((seg_convertir-hora*3600)/60);
	segundos=seg_convertir-(hora*3600+minutos*60);

}

/* abre_sem: Abrir un semáforo que otro proceso ya creó */



int main(int argc, char const *argv[]) {

	pid_t pid_chld;
	int cont_demora=0,cont_hijos=0;
	time_t  hora;
	struct tm *ts;
    char FechaAct[128];
	char *mi_archivo;
	mi_archivo= malloc(ARCH_SIZE);
	pid_padre = getpid();					 // pid padre
	int_signals();
	validar_arg(argc,argv);

	cant_procesos = atoi(argv[1]);
	t_vida = atoi(argv[2]);  				//el timpo de vida enta en ms y usleep usa us
	t_demora = atoi(argv[3]);

	hora = time(0);
	ts = localtime(&hora);
	strftime(FechaAct, sizeof(FechaAct), "%d-%m-%Y_%H:%M:%S", ts);
	sprintf(mi_archivo,"[%s]registro.log" ,FechaAct);


	if((fd= open (mi_archivo,O_CREAT | O_APPEND |  O_RDWR,S_IRWXU ))==-1)

	{
		perror("open: "); //escibe el tipo de error por el que se proboco la falla
		exit(2);
	}
	key = ftok(mi_archivo, 'E');

	crea_sem(key,1); //key y valor inical del semaforo, lo creo liberado para que el primer hijo a escribir lo pueda tomar

	signal(SIGALRM,sigalrm_handler);
	ualarm(T_SIGALRM_USEG,T_SIGALRM_USEG); //seteo para tener una interrpcion por sigalrm cada 1 ms

	do {

		pid_chld = crear_hijo(t_vida);

		if (pid_chld != 0) {


			TRACE_MID("(%d) se creo un nuevo hijo con numero de PID :%d\n", pid_padre,pid_chld);
			//printf("(%d) se creo un nuevo hijo con numero de PID :%d\n", pid_padre,pid_chld);
			cont_hijos++;

			for (cont_demora = 0; cont_demora < DIV_DEMORA; ++cont_demora) {
				usleep(t_demora * (1000/DIV_DEMORA)); //divido el tiempo de demora para que el padre pueda analizar la tabla para relanzar procesos
				relanzar_procesos();
			}
		}
	} while (cont_hijos < cant_procesos && pid_chld != 0);

	if (pid_chld != 0) {

		while((ListaVacia(lista_hijos))==0){
			relanzar_procesos();
			pause();
		}
		printf("(%d) Tuve %d hijos, murieron todos, que desgracia!\n", pid_padre,cont_hijos);
		BorrarLista(&lista_hijos);
		close(fd);
		exit(1);
	}


	return 0;
}


