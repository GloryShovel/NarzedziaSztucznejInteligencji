#include <iostream>
#include <random>
#include <math.h>

float sphereFunction(float x, int n){
    float result = 0;
    for (int i = 0; i < n; i++) {
        result += x*x;
    }
    return result;
}

float bealeFunction(float x, float y){
    return std::pow(1.5-x+(x*y),2)+std::pow(2.25-x+std::pow(x*y,2),2)+std::pow(2.625-x+std::pow(x*y,3),2);
}

float boothFunction(float x, float y){
    return std::pow(x+2*y-7,2)+std::pow(2*x+y-5,2);
}

int main( int argc, char** argv ) {
    std::random_device rd;
    int option = atoi(argv[1]);

    if(argc > 2){
        if(option == 1) {
            //sphere
            float domainMin = -10, domainMax = 10;
            std::uniform_real_distribution<float> distribution(domainMin, domainMax);
            std::uniform_int_distribution<int> distrib(1, 10);

            int n = distrib(rd);
            float bestX = distribution(rd);
            float best = sphereFunction(bestX, n);

            for (int i = 0; i < atoi(argv[2]); i++) {
                float x = distribution(rd);
                if (sphereFunction(x, n) < best) {
                    bestX = x;
                    best = sphereFunction(x, n);
                }
            }
            std::cout << "Najlepsze rozwiazanie to X: " << bestX << ", N:" << n << " A jego wartosc to:" << best << std::endl;

        }else if(option == 2) {
            //beale
            float domainMin = -4.5, domainMax = 4.5;
            std::uniform_real_distribution<float> distribution(domainMin, domainMax);

            float bestX = distribution(rd);
            float bestY = distribution(rd);
            float best = bealeFunction(bestX, bestY);

            for (int i = 0; i < atoi(argv[2]); i++) {
                float x = distribution(rd);
                float y = distribution(rd);
                if (bealeFunction(x, y) < best) {
                    bestX = x;
                    bestY = y;
                    best = bealeFunction(x, y);
                }
            }
            std::cout << "Najlepsze rozwiazanie to X: " << bestX << ", Y:" << bestY << " A jego wartosc to:" << best << std::endl;

        } else if(option == 3) {
            //booth
            float domainMin = -10, domainMax = 10;
            std::uniform_real_distribution<float> distribution(domainMin, domainMax);

            float bestX = distribution(rd);
            float bestY = distribution(rd);
            float best = boothFunction(bestX, bestY);

            for (int i = 0; i < atoi(argv[2]); i++) {
                float x = distribution(rd);
                float y = distribution(rd);
                if (boothFunction(x, y) < best) {
                    bestX = x;
                    bestY = y;
                    best = boothFunction(x, y);
                }
            }
            std::cout << "Najlepsze rozwiazanie to X: " << bestX << ", Y:" << bestY << " A jego wartosc to:" << best << std::endl;

        }else{
            std::cout << "Nie znaleziono takiej flagi!" << std::endl;
        }

    }else{
        std::cout << "Za malo arumentow wywolania." << std::endl;
    }

    return 0;
}
