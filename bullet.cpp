#include "enemy.h"

bullet::bullet(cannon* p) : cann(p)
{
    e = cann->e;
    pos = cpBodyGetPos(cann->getBody());
    direction = cann->angulo_rad+PI/2; //direcao que o tiro vai sair

    mass = 1 * pow(e->dna.e_scale, 2); //massa menor
    moment = INFINITY;
    bulletBody = cpSpaceAddBody(e->spc, cpBodyNew(mass, moment)); //cria um corpo rigido no espaco passado e retorna um ponteiro para ele
    cpBodySetPos(bulletBody, cpv(pos.x, pos.y)); //seta posicao do corpo no espaco
    cpBodySetVelLimit(bulletBody, 300);

    curSprite.x = 240; curSprite.y = 0; curSprite.w = ENEMY_WIDTH; curSprite.h = ENEMY_HEIGHT;
    bulletShape = cpSpaceAddShape(e->spc, cpCircleShapeNew(bulletBody, 9, cpv(0,0)));
    cpShapeSetCollisionType(bulletShape, BULLET_TYPE); // type para gerenciar colisoes
    cpShapeSetFriction(bulletShape, .8); //define algumas propriedades do shape (atrito maximo, zero elasticidade)
    cpShapeSetElasticity(bulletShape, .2);
    cpShapeSetGroup(bulletShape, ENEMY_PARTS_GROUP);
    cpShapeSetUserData(bulletShape, this); //relaciona o bullet ao shape para tratar no callback da colisao
    cpBodySetUserData(bulletBody, e);      //relaciona o enemy ao body para tratar no callback da colisao

    // FIRE !
    float var = (((float)rand() / (float)RAND_MAX) - 0.5)* e->dna.c_potency_var;
    cpBodyApplyImpulse( bulletBody, cpv(cos(direction) * e->dna.c_potency + var,
                        (5+var) * sin(direction)) * e->dna.c_potency, cpv(0,0)); //15x mais forte no sentido y
}

bullet::~bullet()
{
    cpSpaceRemoveShape(e->spc, bulletShape);
    cpShapeFree(bulletShape);
    cpSpaceRemoveBody(e->spc, bulletBody);
    cpBodyFree(bulletBody);
}

void bullet::show(SDL_Renderer* gRenderer){
    cpVect pos = getDrawPos(); //pega a posicao da part no espaco
    draw_pos.x =  pos.x; //copia as coordenadas pra um SDL_Rect pra poder passar pra funcao de render
    draw_pos.y = -pos.y; //inverte pro SDL achar bacana (eixo y eh oposto ao do chipmunk)
    draw_pos.w = curSprite.w * cann->e->dna.e_scale;
    draw_pos.h = curSprite.h * cann->e->dna.e_scale;

    //gRenderer desenha a textura sprites (regiao definida por curSprite) no retangulo dest, girando de angulo em torno do centro (NULL)
    SDL_RenderCopyEx( gRenderer, cann->e->sprites, &curSprite, &draw_pos, 0.0, NULL, SDL_FLIP_HORIZONTAL );
}

cpVect bullet::getDrawPos(){
     cpVect pos = cpBodyGetPos(bulletBody);
     pos.x -= cann->e->dimensions.w/2; //converte as coords do corpo de chipmunk pra sdl (0,0) no centro para (0,0) no canto superior esquerdo
     pos.y += cann->e->dimensions.h/2;
     return pos;
}
