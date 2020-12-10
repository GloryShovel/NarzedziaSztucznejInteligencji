#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <random>

std::random_device rd;

struct bigFloat{
    long integral;
    double precision;
};

//My goal function of choice
double leviFunction(double x, double y){
    return (pow(sin(3*M_PI*x), 2)) + (pow(x-1, 2) * (1+pow(sin(3*M_PI*y), 2))) + (pow((y-1),2) * (1+pow(sin(2*M_PI*y), 2)));
}

double fitness(std::vector<bigFloat> phenotype){
    return 1/(1+leviFunction( (phenotype.at(0).precision + phenotype.at(0).integral), (phenotype.at(1).precision + phenotype.at(1).integral)));
}

void writeGenotype(std::vector<bool> genotype){
    std::cout << "Genotype";
    for(int i = 0; i < genotype.size()-1; i++){
        if(i % 32 == 0){
            std::cout << std::endl;
        }
        std::cout << genotype.at(i) << ",";
    }
    std::cout << genotype.at(genotype.size()-1) << std::endl << std::endl;
}

void writePhenotype(std::vector<bigFloat> phenotype){
    std::cout << "Goal: " << leviFunction( (phenotype.at(0).precision + phenotype.at(0).integral), (phenotype.at(1).precision + phenotype.at(1).integral)) << std::endl;
    std::cout << "Fitness: " << fitness(phenotype) << std::endl;
    std::cout << std::endl;
}

void writePhenotypeInfo(std::vector<bigFloat> phenotype){
    for(auto e: phenotype){
        std::cout << e.integral << ", " << e.precision << std::endl;
    }
    std::cout << "Goal: " << leviFunction( (phenotype.at(0).precision + phenotype.at(0).integral), (phenotype.at(1).precision + phenotype.at(1).integral)) << std::endl;
    std::cout << "Fitness: " << fitness(phenotype) << std::endl;
    std::cout << std::endl;
}

/*
 * Decodes genotype to phenotype
 *
 * Return X and Y as vector<float> from vector<bool> (size = 128)
 *
 * decodes as 2^1, 2^2, 2^3 ... 2^-1, 2^-2, 2^-3 (for both halves)
 */
std::vector<bigFloat> decode( std::vector<bool> genotype){
    std::vector<bigFloat> phenotype;
    phenotype.resize(2);
    for (int i = 0; i < phenotype.size(); i++) {
        long integral = 0;
        double precision = 0;
        //set to 0 for calculating X of pheromone
        int start = 0;
        //start from middle if calculating Y of pheromone
        if(i){
            start = genotype.size()/2;
        }
        for (int j = 1; j <= genotype.size()/4; j++) {
            integral += (genotype.at(start + j -1) * std::pow(2, j));
            precision += (genotype.at(start + j + genotype.size()/4 -1) * (1/std::pow(2,  j)));
        }

        phenotype.at(i).integral = integral;
        phenotype.at(i).precision = precision;
    }

    return phenotype;
}

/*
 * Code phenotype back to genotype
 *
 * Returns vector<bool> (size = 128) from vector<float> (size = 2)
 *
 * codes as 2^1, 2^2, 2^3 ... 2^-1, 2^-2, 2^-3 (for both halves)
 */
std::vector<bool> code(std::vector<bigFloat> phenotype){
    std::vector<bool> genotype;
    genotype.resize(128);

    long x = phenotype.at(0).integral, y = phenotype.at(1).integral;
    double xPrecision = phenotype.at(0).precision, yPrecision = phenotype.at(1).precision;
//    std::cout << "X: " << x << ", prec: " << xPrecision << ", Y: " << y << ", prec: " << yPrecision << std::endl;

    int start = genotype.size()/4;
    for (int i = 1; i <= genotype.size()/4; i++) {
        long divisor = std::pow(2, start - i +1);
        double divisorPrecision = std::pow(2, -1*i);

        //First halve
        if(x >= divisor){
            genotype.at(start -i) = true;
            std::cout << x << ", ";
            x -= divisor;
        }
        if(xPrecision >= divisorPrecision) {
            genotype.at(start +i -1) = true;
            xPrecision -= divisorPrecision;
        }
        //Second halve
        if(y >= divisor){
            genotype.at(3*start -i) = true;
            y -= divisor;
        }
        if(yPrecision >= divisorPrecision) {
            genotype.at(3*start +i -1) = true;
            yPrecision -= divisorPrecision;
        }
    }

    return genotype;
}

