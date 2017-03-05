#include "enemy.h"

#include <chipmunk.h>
#include <sdl.h>
#include "utils.h"

wheel::wheel(enemy* e, cpVect offset) : part(e) {
    offset.x = offset.x;
    offset.y = offset.y;
    angulo_rel = 0;
    e->dna.w_torque *= e->dna.e_scale;

    if(e->dna.w_tooth){ //se a roda for dentada
        curSprite.x = 144;  //sprite da roda dentada
        mass = 90 * pow(e->dna.e_scale, 2);
    } else {
        curSprite.x = 48;  //sprite da roda normal
        mass = 60 * pow(e->dna.e_scale, 2);
    }
    curSprite.y = 0;
    curSprite.w = ENEMY_WIDTH;
    curSprite.h = ENEMY_HEIGHT;

    moment = cpMomentForCircle(mass, ENEMY_WIDTH/2, 0, cpv(0,0)); //momento de inercia para um circulo desse raio
    partBody = cpSpaceAddBody(e->spc, cpBodyNew(mass, moment)); //cria um corpo rigido no espaco passado e retorna um ponteiro para ele
    cpBodySetPos(partBody, cpv(e->start_pos.x + offset.x, e->start_pos.y + offset.y)); //seta posicao do corpo no espaco
    cpBodySetAngVelLimit(partBody, 3);
    cpBodySetTorque(partBody, e->dna.w_torque);

    partShape = cpSpaceAddShape(e->spc, cpCircleShapeNew(partBody, e->dimensions.w/2-2, cpv(0,0))); //cria uma shape sobre o corpo

    if(e->dna.w_tooth) { //se a roda for dentada
        cpShapeSetCollisionType(partShape, TOOTH_WHEEL_TYPE); // type para gerenciar colisoes
        cpShapeSetElasticity(partShape, 0);
    }
    else {
        cpShapeSetCollisionType(partShape, WHEEL_TYPE); // type para gerenciar colisoes
        cpShapeSetElasticity(partShape, e->dna.w_elast);
    }
    cpShapeSetFriction(partShape, e->dna.w_frict);

    //define um grupo para evitar que as partes dos inimigos colidam entre si.
    cpShapeSetGroup(partShape, ENEMY_PARTS_GROUP);

    //mantem uma referencia ao enemy na estrutura da shape para que seja possivel descobrir QUAL
    //o enemy que colidiu (e fazer o tratamento de acordo).
    cpShapeSetUserData(partShape, e);

};

wheel::~wheel()
{
    cpSpaceRemoveShape(e->spc, partShape);
    cpShapeFree(partShape);
    cpSpaceRemoveBody(e->spc, partBody);
    cpBodyFree(partBody);

}

void wheel::color(){
    SDL_SetTextureColorMod(e->sprites, 255,255,255);
}

void wheel::update(){

}

//cria constraints que prendem a roda a carcassa
void wheel::attach(enemy* e, int position){

    /*
    carcassa:

    (-w/2, +h/2)
            -----------------------X (+w/2, +h/2)
            |                      | ...
            |                      |    ...
            |                      |       ...
            |                      |          ...
            |           .          |             ...
            |         (0,0)        |                 X (centro da roda (0,0))
            |                      |            ....
            |                      |        ....
            |                      |    ....
            |                      | ...
            -----------------------X (+w/2, -h/2)
    (-w/2, -h/2)

    legenda: X: pontos terminais dos pinJoints
           ...: distancia fixa que o pinJoint forca.

    (obs.: na verdade os pontos na carcassa foram trocados para o outro lado para as rodas suportarem mais peso

    vale a mesma ideia pras outras partes.
    */

    SDL_Rect dim = e->dimensions;

    /*o eixo de cada roda eh fixado a 2 pontos na carcassa para evitar que a roda gire em torno dela.*/

    cpConstraint *c1, *c2;

    //cria os constraints no lugar certo
    if(position == RIGHT){
        c1 = cpPinJointNew(e->center->getBody(), partBody, cpv(-dim.w/2,-dim.h/2), cpv(0,0));
        c2 = cpPinJointNew(e->center->getBody(), partBody, cpv(-dim.w/2,+dim.h/2), cpv(0,0));
    } else if (position == BOTTOM) {
        c1 = cpPinJointNew(e->center->getBody(), partBody, cpv(+dim.w/2,+dim.h/2), cpv(0,0));
        c2 = cpPinJointNew(e->center->getBody(), partBody, cpv(-dim.w/2,+dim.h/2), cpv(0,0));
    } else if (position == TOP) {
        c1 = cpPinJointNew(e->center->getBody(), partBody, cpv(+dim.w/2,-dim.h/2), cpv(0,0));
        c2 = cpPinJointNew(e->center->getBody(), partBody, cpv(-dim.w/2,-dim.h/2), cpv(0,0));
    } else if (position == LEFT) {
        c1 = cpPinJointNew(e->center->getBody(), partBody, cpv(+dim.w/2,-dim.h/2), cpv(0,0));
        c2 = cpPinJointNew(e->center->getBody(), partBody, cpv(+dim.w/2,+dim.h/2), cpv(0,0));
    }
    //adiciona-os ao espaco
    cpSpaceAddConstraint(e->spc, c1);
    cpSpaceAddConstraint(e->spc, c2);

    //mantem ponteiros para eles num vector para poder remover depois
    e->constraints.push_back(c1);
    e->constraints.push_back(c2);
}
