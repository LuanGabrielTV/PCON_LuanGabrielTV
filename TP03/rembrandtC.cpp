#include <iostream>
#include <pthread.h>
#include <SFML/Graphics.hpp>
#include <math.h>
#include <chrono>
#include <string.h>
#include <SFML/Graphics/RenderTexture.hpp>

using namespace std;

#define SIZE 500
// #define nthreads 8

// imagem global
sf::Image image;
// numero de threads global
int nthreads = 1;
typedef struct
{
    int idThread;
} t_Args;

void *desenhaImagem(void *arg)
{
    auto start = chrono::high_resolution_clock::now();
    int r1 = 255, g1 = 0, b1 = 127, r2 = 0, g2 = 153, b2 = 255;
    float r, g, b;
    t_Args *args = (t_Args *)arg;
    int idThread = args->idThread;
    int inicio = idThread * (SIZE / nthreads);
    float percent;
    int counter = 1;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = inicio; j < inicio + (SIZE / nthreads); j++)
        {
            percent = ((float)counter / ((SIZE) * (SIZE / nthreads))) * 0.9;

            r = percent * sqrt(pow(r2 - r1, 2)) - r1;
            g = percent * sqrt(pow(g2 - g1, 2)) - g1;
            b = percent * sqrt(pow(b2 - b1, 2)) - b1;

            sf::Color color(r, g, b);
            image.setPixel(i, j, color);
            counter++;
        }
    }
    chrono::duration<double> tempo = chrono::high_resolution_clock::now() - start;
    cout << (tempo).count() << endl;

    pthread_exit(NULL);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        int i;
        for (i = 0; i < argc; i++)
            nthreads = atoi(argv[i]);
    }
    auto start = chrono::high_resolution_clock::now();
    sf::RenderWindow window(sf::VideoMode(SIZE, SIZE), "My window"); // nova janela

    image.create(SIZE, SIZE, sf::Color::White);
    chrono::duration<double> tempo = chrono::high_resolution_clock::now() - start;
    cout << (tempo).count() << "\n"
         << endl;

    // cria as threads
    pthread_t tid_sistema[nthreads];
    int t;
    t_Args *arg;

    for (t = 0; t < nthreads; t+=2)
    {
        arg = (t_Args *)malloc(sizeof(t_Args));
        if (arg == NULL)
        {
            printf("--ERRO: malloc()\n");
            return -1;
        }
        arg->idThread = t;

        if (pthread_create(&tid_sistema[t], NULL, desenhaImagem, (void *)arg))
        {
            printf("--ERRO: pthread_create()\n");
            return -1;
        }
    }

    //--espera todas as threads terminarem
    for (t = 0; t < nthreads; t+=2)
    {
        if (pthread_join(tid_sistema[t], NULL))
        {
            printf("--ERRO: pthread_join() \n");
            return -1;
        }
    }

    start = chrono::high_resolution_clock::now();
    sf::Texture imageTexture;
    imageTexture.loadFromImage(image);
    sf::Sprite imageSprite(imageTexture);
    window.clear(sf::Color::Black);
    window.draw(imageSprite);
    window.display();

    std::string name = "tests/ex/";
    name += std::to_string(nthreads);
    name += ".jpg";
    window.capture().saveToFile(name);
    tempo = chrono::high_resolution_clock::now() - start;
    cout << "\n"
         << (tempo).count() << endl;

    return 0;
}
