#include <iostream> 
#include <thread>

using namespace std; 

// A callable object 
class hiloFibonacci { 
public: 
    hiloFibonacci(int n);
    
    int n;
    
    void operator()(int x) 
    { 
        for (int i = 0; i < x; i++)
        {
            cout << "Tasdfasdf\n";
            cout << "X: " << x << "i: " << i << endl;
        }
    }

    //destructor
    //virtual ~hiloFibonacci();

}; 

/*
///DESTRUCTOR
hiloFibonacci::~hiloFibonacci()
{
    //dtor
}*/

///CONSTRUCTOR
hiloFibonacci::hiloFibonacci(int n)
:n(n){}

int main() 
{ 

    return 0; 
}