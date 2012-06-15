#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace std;

 bool forever = true;

void sighandler(int sig)
{
    cout<< "Signal " << sig << " caught..." << endl;

	forever = false;
}

int main(int argc, char *argv[])
{
    signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
	signal(SIGINT, &sighandler);

        char name[32];
        cout << gethostname(name,32) << " " << name << endl;

	while(forever)
	{
	}
} 


