#include <iostream>
#include <random>
#include <math.h>
#include "functional"

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

void randomProbe(std::function<float(float, float)> f, float domainMin, float domainMax, int iterations){
    std::random_device rd;
    std::uniform_real_distribution<float> distribution(domainMin, domainMax);

    float bestX = distribution(rd);
    float bestY = distribution(rd);
    float best = f(bestX, bestY);

    for (int i = 0; i < iterations; i++) {
        float x = distribution(rd);
        float y = distribution(rd);
        if (f(x, y) < best) {
            bestX = x;
            bestY = y;
            best = f(x, y);
        }
    }
    std::cout << "Najlepsze rozwiazanie to X: " << bestX << ", Y:" << bestY << " A jego wartosc to:" << best << std::endl;
}

void hillClimbing(std::function<float(float, float)> f, float domainMin, float domainMax){
    std::random_device rd;
    std::uniform_real_distribution<float> distribution(domainMin, domainMax);

    float bestX = distribution(rd);
    float bestY = distribution(rd);
    std::vector<float> best = {bestX, bestY, f(bestX, bestY)};
    std::vector<float> nextBest = {bestX, bestY, best.at(2)};

    do{
        best = nextBest;

        std::vector<std::vector<float>> neighbours;
        neighbours.push_back({bestX-0.1f, bestY, f(bestX-0.1f, bestY)});
        neighbours.push_back({bestX+0.1f, bestY, f(bestX+0.1f, bestY)});
        neighbours.push_back({bestX, bestY-0.1f, f(bestX, bestY-0.1f)});
        neighbours.push_back({bestX, bestY+0.1f, f(bestX, bestY+0.1f)});

        for(int i = 0; i < neighbours.size(); i++){
            if(neighbours.at(i).at(2) < nextBest.at(2)){
                nextBest = neighbours.at(i);
            }
        }

    }while(best != nextBest);
    std::cout << "Najlepsze rozwiazanie to X: " << best.at(0) << ", Y:" << best.at(1) << " A jego wartosc to:" << best.at(2) << std::endl;

}


//argv[2] - function, argv[3] - iterations
void randemonium( int argc, char** argv){
    if(argc > 2){
        int option = atoi(argv[2]);
        if(option == 1) {
            //sphere
            std::random_device rd;
            float domainMin = -10, domainMax = 10;
            std::uniform_real_distribution<float> distribution(domainMin, domainMax);
            std::uniform_int_distribution<int> distrib(1, 10);

            int n = distrib(rd);
            float bestX = distribution(rd);
            float best = sphereFunction(bestX, n);

            for (int i = 0; i < atoi(argv[3]); i++) {
                float x = distribution(rd);
                if (sphereFunction(x, n) < best) {
                    bestX = x;
                    best = sphereFunction(x, n);
                }
            }
            std::cout << "Najlepsze rozwiazanie to X: " << bestX << ", N:" << n << " A jego wartosc to:" << best << std::endl;

//            randomProbe(sphereFunction, -10, 10);

        }else if(option == 2) {
            //beale
            randomProbe(bealeFunction, -4.5, 4.5, atoi(argv[2]));

        } else if(option == 3) {
            //booth
            randomProbe(boothFunction, -10, 10, atoi(argv[2]));

        }else{
            std::cout << "Nie znaleziono takiej flagi!" << std::endl;
        }

    }else{
        std::cout << "Za malo arumentow wywolania." << std::endl;
    }
}

//argv[2] - function, argv[3] - min domain restriction, argv[4] - max domain restriction
void hill(int argc, char** argv ){
        int option = atoi(argv[2]);
        if(option == 1) {
            //sphere
            std::random_device rd;
            std::uniform_real_distribution<float> distribution(atof(argv[3]), atof(argv[4]));
            std::uniform_int_distribution<int> distrib(1, 10);

            int n = distrib(rd);
            float bestX = distribution(rd);
            std::vector<float> best = {bestX, sphereFunction(bestX, n)};
            std::vector<float> nextBest = {bestX, best.at(1)};

            do{
                best = nextBest;

                std::vector<std::vector<float>> neighbours;
                neighbours.push_back({bestX-0.1f, sphereFunction(bestX-0.1f, n)});
                neighbours.push_back({bestX+0.1f, sphereFunction(bestX+0.1f, n)});

                for(int i = 0; i < neighbours.size(); i++){
                    if(neighbours.at(i).at(1) < nextBest.at(1)){
                        nextBest = neighbours.at(i);
                    }
                }

            }while(best != nextBest);
            std::cout << "Najlepsze rozwiazanie to X: " << best.at(0) << ", N:" << n << " A jego wartosc to:" << best.at(1) << std::endl;

        }else if(option == 2) {
            //beale (-4.5 4,5)
            hillClimbing(bealeFunction, atof(argv[3]), atof(argv[4]));

        } else if(option == 3) {
            //booth (-10 10)
            hillClimbing(boothFunction, atof(argv[3]), atof(argv[4]));

        }else{
            std::cout << "Nie znaleziono takiej flagi!" << std::endl;
        }
}

//argv[1] - type of heuristic,
// randemonium : argv[2] - function, argv[3] - iterations
// hill: argv[2] - function, argv[3] - min domain restriction, argv[4] - max domain restriction
int main( int argc, char** argv ) {
    std::string functionName;
    functionName.push_back(*argv[1]);
    if(argc > 3){
        if(functionName == "h"){
            hill(argc, argv);
        } else{
//          if not specified it will use randemonium
            randemonium(argc, argv);
        }
    }else{
        std::cout << "Podano za malo argumentow" << std::endl;
    }

    return 0;
}
