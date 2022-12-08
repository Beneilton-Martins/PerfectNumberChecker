/* Checa os multiplos do thread_num das threads, apenas checa os valores menores que o threads_limit;
 * Não é preciso chegar todos os numeros visto que ao menos um fator do numero vai ser menor que o teto da raiz do numero; (ceil)
 * Nesse caso se salva os 2 fatores ( menos quando é raiz quadrada pois o fatores são iguais );
 * Exemplo: num_data = 10, num_of_threads = 4;
 * thread[0] checa os numeros <4,8> (Nesse caso a thread[0] sempre recebe o indice igual ao num_of_threads pois não faz sentido usar multiplos de 0);
 * thread[1] checa os numeros <1,5> 1 é adicionado e o 5 ignorado pois ele excede o threads_limit;
 * thread[2] checa os numeros <2,5> 2 e 5 adicionados (5 é adicionado pois ele é resultado da divisão de 2);
 * thread[3] checa os numeros <3,7> nenhum adicionado;
 * Obs1. as threads não necessariamente checam os numeros listados acima, a thread[1] nem chega a processar o valor 5.
 * Obs2. Meu programa sempre cria no minimo 2 threads e nunca deixa o usario criar mais threads do que o necessario para fazer a checagem por pares;
 * Referências:
  - Sistemas Operacionais Modernos 4ª edição - capitulo 2, processos e threads - p. 124 problema 64 - Nota: restringir busca de 1 até sqrt(N);
	- https://math.stackexchange.com/questions/63276/is-a-prime-factor-of-a-number-always-less-than-its-square-root;
	- https://sites.millersville.edu/bikenaga/number-theory/primes/primes.html;
  - http://csweb.wooster.edu/nsommer/cs212/perfect-numbers.html;
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define SIZE_OF_BUFFER 10

struct thread_data {
  unsigned long long thread_num;
  struct shared_data *shared_data;
};

struct shared_data {
  pthread_mutex_t lock;
  unsigned long long num_data;
  unsigned long long array_limit;
	unsigned long long n_of_factors;
	unsigned long long threads_limit;
  unsigned long long lowest_factor;
  unsigned long long num_of_threads;
  unsigned long long *array_of_factors;
};

void *checkingRoutine(void *t_parameter) {
  struct thread_data *t_parameters_cast = (struct thread_data *)t_parameter;

  unsigned long long current_num = t_parameters_cast->thread_num;
	
  if (current_num == 0) { //caso a thread[0] entre ela é considerada como 4 para checar os multiplos (Seguindo o exemplo teste);
    current_num += t_parameters_cast->shared_data->num_of_threads;
  }
	// 
  while (current_num <= (t_parameters_cast->shared_data->threads_limit)) {
    // Se há um fator;
    if (t_parameters_cast->shared_data->num_data % current_num == 0) {
      unsigned long long factor_aux = t_parameters_cast->shared_data->num_data / current_num;
      pthread_mutex_lock(&t_parameters_cast->shared_data->lock);

      // VERIFICAR SE O FATOR JÁ NÃO FOI ACHADO;
      if (t_parameters_cast->shared_data->num_data / current_num == t_parameters_cast->shared_data->lowest_factor) {
        // Temos todos os fatores;
        pthread_mutex_unlock(&t_parameters_cast->shared_data->lock);
        return NULL;
      }

      // Verifica se o fator faz parte de um par ou se é um quadrado perfeito;
      if (current_num != 1 && current_num != factor_aux) {
        t_parameters_cast->shared_data->n_of_factors+= 2;
      } else {
        t_parameters_cast->shared_data->n_of_factors++;
      }

      // Aumenta o array caso seja necessario;
      if (t_parameters_cast->shared_data->n_of_factors >
          t_parameters_cast->shared_data->array_limit) {
        t_parameters_cast->shared_data->array_of_factors = realloc(t_parameters_cast->shared_data->array_of_factors, sizeof(unsigned long long) * t_parameters_cast->shared_data->array_limit * 2);
        t_parameters_cast->shared_data->array_limit *= 2;
      }
			
      if (current_num != 1 && current_num != factor_aux) { // Se o numero corrente for diferente de 1 e diferente do fator auxiliar;
        t_parameters_cast->shared_data->array_of_factors[t_parameters_cast->shared_data->n_of_factors -2] = current_num;
        t_parameters_cast->shared_data->array_of_factors[t_parameters_cast->shared_data->n_of_factors -1] = factor_aux;
        if (factor_aux < t_parameters_cast->shared_data->lowest_factor) { // Checa se é menor que o atual menor fator;
          t_parameters_cast->shared_data->lowest_factor = factor_aux;
        }
      } else { // Thread[1] caso especifico, para não salvar o proprio número como fator e apenas o fator 1;
        t_parameters_cast->shared_data->array_of_factors[t_parameters_cast->shared_data->n_of_factors - 1] = current_num;
        if (current_num < t_parameters_cast->shared_data->lowest_factor) {                 
          t_parameters_cast->shared_data->lowest_factor = current_num; 
        }
      }
      pthread_mutex_unlock(&t_parameters_cast->shared_data->lock);
    }

    current_num += t_parameters_cast->shared_data->num_of_threads; //
  }
  return NULL;
}

int main(int argc, char **argv) {
  unsigned long long num = num;
  unsigned long long threads_limit = ceil(sqrt(num));
  unsigned long long n_of_threads = n_of_threads;
  if (n_of_threads > threads_limit) {
    n_of_threads = threads_limit;
  }
  //
  printf("Digite o numero que devera ser analisado: \n");
  scanf("%llu", &num);

  printf("Digite o numero de Threads para serem usadas na analise: \n");
  scanf("%llu", &n_of_threads);
  //
  pthread_t *threads = malloc(sizeof(pthread_t) * n_of_threads);
  struct shared_data *shared_data = malloc(sizeof(struct shared_data));
  shared_data->array_of_factors = malloc(sizeof(unsigned long long) * SIZE_OF_BUFFER);
  shared_data->n_of_factors = 0;
  shared_data->array_limit = SIZE_OF_BUFFER;
  shared_data->num_of_threads = n_of_threads;
  shared_data->threads_limit = threads_limit;
  shared_data->num_data = num;
  shared_data->lowest_factor = num + 1;
  pthread_mutex_init(&shared_data->lock, NULL);

  struct thread_data thread_data_struct[n_of_threads];
	
  for (int i = 0; i < n_of_threads; i++) {
    thread_data_struct[i].shared_data = shared_data;
    thread_data_struct[i].thread_num = i;
    pthread_create(&threads[i], NULL, checkingRoutine,(void *)&thread_data_struct[i]);
  }

  for (int i = 0; i < n_of_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  unsigned long long sum;
	
  if (shared_data->n_of_factors == 1) {
    sum = 1;
    printf("%llu Seu unico fator é 1.\n", num);
  } else {
    printf("Os fatores de %llu são:\n", num);
    sum = 0;
    for (int i = 0; i < shared_data->n_of_factors; i++) {
      sum += shared_data->array_of_factors[i];
      if (i + 1 < shared_data->n_of_factors) {
        printf(" %llu,", shared_data->array_of_factors[i]);
      } else {
        printf(" e %llu.\n", shared_data->array_of_factors[i]);
      }
    }
  }

  if (sum == num) {
    printf("%llu \nÉ um numero perfeito\n", num);
  } else {
    printf("%llu \nNão é um numero perfeito\n", num);
  }

  return 0;
}