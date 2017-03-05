#ifndef CARCASS_H
#define CARCASS_H

#include "enemy.h"

class carcass : public part {
    public:
        carcass(enemy* e);
        ~carcass();

        void attach(enemy* e, int position){};

        void update(){};
};

#endif // CARCASS_H
