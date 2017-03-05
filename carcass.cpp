#include "enemy.h"

#include <chipmunk.h>
#include <sdl.h>

carcass::carcass(enemy* e) : part(e) {

    angulo_rel = 0;

    mass = 50 * pow(e->dna.e_scale, 2);

    //moment = INFINITY;
    moment = cpMomentForBox(mass, ENEMY_WIDTH, ENEMY_HEIGHT); //determina o momento de inercia dada a massa e as dimensoes
    partBody = cpSpaceAddBody(e->spc, cpBodyNew(mass, moment)); //cria um corpo rigido no espaco passado e retorna um ponteiro para ele
    cpBodySetPos(partBody, cpv(e->start_pos.x , e->start_pos.y )); //seta posicao do corpo no espaco
    cpBodySetVelLimit(partBody, 120);
    cpBodySetAngVelLimit(partBody, 2);

    curSprite.x = 0; curSprite.y = 0; curSprite.w = ENEMY_WIDTH; curSprite.h = ENEMY_HEIGHT;
    partShape = cpSpaceAddShape(e->spc, cpBoxShapeNew(partBody, e->dimensions.w-2, e->dimensions.h-2));  //cria uma shape sobre o corpo
    cpShapeSetCollisionType(partShape, CARCASS_TYPE); // type para gerenciar colisoes
    cpShapeSetFriction(partShape, .7); //define algumas propriedades do shape (atrito maximo, zero elasticidade)
    cpShapeSetElasticity(partShape, 0);
    cpShapeSetGroup(partShape, ENEMY_PARTS_GROUP);
    cpShapeSetUserData(partShape, e);

};

void carcass::color(){
    SDL_SetTextureColorMod(e->sprites, 255*e->dna.e_maxLife / MAX_ENEMY_LIFE, 0, 0);
}

carcass::~carcass()
{
    cpSpaceRemoveShape(e->spc, partShape);
    cpShapeFree(partShape);
    cpSpaceRemoveBody(e->spc, partBody);
    cpBodyFree(partBody);
}

