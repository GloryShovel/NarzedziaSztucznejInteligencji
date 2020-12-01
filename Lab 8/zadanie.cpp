#include <iostream>
#include <vector>
#include <math.h>

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

int main( int argc, char** argv )
{
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

    phenotype = decode(genotype);
    for(auto e: phenotype){
        std::cout << e.integral << "," << e.precision << std::endl;
    }

    std::cout << leviFunction( (phenotype.at(0).precision + phenotype.at(0).integral), (phenotype.at(1).precision + phenotype.at(1).integral)) << std::endl;
    std::cout << fitness(phenotype) << std::endl;

 return 0;
}
