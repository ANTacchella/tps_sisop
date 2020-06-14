#include <iostream> 
#include <thread>
#include <vector>

using namespace std; 

// A callable object 
class ejercicio_2 { 
public: 
    ejercicio_2(int n);
    
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
    //virtual ~ejercicio_2();

}; 


/*
///DESTRUCTOR
ejercicio_2::~ejercicio_2()
{
    //dtor
}*/

///CONSTRUCTOR
ejercicio_2::ejercicio_2(int n)
:n(n){}

// A dummy function 
void foo(int n) 
{ 
    for (int i = 0; i < n; i++) { 
        cout << "Thread using function"
               " pointer as callable\n"; 
    } 
} 



  
int main() 
{ 
    cout << "Threads 1 and 2 and 3 "
         "operating independently" << endl; 
    
    int n = 3;
    //int ThreadVector[n];
    vector<thread> ThreadVector;

    for (int i = n; i > 0; i--)
    {
        cout << "N: " << n << "i: " << i << endl;
        thread th1(ejercicio_2(i));

        ThreadVector.emplace_back([&](){function(a, b, Obj, Obj2)}); // Pass by reference here, make sure the object lifetime is correct
    }


    for(auto& t: Threadvector)
    {
        t.join();

    }

/*
    // This thread is launched by using  
    // function pointer as callable 
    thread th1(foo, 3); 
  
    // This thread is launched by using 
    // function object as callable 
    thread th2(ejercicio_2(), 3); 
  
    // Define a Lambda Expression 
    auto f = [](int x) { 
        for (int i = 0; i < x; i++) 
            cout << "Thread using lambda"
             " expression as callable\n"; 
    }; 
  
    // This thread is launched by using  
    // lamda expression as callable 
    thread th3(f, 3); 
*/


/* 
  
    // Wait for the threads to finish 
    // Wait for thread t1 to finish 
    th1.join(); 
    cout << "Threads 1 llego" << endl; 
 
    // Wait for thread t2 to finish 
    th2.join(); 
  
    // Wait for thread t3 to finish 
    th3.join(); 
*/  
    return 0; 
}