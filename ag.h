#ifndef AG_H
#define AG_H

#include <vector>
#include <algorithm>
#include <chipmunk.h>
#include <SDL.h>
#include <SDL_image.h>
#include "enemy.h"
#include "utils.h"

using namespace std;

class ag
{
    public:
        ag(cpSpace* spc, SDL_Texture* enemySprites, bool elitism, float mutation_rate);
        virtual ~ag(){};
        void evolve(vector<enemy*>& generation, vector<DNA>& nextGeneration);
        DNA getRandomDNA();
    protected:
    private:
        DNA crossover(enemy* e1, enemy* e2);
        void mutate(DNA* dna);
        cpSpace* spc;
        SDL_Texture* enemySprites;
        float bestScore; //mantem o score do melhor para forcar elitismo
        float mutation_rate;
        bool elitism;
};

#endif // AG_H
