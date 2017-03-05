#include <math.h>

#include "chao.h"

//inicializa um chao parametrizado (nao usado)
chao::chao(cpSpace *space, int nsegs, int variation, int minH, int maxH, float friction, float elasticity, int width, int height) : spc(space)
{
    this->nsegs = nsegs;
    ground = (cpShape**) malloc((nsegs-1)*sizeof(cpShape*));
    segs = (SDL_Point*) malloc (nsegs * sizeof(SDL_Point));
    paredes = (cpShape**) malloc(4*sizeof(cpShape*));
    this->variation = variation;
    this->minH = minH;
    this->maxH = maxH;
    this->width = width;
    this->friction = friction;
    this->elasticity = elasticity;
    gap = ceil(((float) width) / ((float) nsegs)); //distancia horizontal entre 2 segmentos do chao

    set_up(space);
}

//inicializa um chao
chao::chao(cpSpace *space) : spc(space)
{
    nsegs = 100;
    ground = (cpShape**) malloc((nsegs-1)*sizeof(cpShape*));
    segs = (SDL_Point*) malloc (nsegs * sizeof(SDL_Point));
    paredes = (cpShape**) malloc(4*sizeof(cpShape*));
    variation = DEFAULT_GROUND_VAR;
    minH = DEFAULT_RES_H - 200;
    maxH = DEFAULT_RES_H - 100;
    this->width = DEFAULT_RES_W;
    this->height = DEFAULT_RES_H;
    friction = DEFAULT_GROUND_FRICTION;
    elasticity = DEFAULT_GROUND_ELASTICITY;
    gap = ceil(((float) width) / ((float) nsegs-1)); //distancia horizontal entre 2 segmentos do chao

    //monta o chao aleatoriamente
    set_up(space);
}

chao::~chao()
{
    //dtor
    free(segs);
    for(int i = 0; i < nsegs-1; i++){
        cpSpaceRemoveShape(spc, ground[i]);
        cpShapeFree(ground[i]);
    }
    for(int i = 0; i < 4; i++){
        cpSpaceRemoveShape(spc, paredes[i]);
        cpShapeFree(paredes[i]);
    }
    free(paredes);
    free(ground);
}

void chao::set_up(cpSpace *space){

    //define ponto inicial do chao (primeiro segmento)
    segs[0].y = (rand() % abs(maxH - minH)) + minH; //posicao y do ponto inicial do primeiro segmento
    segs[0].x = gap*(0);

    //define demais pontos em funcao do anterior
    for(int i = 1; i < nsegs; i++){
        if(variation > 0)
            segs[i].y = ((rand() % (2*variation))-variation) + segs[i-1].y;  //posicao y dos demais pontos
        else segs[i].y = segs[i-1].y;
        segs[i].x = gap*i;
        if(segs[i].y < minH)
            segs[i].y += variation;   //checa se ta no intervalo certo e corrige se nao.
        if(segs[i].y > maxH)
            segs[i].y -= variation;

        //cria uma segment shape entre 2 pontos
        ground[i-1] = cpSegmentShapeNew(space->staticBody, cpv(segs[i-1].x, -segs[i-1].y), cpv(segs[i].x, -segs[i].y), 2);

        cpShapeSetCollisionType(ground[i-1], GROUND_TYPE); //type para gerenciar colisoes
        cpShapeSetFriction(ground[i-1], friction);
        cpShapeSetElasticity(ground[i-1], elasticity); // a elasticidade da colisao depende dos dois corpos. se apenas um for elastico, nao quica.

    }

    for(int i = 0; i < nsegs-1; i++)
        cpSpaceAddShape(space, ground[i]);

    paredes[0] =  cpSegmentShapeNew(space->staticBody, cpv(0, 0), cpv(0, -height), 1); //cria parede esquerda
    paredes[1] =  cpSegmentShapeNew(space->staticBody, cpv(0, -height), cpv(width, -height), 1); //parede em baixo do chao
    paredes[2] =  cpSegmentShapeNew(space->staticBody, cpv(width, -height), cpv(width, 0), 1); //parede direita
    paredes[3] =  cpSegmentShapeNew(space->staticBody, cpv(width, 0), cpv(0,0), 1); //teto

    //define propriedades da parede
    for(int i = 0; i < 4; i++){
        cpShapeSetFriction( paredes[i], 0);
        cpShapeSetElasticity(paredes[i], 1);
        cpSpaceAddShape(space, paredes[i]);
    }

    //seta collision type para detectar quando um bixo bate na parede (para inverter o torque das rodas)
    cpShapeSetCollisionType(paredes[0], WALL_TYPE);
    cpShapeSetCollisionType(paredes[2], WALL_TYPE);
}

//desenha o chao
void chao::show(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x00 );
    SDL_RenderDrawLines(renderer, segs, nsegs);
}
