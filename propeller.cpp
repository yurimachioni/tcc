#include "enemy.h"

#include <chipmunk.h>
#include <sdl.h>

#define PROPELLER_Y_BIAS 2000 //cheat para os propellers serem mais estaveis (forca tendenciosa pra cima, nao realistico)

//analogo ao construtor das demais partes
propeller::propeller(enemy* e, cpVect offset) : part(e) {
    offset.x = offset.x;
    offset.y = offset.y;
    angulo_rel = 0;
    e->dna.p_base_potency *= e->dna.e_scale;

    mass = 10 * pow(e->dna.e_scale, 2); //massa menor
    moment = cpMomentForBox(mass, ENEMY_WIDTH, ENEMY_HEIGHT);
    partBody = cpSpaceAddBody(e->spc, cpBodyNew(mass, moment)); //cria um corpo rigido no espaco passado e retorna um ponteiro para ele
    cpBodySetPos(partBody, cpv(e->start_pos.x + offset.x, e->start_pos.y + offset.y)); //seta posicao do corpo no espaco
    cpBodySetVelLimit(partBody, 150);

    curSprite.x = 96; curSprite.y = 0; curSprite.w = ENEMY_WIDTH; curSprite.h = ENEMY_HEIGHT;
    spriteCounter = 0;
    partShape = cpSpaceAddShape(e->spc, cpBoxShapeNew(partBody, e->dimensions.w-2, e->dimensions.h-2));
    cpShapeSetCollisionType(partShape, PROPELLER_TYPE); // type para gerenciar colisoes
    cpShapeSetFriction(partShape, .4); //define algumas propriedades do shape (atrito maximo, zero elasticidade)
    cpShapeSetElasticity(partShape, 1);
    cpShapeSetGroup(partShape, ENEMY_PARTS_GROUP);
    cpShapeSetUserData(partShape, e);
}

propeller::~propeller()
{
    cpSpaceRemoveShape(e->spc, partShape);
    cpShapeFree(partShape);
    cpSpaceRemoveBody(e->spc, partBody);
    cpBodyFree(partBody);

}

void propeller::color(){
    SDL_SetTextureColorMod(e->sprites, 255,255,255);
}

void propeller::update(){
    spriteCounter++;     //animacao da helice
    if(spriteCounter > 10)
        curSprite.y = 48;
    if(spriteCounter > 20)
        curSprite.y = 96;
    if(spriteCounter > 30){
        spriteCounter = 0;
        curSprite.y = 0;
    }

    //potencia varia (pot base + (var * uma senoide q varia com o tempo))
    potency = e->dna.p_base_potency + fabs(1+sin(e->t)) * e->dna.p_var_potency;
    //seta a forca do propeller na direcao que ele aponta + um bias vertical pra cima (aumenta estabilidade)
    cpBodySetForce(partBody, cpv(cos(angulo_rad+PI/2) * potency, sin(angulo_rad+PI/2) * potency + PROPELLER_Y_BIAS));

}

void propeller::attach(enemy* e, int position){

    SDL_Rect dim = e->dimensions;

    /*ver primeiro a classe wheel pra entender os constraints*/

    /*como o propeller nao deve se mover em relacao a carcassa, os pinJoints sao fixado
      a dois pontos distintos (nos cantos) tanto na carcassa quando no propeller.*/

    cpConstraint *c1, *c2;

    if(position == RIGHT){
        c1 = cpPinJointNew(e->center->getBody(), partBody, cpv(+dim.w/2,+dim.h/2), cpv(-dim.w/2,+dim.h/2));
        c2 = cpPinJointNew(e->center->getBody(), partBody, cpv(+dim.w/2,-dim.h/2), cpv(-dim.w/2,-dim.h/2));
        angulo_rel = -PI/2;
    } else if (position == BOTTOM) {
        c1 = cpPinJointNew(e->center->getBody(), partBody, cpv(-dim.w/2,-dim.h/2), cpv(-dim.w/2,+dim.h/2));
        c2 = cpPinJointNew(e->center->getBody(), partBody, cpv(+dim.w/2,-dim.h/2), cpv(+dim.w/2,+dim.h/2));
        angulo_rel = PI;
    } else if (position == TOP) {
        c1 = cpPinJointNew(e->center->getBody(), partBody, cpv(-dim.w/2,+dim.h/2), cpv(-dim.w/2,-dim.h/2));
        c2 =cpPinJointNew(e->center->getBody(), partBody, cpv(+dim.w/2,+dim.h/2), cpv(+dim.w/2,-dim.h/2));
        angulo_rel = 0;
    } else if (position == LEFT) {
        c1 = cpPinJointNew(e->center->getBody(), partBody, cpv(-dim.w/2,+dim.h/2), cpv(+dim.w/2,+dim.h/2));
        c2 = cpPinJointNew(e->center->getBody(), partBody, cpv(-dim.w/2,-dim.h/2), cpv(+dim.w/2,-dim.h/2));
        angulo_rel = +PI/2;
    }
    cpSpaceAddConstraint(e->spc, c1);
    cpSpaceAddConstraint(e->spc, c2);
    e->constraints.push_back(c1);
    e->constraints.push_back(c2);
}
