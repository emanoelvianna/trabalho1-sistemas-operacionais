#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/**
 * comando de compilação:
 * gcc jantar_dos_filosofos.c -o main -lpthread
 * comando de execução:
 * ./main <quantidade_filosofos> <quantidade_tempo>
 */

typedef struct
{
	int filosofo_id;
	int pensou;
	int comeu;
	int nao_conseguiu_pegar_os_garfos;
} thread_data;

//variáveis globais
thread_data* dados_dos_filosofos;
pthread_mutex_t* ponteiro_mutex_garfos;
pthread_t* ponteiro_threads_filosofos;
int quantidade_filosofos;
double quantidade_tempo;

//protótipos de funções
void* funcao_filosofo(void* thread_argument);
void pensar(double tempo);
void comer(double tempo);
double get_time();

/**
 * retorna o momento atual do programa em segundos
 */
double get_time()
{
	struct timeval time_value;
	gettimeofday(&time_value, 0);	
	return time_value.tv_sec + (time_value.tv_usec / 1e6);
}

/**
 * emula o comportamente de um filósofo
 */
void* funcao_filosofo(void* thread_argument)
{
	thread_data* dados_do_filosofo = (thread_data*)thread_argument;

	int filosofo_id = dados_do_filosofo->filosofo_id;

	double tempo_inicial = get_time();
	double tempo_atual = 0.0;
	double tempo_decorrido = 0.0;

	//pensa por 5 segundos
	pensar(5.0);
	//contabiliza pensar
	dados_do_filosofo->pensou = (dados_do_filosofo->pensou) + 1;

	while(tempo_decorrido < quantidade_tempo)
	{		
		int garfo_esquerda_id = filosofo_id - 1;

		//se é o filosofo de id 0, o garfo da esquerda é o garfo (quantidade_filosofos-1)
		if(filosofo_id == 0)
		{
			garfo_esquerda_id = quantidade_filosofos - 1;
		}

		//se o filósofo conseguiu pegar o garfo da esquerda
		if(pthread_mutex_trylock(&ponteiro_mutex_garfos[garfo_esquerda_id]) == 0)
		{
			int garfo_direita_id = filosofo_id;

			//se o filósofo conseguiu pegar o garfo da direita
			if(pthread_mutex_trylock(&ponteiro_mutex_garfos[garfo_direita_id]) == 0)
			{
				//come por 2 microsegundos
				comer(0.000002);
				//contabiliza comer
				dados_do_filosofo->comeu = (dados_do_filosofo->comeu) + 1;

				//solta os dois garfos na mesa
				pthread_mutex_unlock(&ponteiro_mutex_garfos[garfo_esquerda_id]);
				pthread_mutex_unlock(&ponteiro_mutex_garfos[garfo_direita_id]);

				//pensa por 5 segundos
				pensar(5.0);
				//contabiliza pensar
				dados_do_filosofo->pensou = (dados_do_filosofo->pensou) + 1;
			}
			//se o filósofo não conseguiu pegar o garfo da direita
			else
			{
				//solta o garfo da esquerda na mesa
				pthread_mutex_unlock(&ponteiro_mutex_garfos[garfo_esquerda_id]);
				//nao_conseguiu_pegar_os_garfos
				dados_do_filosofo->nao_conseguiu_pegar_os_garfos = (dados_do_filosofo->nao_conseguiu_pegar_os_garfos) + 1;

				//pensa por alguns segundos (entre 0 e 3 segundos)
				pensar(rand() % 4); 
				//contabiliza pensar
				dados_do_filosofo->pensou = (dados_do_filosofo->pensou) + 1;
			}
		}
		//se o filósofo não conseguiu pegar o garfo da esquerda
		else
		{
			//nao_conseguiu_pegar_os_garfos
			dados_do_filosofo->nao_conseguiu_pegar_os_garfos = (dados_do_filosofo->nao_conseguiu_pegar_os_garfos) + 1;

			//pensa por alguns segundos (entre 0 e 3 segundos)
			pensar(rand() % 4);
			//contabiliza pensar
			dados_do_filosofo->pensou = (dados_do_filosofo->pensou) + 1; 
		}

		//contabiliza tempo atual
		tempo_atual = get_time();
		tempo_decorrido = tempo_atual - tempo_inicial;
	}

	//printf("filosofo:%d - tempo:%f\n",filosofo_id,tempo_decorrido);

	pthread_exit(NULL);
}

/**
 * emula o comportamente de um filósofo pensando
 */
void pensar(double tempo)
{
	sleep(tempo);
}

/**
 * emula o comportamente de um filósofo comendo
 */
void comer(double tempo)
{
	sleep(tempo);
}

int main(int argc, char* argv[])
{
	//a quantidade de filósofos é passada por parâmetro no main
	quantidade_filosofos = atoi(argv[1]);

	//a quantidade de tempo para execução da aplicação é passada por parâmetro no main
	quantidade_tempo = atof(argv[2]);

	//quantidade_filosofos deve ser sempre >= 2!!!
	if(quantidade_filosofos < 2)
	{
		quantidade_filosofos = 2;
	}

	//alocação dinâmica de memória
	dados_dos_filosofos = (thread_data *) malloc(quantidade_filosofos * sizeof(thread_data));
	ponteiro_mutex_garfos = (pthread_mutex_t*) malloc(quantidade_filosofos * sizeof(pthread_mutex_t));
	ponteiro_threads_filosofos = (pthread_t*) malloc(quantidade_filosofos * sizeof(pthread_t));

	//inicializa o ponteiro do tipo pthread_mutex_init
	for(int i = 0; i < quantidade_filosofos; i++)
	{		
		pthread_mutex_init(&ponteiro_mutex_garfos[i], NULL);
	}

	//cria threads
	for(int i = 0; i < quantidade_filosofos; i++)
	{	
		dados_dos_filosofos[i].filosofo_id = i;
		dados_dos_filosofos[i].pensou = 0;
		dados_dos_filosofos[i].comeu = 0;
		dados_dos_filosofos[i].nao_conseguiu_pegar_os_garfos = 0;

		pthread_create(&ponteiro_threads_filosofos[i], NULL, funcao_filosofo, (void*)&dados_dos_filosofos[i]);
	}

	//aguarda o encerramento de todas as threads
	for(int i = 0; i < quantidade_filosofos; i++)
	{
		pthread_join(ponteiro_threads_filosofos[i], NULL);
	}

	//imprime relatório
	for(int i = 0; i < quantidade_filosofos; i++)
	{	
		printf("FILOSOFO %d:\n", dados_dos_filosofos[i].filosofo_id = i);
		printf("pensou %d vezes\n", dados_dos_filosofos[i].pensou);
		printf("comeu %d vezes\n", dados_dos_filosofos[i].comeu);
		printf("não conseguiu pegar os garfos %d vezes\n\n", dados_dos_filosofos[i].nao_conseguiu_pegar_os_garfos);
	}
}
