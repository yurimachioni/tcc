#ifndef CHAO_H
#define CHAO_H

#include <chipmunk.h>
#include <sdl.h>
#include <sdl_image.h>
#include "utils.h"

#define DEFAULT_GROUND_VAR rand() % 8 + 2
#define DEFAULT_GROUND_FRICTION .8
#define DEFAULT_GROUND_ELASTICITY .5

class chao
{
    private:
        int nsegs;         //numero de segmentos de reta que compoe o chao
        int variation;     //variacao maxima de altura entre dois segmentos consecutivos
        SDL_Point* segs;   //os pontos entre os quais serao desenhados os segmentos
        cpShape **ground;  //um shape para cada segmento
        cpShape **paredes; //shape para a parede (invisivel)
        int minH;          //altura minima que o chao pode atingir
        int maxH;          //altura maxima
        int width;         //largura do chao (tamanho da tela)
        int height;
        float friction;    //atrito do chao
        float elasticity;  //elasticidade do chao
        int gap;           //distancia (no eixo x) entre dois pontos

        cpSpace* spc;

    public:
        chao(cpSpace *space);
        chao(cpSpace *space, int nsegs, int variation, int minH, int maxH, float friction, float elasticity, int width, int height);
        void set_up(cpSpace *space);
        void show(SDL_Renderer* renderer);

        virtual ~chao();
        int Getnsegs() { return nsegs; }
        void Setnsegs(int val) { nsegs = val; }
        int Getvariation() { return variation; }
        void Setvariation(int val) { variation = val; }
        int GetminH() { return minH; }
        void SetminH(int val) { minH = val; }
        int GetmaxH() { return maxH; }
        void SetmaxH(int val) { maxH = val; }
        float Getfriction() { return friction; }
        void Setfriction(float val) { friction = val; }
        float Getelasticity() { return elasticity; }
        void Setelasticity(float val) { elasticity = val; }
};

#endif // CHAO_H
