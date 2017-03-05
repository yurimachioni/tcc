#include "enemy.h"

void part::show(SDL_Renderer* gRenderer){

    cpVect pos = getDrawPos(); //pega a posicao da part no espaco
    draw_pos.x =  pos.x; //copia as coordenadas pra um SDL_Rect pra poder passar pra funcao de render
    draw_pos.y = -pos.y; //inverte pro SDL achar bacana (eixo y eh oposto ao do chipmunk)
    draw_pos.w = curSprite.w * e->dna.e_scale;
    draw_pos.h = curSprite.h * e->dna.e_scale;
    angulo_rad = cpBodyGetAngle(partBody) + angulo_rel;
    angulo = CONVERT_TO_DEGREES(angulo_rad);

    //chama a funçao que pinta a parte da cor certa
    color();
    //gRenderer desenha a textura sprites (regiao definida por curSprite) no retangulo dest, girando de angulo em torno do centro (NULL)
    SDL_RenderCopyEx( gRenderer, e->sprites, &curSprite, &draw_pos, angulo, NULL, SDL_FLIP_HORIZONTAL );
}

cpVect part::getDrawPos(){
     cpVect pos = cpBodyGetPos(partBody);
     pos.x -= e->dimensions.w/2; //converte as coords do corpo de chipmunk pra sdl (0,0) no centro para (0,0) no canto superior esquerdo
     pos.y += e->dimensions.h/2;
     return pos;
}

//cria e retorna a parte devidamente posicionada em relacao a carcassa
part* partFactory::newPart(enemy* e, int position, int partno){
    if (position == TOP){
        if(partno == WHEEL)
            return new wheel(e, cpv(0,e->dimensions.h));
        else if (partno == PROPELLER)
            return new propeller(e, cpv(0,e->dimensions.h));
        else if (partno == CANNON)
            return new cannon(e, cpv(0,e->dimensions.h));
    }
    else if (position == BOTTOM){
        if(partno == WHEEL)
            return new wheel(e, cpv(0,-e->dimensions.h));
        else if (partno == PROPELLER)
            return new propeller(e, cpv(0,-e->dimensions.h));
        else if (partno == CANNON)
            return new cannon(e, cpv(0,-e->dimensions.h));
    }
    else if (position == LEFT){
        if(partno == WHEEL)
            return new wheel(e, cpv(-e->dimensions.w,0));
        else if (partno == PROPELLER)
            return new propeller(e, cpv(-e->dimensions.w,0));
        else if (partno == CANNON)
            return new cannon(e, cpv(-e->dimensions.w,0));
    }
    else if (position == RIGHT){
        if(partno == WHEEL)
            return new wheel(e, cpv(e->dimensions.w,0));
        else if (partno == PROPELLER)
            return new propeller(e, cpv(e->dimensions.w,0));
        else if (partno == CANNON)
            return new cannon(e, cpv(e->dimensions.w,0));
    }
    return NULL;
}

