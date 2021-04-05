#include <string>
#include "FA.h"
using namespace std;

int main()
{
    ifstream fin("test1.txt");
    FA myFA;
    myFA.read(fin);
    myFA.print();
    myFA.NFAtoDFA();
    myFA.print();
    myFA.DFAminimize();
    myFA.print();
    return 0;
}
