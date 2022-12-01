/*
Grupo:
		Gabriel Pampuch
		Guilherme de Matos Oliani
		Rafael Kozlowski Andreola
*/


/*
	CURITIBA 11/2022
	UNIVERSIDADE POSITIVO
	PROGRAMACAO CONCORRENTE/DESENVOLVIMENTO DE SISTEMAS
	
	TRABALHO 2
	- ADAPTACAO DO "PROBLEMA DO BANHEIRO UNICO" (VER DETALHES NA ESPECIFICACAO)
	
	TAREFA
	- COMPLETAR/COMPLEMENTAR AS FUNCOES "CAT" E "DOG" NO FINAL DESTE ARQUIVO
	
	REGRAS
	- VOCE PODE ADICIONAR CODIGO A VONTADE DESDE QUE SEJA A PARTIR DA LINHA COM O COMENTARIO "TODO"
	- VOCE PODE INCLUIR CABECALHOS A VONTADE
	- NADA DO QUE ESTA ESCRITO PODE SER APAGADO
	
	INFORMACOES IMPORTANTES
	- A ACAO "EATING" EH CRITICA, A ACAO "PLAYING" EH NAO-CRITICA
	- DEVE HAVER EXCLUSAO MUTUA ENTRE GATOS E CACHORROS NA AREA DE COMIDA
	- O NUMERO DE PETS NA AREA DE COMIDA NAO PODE ULTRAPASAR O VALOR DA MACRO "MAX_PETS"
	- NAO DEVE HAVER STARVATION DE GATOS OU CACHORROS
	
	DICAS
	- HA UMA CLASSE "SEMAFORO" DISPONIVEL PARA USO
	- LEMBRE-SE DE COMPILAR PARA C++11 (-std=c++11) OU SUPERIOR
	- A COMPREENSAO DO CODIGO EH PARTE DO TRABALHO
*/

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class semaphore
{
	long count;
	std::mutex mtx;
	std::condition_variable cv;
	
	public:
	
	semaphore(long const c = 0) : count(c) {}
	
	semaphore(semaphore const &) = delete;
	semaphore(semaphore &&) = default;
	semaphore & operator=(semaphore const &) = delete;
	semaphore & operator=(semaphore &&) = default;
	~semaphore() = default;
	
	void acquire() //aka "wait", "down", "p"
	{
		auto lock = std::unique_lock<std::mutex>(mtx);
		while(!count) cv.wait(lock);
		--count;
	}
	
	void release() //aka "signal", "up", "v"
	{
		auto lock = std::unique_lock<std::mutex>(mtx);
		++count;
		cv.notify_one();
	}
};

#define MAX_PETS 16
#define MAX_SLEEP_US 4
#define NUM_THREADS 100

void do_stuff(int const id, char const * kind, char const * action)
{
	std::printf("pet #%d (%s) started %s...\n", id, kind, action);
	std::this_thread::sleep_for(std::chrono::microseconds(std::rand() % MAX_SLEEP_US));
	std::printf("pet #%d (%s) stopped %s...\n", id, kind, action);
}

void cat(int const);
void dog(int const);

int main()
{
	auto pets = std::vector<std::thread>(NUM_THREADS);
	
	for(int i = 0; i < pets.size(); ++i)
	{
		pets.at(i) = std::thread(i % 2 ? cat : dog, i);
	}
	
	for(int i = 0; i < pets.size(); ++i)
	{
		pets.at(i).join();
	}
	
	return 0;
}

semaphore 3(1), 2(1);

std::mutex mutex_a;
std::mutex mutex_b;
int count_a = 0;
int count_b = 0;

void cat(int const id)
{
	while (true)
	{
		// do_stuff(id, "cat", "playing");
		a.acquire();

		if (count_b >= MAX_PETS) {
			a.release();
			continue;
		}
		mutex_b.lock();
		count_b++;

		if(count_b == 1) {
			s.acquire();
		}

		mutex_b.unlock();
		a.release();
		std::printf(" %d ", count_b);
		do_stuff(id, "cat", "eating");
		mutex_b.lock();
		count_b--;

		if (count_b == 0) {
			s.release();
		}

		mutex_b.unlock();
	}
}

void dog(int const id)
{
	while (true)
	{
		// do_stuff(id, "dog", "playing");
		a.acquire();

		if (count_a >= MAX_PETS) {
			a.release();
			continue;
		}

		mutex_a.lock();
		count_a++;

		if (count_a == 1) {
			s.acquire();
		}

		mutex_a.unlock();
		a.release();
		std::printf(" %d ", count_a);

		do_stuff(id, "dog", "eating");
		mutex_a.lock();
		count_a--;

		if (count_a == 0) {
			s.release();
		}
		
		mutex_a.unlock();
	}
}
