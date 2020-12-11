#include <iostream>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <semaphore.h>
#include <windows.h>
#include <chrono>
#include <string>
#include <vector>
using namespace std;
#pragma comment(lib,"pthreadVC2.lib") 

int beesInHive;
auto globalStart = chrono::system_clock::now();
int beeNumber;
int honeyInHive;
const int maxHoney = 30;
sem_t semaphore;
pthread_mutex_t mutex;

void* Bee(void* args)
{
	auto start = chrono::system_clock::now();
	auto end = chrono::system_clock::now();
	int n = *((int*)args);
	srand(time(NULL) + n);
	int delay = 3000 + (rand() % 20) * 100 + (rand() % 10) * 10 + rand() % 10;
	int num = *((int*)args);
	while ((std::chrono::duration_cast<chrono::seconds>(end - start).count() <= 60)) {
		sem_wait(&semaphore);
		beesInHive--;
		pthread_mutex_lock(&mutex);
		cout << "Время [" << chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - globalStart).count() << "] "<< " Пчела [" << n << "] улетела за мёдом" << endl;
		pthread_mutex_unlock(&mutex);
		Sleep(delay);
		beesInHive++;
		pthread_mutex_lock(&mutex);
		cout << "Время [" << chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - globalStart).count() << "] "<< "Пчела [" << n << "] вернулась с медом!" << endl;
		pthread_mutex_unlock(&mutex);
		if (honeyInHive < 30) {
			honeyInHive++;
			pthread_mutex_lock(&mutex);
			cout << "Теперь тут " << honeyInHive << " мёда" << endl;
			pthread_mutex_unlock(&mutex);
		}
		sem_post(&semaphore);
		delay = 1000 + (rand() % 30) * 100 + (rand() % 10) * 10 + rand() % 10;
		Sleep(delay);
		end = std::chrono::system_clock::now();
	}
	return NULL;
}

void* Bear(void* args)
{
	auto start = chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	while ((chrono::duration_cast<std::chrono::seconds>(end - start).count() <= 60)) {
		if (honeyInHive >= 15)
		{
			if (beesInHive >= 3)
			{
				pthread_mutex_lock(&mutex);
				cout << "Время [" << chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - globalStart).count() << "] " << "Мишку покусали" << endl;
				pthread_mutex_unlock(&mutex);
				Sleep(2000);
			}
			else {
				honeyInHive = 0;
				pthread_mutex_lock(&mutex);
				cout << "Время [" << chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - globalStart).count() << "] " << "Мишка украл весь мёд" << endl;
				pthread_mutex_unlock(&mutex);
			}
		}
		end = std::chrono::system_clock::now();
	}
	return NULL;
}



int main()
{
	setlocale(LC_ALL, "Russian");
	int beeNumber;
	cout << "Введите количество пчел > 3" << endl;
	cin >> beeNumber;
	while (beeNumber < 4) {
		cout << "Неверный ввод, введите целое число больше трёх " << endl;
		cin.clear();
		cin.ignore(32767, '\n');
		cin >> beeNumber;
	}
	beesInHive = beeNumber;
	sem_init(&semaphore, 0, beeNumber - 1);
	pthread_mutex_init(&mutex, nullptr);
	int* arr = new int[beeNumber];
	vector<pthread_t> bees(beeNumber);
	pthread_t bear;
	pthread_create(&bear, NULL, Bear, NULL);
	for (int t = 0; t < beeNumber; t++)
	{
		arr[t] = t + 1;
		int rc = pthread_create(&bees[t], NULL, Bee, &arr[t]);
		if (rc)
		{
			printf("ERROR:return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}
	for (int t = 0; t < beeNumber; t++)
	{
		pthread_join(bees[t], NULL);
	}
	pthread_join(bear, NULL);
	delete[] arr;
	sem_destroy(&semaphore);
}
