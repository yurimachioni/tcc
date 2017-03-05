#include "enemy.h"

#include <chipmunk.h>
#include <sdl.h>

//analogo ao construtor das demais partes
cannon::cannon(enemy* e, cpVect offset) : part(e) {
    offset.x = offset.x;
    offset.y = offset.y;
    angulo_rel = 0;
    e->dna.c_potency *= e->dna.e_scale;
    rateOfFire = e->dna.c_rate_of_fire;
    potency = e->dna.c_potency;
    potencyVar = e->dna.c_potency_var;
    bulletCounter = 0;
    bulletDelay = 60*4/rateOfFire; //fps * physIt / rateOfFire (delay entre tiros)

    mass = 50 * pow(e->dna.e_scale, 2); //massa menor
    moment = cpMomentForBox(mass, ENEMY_WIDTH, ENEMY_HEIGHT);
    partBody = cpSpaceAddBody(e->spc, cpBodyNew(mass, moment)); //cria um corpo rigido no espaco passado e retorna um ponteiro para ele
    cpBodySetPos(partBody, cpv(e->start_pos.x + offset.x, e->start_pos.y + offset.y)); //seta posicao do corpo no espaco
    cpBodySetVelLimit(partBody, 150);

    curSprite.x = 192; curSprite.y = 0; curSprite.w = ENEMY_WIDTH; curSprite.h = ENEMY_HEIGHT;
    partShape = cpSpaceAddShape(e->spc, cpBoxShapeNew(partBody, e->dimensions.w-20, e->dimensions.h));
    cpShapeSetCollisionType(partShape, CANNON_TYPE); // type para gerenciar colisoes
    cpShapeSetFriction(partShape, .8); //define algumas propriedades do shape
    cpShapeSetElasticity(partShape, 0);
    cpShapeSetGroup(partShape, ENEMY_PARTS_GROUP);
    cpShapeSetUserData(partShape, e);
}

cannon::~cannon()
{
    cpSpaceRemoveShape(e->spc, partShape);
    cpShapeFree(partShape);
    cpSpaceRemoveBody(e->spc, partBody);
    cpBodyFree(partBody);
    for(unsigned int i = 0; i < bullets.size(); i++)
        delete bullets[i];
}

void cannon::color(){
    SDL_SetTextureColorMod(e->sprites, 255,255,255);
}

void cannon::update(){

   bullet* temp;
   for(unsigned int i = 0; i < bullets.size(); i++) //percorre o vector de bullets deletando as que estao paradas
        if(cpvlengthsq(cpBodyGetVel(bullets[i]->bulletBody)) < 10){
            temp = bullets[i];
            bullets.erase(bullets.begin()+i);
            delete temp;
        }
   bulletCounter++; //conta quantos updates desde o ultimo tiro
   if(bulletCounter == bulletDelay){
        bullets.push_back(new bullet(this)); //cria uma bullet e atira ela
        float direction = angulo_rad; //angulo contrario ao que o canhao aponta
        cpBodyApplyImpulse( partBody, cpv(sin(direction) * 100,
                            cos(direction)) * 100, cpv(0,0)); //aplica coice no canhao
        bulletCounter = 0;
   }

}

void cannon::show(SDL_Renderer* gRenderer){

    part::show(gRenderer);
    for(unsigned int i = 0; i < bullets.size(); i++)
        bullets[i]->show(gRenderer);

}

//analogo ao attach do propeller
void cannon::attach(enemy* e, int position){

    SDL_Rect dim = e->dimensions;

    /*ver primeiro a classe wheel pra entender os constraints*/

    /*como o propeller nao deve se mover em relacao a carcassa, os pinJoints sao fixado a dois pontos distintos tanto na carcassa quando no propeller.
      a colisao entre os corpos impede que eles girem em torno um do outro*/

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