/*
 * Takes population and finds population.size() amount of partners to be compared in two
 */
std::vector<std::vector<bool>> selectionRoulette(std::vector<std::vector<bool>> population){
    std::vector<std::vector<bool>> partners;

    //Sum up all fitness
    double fitnessOverhaul = 0.0;
    for (auto e: population){
        std::vector<bigFloat> phenotype = decode(e);
        fitnessOverhaul += fitness(phenotype);
    }

    std::uniform_real_distribution<double> distribution(0, fitnessOverhaul);
    for (int i = 0; i < population.size(); i++) {
        double fitnessAccumulator = 0.0;
        double choice = distribution(rd);
        //Select partners
        for (auto e: population){
            std::vector<bigFloat> phenotype = decode(e);
            fitnessAccumulator += fitness(phenotype);
            if(fitnessAccumulator > choice){
                partners.push_back(e);
                break;
            }
        }
    }
    return partners;
}

/*
 * Selects 3 specimen form population to tournament add only the best to partners
 */
std::vector<std::vector<bool>> selectionTournament(std::vector<std::vector<bool>> population) {
    std::uniform_int_distribution<int> distribution(0, population.size() - 1);
    std::vector <std::vector<bool>> partners;

    for (int i = 0; i < population.size(); ++i) {
        std::vector <std::vector<bool>> participants;

        for (int i = 0; i < 3; i++) {
            participants.push_back(population.at(distribution(rd)));
        }

        std::vector<bool> best = participants.at(0);
        for (auto e: participants) {
            if (fitness(decode(e)) > fitness(decode(best))) {
                best = e;
            }
        }
        partners.push_back(best);
    }
    std::cout << partners.size() << std::endl;


    return partners;
}

/*
 * Take partners and combine them in two to make two children
 */
std::vector<std::vector<bool>> cross(std::vector<std::vector<bool>> partners){
    int genotypeSize = partners.at(0).size();
    std::uniform_int_distribution<int> distribution(0, genotypeSize-1);
    std::vector<std::vector<bool>> newGeneration;

    for(int i=0; i <= partners.size()-2; i+=2){

        //Point where to cross each parent
        int crossPoint = distribution(rd);
//        std::cout << "crossing point " << crossPoint << std::endl;

        //Creating two children
        std::vector<bool> firstChild;
        firstChild.resize(genotypeSize);
        std::vector<bool> secondChild;
        secondChild.resize(genotypeSize);

        //Crossing two partners
        for (int j = 0; j < genotypeSize; j++) {
            if(j < crossPoint){
                firstChild.at(j) = partners.at(i).at(j);
                secondChild.at(j) = partners.at(i+1).at(j);
            }else{
                firstChild.at(j) = partners.at(i+1).at(j);
                secondChild.at(j) = partners.at(i).at(j);
            }
        }

        //Saving children to new generation
        newGeneration.push_back(firstChild);
        newGeneration.push_back(secondChild);
    }

    return newGeneration;
}

/*
 * Takes genotype, mutates it on random position and returns it
 * Chance as default is 0.1 and should be between 0 and 1
 */
std::vector<bool> mutate(std::vector<bool> genotype, double chance=0.1){
    std::uniform_int_distribution<int> distribution(0, genotype.size()-1);
    std::uniform_real_distribution<double> odds(0, 1);

    int position = distribution(rd);

    if(chance > odds(rd)){
        genotype.at(position) = (genotype.at(position) +1) % 2;
    }

    return genotype;
}

int main( int argc, char** argv )
{
    std::vector<std::vector<bool>> population;
    std::vector<bool> genotype;
    std::vector<bigFloat> phenotype;

    //Filling genotype
    for (int i = 0; i < 128; i++) {
        if( (i >= 32 && i < 64) || (i >= 96 && i < 128)){
            genotype.push_back(true);
        }else {
            genotype.push_back(false);
        }
    }

    //Fills population with mutations of first genotype (should be randomly generated)
    for (int i = 0; i < 10; i++) {
        std::vector<bool> genotypeTMP = mutate(genotype, 1.0);
        population.push_back(genotypeTMP);
    }

    for(auto e: population){
        writePhenotype(decode(e));
    }
    for (int i = 0; i < 20; i++) {
        population = cross(selectionTournament(population));
    }
    std::cout << std::endl << "New Generation --------------------------------" << std::endl;
    for(auto e: population){
        writePhenotype(decode(e));
    }

 return 0;
}
