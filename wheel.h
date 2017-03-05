#ifndef WHEEL_H
#define WHEEL_H

#include "enemy.h"

class wheel : public part {
    public:
        wheel(enemy* e, cpVect offset) : part(e) {
            offset.x = offset.x;
            offset.y = offset.y;

            mass = 70 * pow(e->scale, 2);
            moment = cpMomentForCircle(mass, ENEMY_WIDTH/2, ENEMY_HEIGHT/2, cpv(0,0));
            partBody = cpSpaceAddBody(e->spc, cpBodyNew(mass, moment)); //cria um corpo rigido no espaco passado e retorna um ponteiro para ele
            cpBodySetPos(partBody, cpv(DEFAULT_RES_W - START_POSX + offset.x, START_POSY + offset.y)); //seta posicao do corpo no espaco

            curSprite.x = 48; curSprite.y = 0; curSprite.w = ENEMY_WIDTH; curSprite.h = ENEMY_HEIGHT;
            partShape = cpSpaceAddShape(e->spc, cpCircleShapeNew(partBody, e->dimensions.w/2, cpv(0,0))); //cria uma shape sobre o corpo
            cpShapeSetCollisionType(partShape, ENEMY_TYPE); // type para gerenciar colisoes
            cpShapeSetFriction(partShape, .7); //define algumas propriedades do shape (atrito maximo, zero elasticidade)
            cpShapeSetElasticity(partShape, .7);

        }
        void update();
        void attach(enemy* e, int position);

        float radius;
        float potency;
};

#endif // WHEEL_H
