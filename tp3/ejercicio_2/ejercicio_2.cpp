#include <iostream>
#include <exception>
#include <string.h>
#include <thread>
#include <vector>

using namespace std; 

/*
# ejercicio_2.cpp | Trabajo Práctico 3) Ejercicio 2) | Primera reentrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332
*/

class hiloFibonacci { 
private:
    int n;
public: 
    /*Constructor seteo el parámetro n (número de sucesión Fibonacci)*/
    hiloFibonacci() : n(0){}
    hiloFibonacci(int n) : n(n){}
    /* Método para sumar los n números de la sucesión Fibonacci)*/
    void fib(int &);
};

void hiloFibonacci::fib(int &sum)
{
    /*Método que recibe como parámetro la posición de un array donde 
    escribir la suma de los n números de la sucesión Fibonacci*/
    int fibo1 = 0;
    int fibo2 = 1;
    int prox_fibo;
    sum = 1;//Inicializo la posición del array para asegurarme que no tenga basura
    for (int i = 0 ; i < (this->n)-1 ; i++)
    {
        prox_fibo = fibo1 + fibo2;
        fibo1 = fibo2;
        fibo2 = prox_fibo;
        sum += prox_fibo;
    }
}

void suma(vector<int> fib, int &res, int tam)
{
    /*Función para sumar los elementos de un array (fib[]) 
    y escribirlos en la posición de otro array (&res)*/
    res = 0;//Inicializo la posición del array para asegurarme que no tenga basura
    for(int i = 0; i < tam; i++)
    {
        res += fib[i];
    }
}

void prod(vector<int> fib, int &res, int tam)
{
    /*Función para multiplicar los elementos de un array (fib[]) 
    y escribirlos en la posición de otro array (&res)*/
    res = 1;//Debido a que es una multiplicación inicializo en 1 la posición del array
    for(int i = 0; i < tam; i++)
    {
        res *= fib[i];
    }
}

void help()
{
    cout << "\n######    HELP Ejercicio_2    ######\n" << endl;
    cout << "Este programa dado un parámetro N de entrada" << endl;
    cout << "calcula la suma de los números de la sucesión" << endl;
    cout << "Fibonacci hasta n, siendo n un entero variable" << endl;
    cout << "desde N hasta 1. Luego, a la multiplicación de " << endl;
    cout << "estos valores, les resta la suma de los mismos " << endl;
    cout << "e imprime el resultado por pantalla." << endl;
    cout << "\nPara ejecutar el programa: \"./Ejercicio_2 N\"" << endl;
    cout << "donde N debe ser un entero mayor o igual a cero." << endl; 
}
 
int main(int argc, char** argv) 
{
    char info[]="Para más información ejecute Ejercicio_2 -h, Ejercicio_2 --help o Ejercicio_2 -?.";
    int n;
    if(argc == 1){
        cout << "¡ERROR!: Debe enviar un parámetro para ejecutar el programa" << endl;
        cout << info << endl;
        return EXIT_FAILURE;
    }
    else if(argc > 2){
        cout << "¡ERROR!: El programa debe recibir un único parámetro" << endl;
        cout << info << endl;
        return EXIT_FAILURE;
    }

    if( !strcmp("-h", argv[1]) || !strcmp("-?", argv[1]) || !strcmp("--help", argv[1]) ){
        help();
        return EXIT_SUCCESS;
    }
    /*Si el parámetro no representara un entero stoi() lanza excepción
    entonces la capturo para mostrar un mensaje más entendible por el usuario*/
    try
    {
        n = stoi(argv[1]);
    }
    catch (exception& e)
    {
        cout << "¡ERROR!: Ingrese un entero mayor a cero" << endl;
        cout << info << endl;
        return EXIT_FAILURE;
    }
    if(n <= 0){
        cout << "¡ERROR!: Ingrese un entero mayor a cero" << endl;
        cout << info << endl;
        return EXIT_FAILURE;
    }
    //Array con los threads que calcularán la suma de los n elementos de la sucesión Fibonacci
    vector<thread> fiboAcumVector;
    //Array de objetos hiloFibonacci para construir cada thread
    vector<hiloFibonacci> hf_array(n);
    //Array con el resultado de cada thread
    vector<int> fib_result(n);
    //Array con el resultado del producto (pos 1) y la suma (pos 0) de los elementos de fib_result
    int prod_sum_array[2];
    int total;

    /*Realizo un for desde n para calcular la suma hasta i de la sucesión Fibonacci en cada thread*/
    for (int i = n; i > 0; i--)
    {   
        //Asigno a cada posición de hf_array un objeto hiloFibonacci con su respectivo n
        //Posición 0 será la suma de los n números de la sucesión, y así sucesivamente hasta n
        hf_array[n-i] = hiloFibonacci(i);
        /*Instancio un thread pasándole como parámetro la referencia al método de la clase hiloFibonacci
        encargado de sumar los n números de la sucesión, el objeto hiloFibonacci con su respectivo n a utilizar
        y la posición del array de resultados que la tocará (siguiendo la misma lógica de posiciones que para hf_array)*/
        thread th(&hiloFibonacci::fib, &hf_array[n-i], std::ref(fib_result[n-i]));
        fiboAcumVector.push_back(std::move(th));
    }

    for(auto& t: fiboAcumVector)
    {
        t.join();
    }

    /*Threads de suma y producto. Reciben la referencia a las funciones de suma y producto (respectivamente) y 
    el array con los resultados de la suma de los n números de la sucesión*/
    thread thSuma(suma, fib_result, std::ref(prod_sum_array[0]), n);
    thread thProd(prod, fib_result, std::ref(prod_sum_array[1]), n);

    thSuma.join();
    thProd.join();
    
    total = prod_sum_array[1] - prod_sum_array[0];

    cout << "Resultado: " << total <<endl;

    return EXIT_SUCCESS; 
}
