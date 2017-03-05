#ifndef PROPELLER_H
#define PROPELLER_H

#include "enemy.h"

using namespace parts;

class propeller : public part {
    public:
        propeller(enemy* e, cpVect offset) : part(e) {
            offset.x = offset.x;
            offset.y = offset.y;

            mass = 7 * pow(e->scale, 2); //massa menor
            moment = cpMomentForBox(mass, ENEMY_WIDTH, ENEMY_HEIGHT);
            partBody = cpSpaceAddBody(e->spc, cpBodyNew(mass, moment)); //cria um corpo rigido no espaco passado e retorna um ponteiro para ele
            cpBodySetPos(partBody, cpv(DEFAULT_RES_W - START_POSX + offset.x, START_POSY + offset.y)); //seta posicao do corpo no espaco

            curSprite.x = 96; curSprite.y = 0; curSprite.w = ENEMY_WIDTH; curSprite.h = ENEMY_HEIGHT;
            partShape = cpSpaceAddShape(e->spc, cpBoxShapeNew(partBody, e->dimensions.w, e->dimensions.h));
            cpShapeSetCollisionType(partShape, ENEMY_TYPE); // type para gerenciar colisoes
            cpShapeSetFriction(partShape, .4); //define algumas propriedades do shape (atrito maximo, zero elasticidade)
            cpShapeSetElasticity(partShape, .7);
        }
        void update();
        void attach(enemy* e, int position);

        float potency;
};

#endif // PROPELLER_H
