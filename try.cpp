#include <ncurses.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#define A 9
std::mutex m;
std::condition_variable cv[5];
std::condition_variable shop;
int Que[5];
int kasa = 0;

void bigThread(int name)
{

    std::unique_lock<std::mutex> lk1(m);
    cv[0].wait(lk1);
    Que[0] = name;
    lk1.unlock();

    std::unique_lock<std::mutex> lk2(m);
    cv[1].wait(lk2);
    Que[0] = 0;
	Que[1] = name;
    lk2.unlock();
    cv[0].notify_one();

    std::unique_lock<std::mutex> lk3(m);
    cv[2].wait(lk3);
    Que[1] = 0;
	Que[2] = name;
    lk3.unlock();
    cv[1].notify_one();

    std::unique_lock<std::mutex> lk4(m);
    cv[3].wait(lk4);
    Que[2] = 0;
	Que[3] = name;
    lk4.unlock();
    cv[2].notify_one();

    std::unique_lock<std::mutex> lk5(m);
    cv[4].wait(lk5);
    Que[3] = 0;
	Que[4] = name;
    lk5.unlock();
    cv[3].notify_one();


    std::unique_lock<std::mutex> lks(m);
    shop.wait(lks);
    Que[4] = 0;
	kasa = name;
    cv[4].notify_one();
    lks.unlock();
    srand(time(NULL));
    std::this_thread::sleep_for(std::chrono::seconds(rand() % 5 + 1));
    m.lock();
    kasa = 0;
    m.unlock();
    shop.notify_one();



}

void view()
{
	int iter = 0;
	int count = 0;
	while (true)
	{
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
		m.lock();
		move(3, iter + 1);
		printw("%d", Que[iter]);
		refresh();
		iter++;
		if (iter == 5)
		{
			move(4, 2);
			printw("%d", count++);
			iter = 0;
		}
		move (3, 10);
		printw("%d", kasa);
		refresh();
		m.unlock();
	}
}


int main()
{
	initscr();
	curs_set(0);
	std::thread T[A];
	std::thread view_thread = std::thread(view);

	for (int i = 0; i < A; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		T[i] = std::thread(bigThread, i + 1);
	}

    for(int i=0;i<5;i++)
    {
        cv[i].notify_one();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    shop.notify_one();

	for (int i = 0; i < A; i++)
	{
		T[i].join();
	}
	view_thread.join();
	getch();
	endwin();
	return 0;
}
