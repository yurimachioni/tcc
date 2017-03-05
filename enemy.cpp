#include "enemy.h"
#include <chipmunk.h>
#include <sdl.h>
#include <vector>
#include <math.h>
#include "utils.h"
#include <limits.h>

enemy::enemy(int pos, cpSpace *space, SDL_Texture* enemySprites, DNA* newDNA) : spc(space)
{
    this->pos = pos;
    dna = *newDNA;

    dna.e_maxLife *= dna.e_scale;
    if(dna.e_maxLife > MAX_ENEMY_LIFE) dna.e_maxLife = MAX_ENEMY_LIFE;

    nparts = 0;
    t = 0;
    vel.x = 0;
    vel.y = 0;
    life = dna.e_maxLife; //vida eh proporcional ao fator de escala
    score = 0.0;
    alive = true;
    inAir = true;
    sprites = enemySprites;
    dimensions.w = ENEMY_WIDTH*dna.e_scale; dimensions.h = ENEMY_HEIGHT*dna.e_scale;
    start_pos.x = DEFAULT_RES_W - START_POSX - pos*130;
    start_pos.y = START_POSY;

    center = new carcass(this); //cria a carcassa
    nparts++;

    //cria as demais partes de acordo com o que estiver definido no DNA passado e as anexa
    if(dna.top_part != NOTHING){
        topPart = partFactory::newPart(this, TOP, dna.top_part);
        topPart->attach(this, TOP);
        nparts++;
    } else topPart = NULL;
    if(dna.bottom_part != NOTHING){
        bottomPart = partFactory::newPart(this, BOTTOM, dna.bottom_part);
        bottomPart->attach(this, BOTTOM);
        nparts++;
    } else bottomPart = NULL;
    if(dna.left_part != NOTHING){
        leftPart = partFactory::newPart(this, LEFT, dna.left_part);
        leftPart->attach(this, LEFT);
        nparts++;
    } else leftPart = NULL;
    if(dna.right_part != NOTHING){
        rightPart = partFactory::newPart(this, RIGHT, dna.right_part);
        rightPart->attach(this, RIGHT);
        nparts++;
    } else rightPart = NULL;
}

void enemy::update(){
    if(alive){
        score+=.4; //ganha pontos por viver mais tempo

        //atualiza as partes do robo (animacao, tiros, variacao de potencia das helices)
        if(topPart != NULL)
            topPart->update();
        if(bottomPart != NULL)
            bottomPart->update();
        if(leftPart != NULL)
            leftPart->update();
        if(rightPart != NULL)
            rightPart->update();

        //varia o parametro usado para calcular a forca da helice
        if(t < 10e20)
            t += TIMESTEP/10;
    }
}

//desenha o robo
void enemy::show(SDL_Renderer* gRenderer){
    if(alive){
        //desenha as partes do enemy (se houver)
        center->show(gRenderer);
        if(topPart != NULL)
            topPart->show(gRenderer);
        if(bottomPart != NULL)
            bottomPart->show(gRenderer);
        if(leftPart != NULL)
            leftPart->show(gRenderer);
        if(rightPart != NULL)
            rightPart->show(gRenderer);
    }
}

//destrutor apenas limpa variaveis "estaticas" no objeto
enemy::~enemy()
{

}

/*desaloca variaveis dinamicas e remove e enemy do jogo
(separado do destrutor para poder mantero o inimigo no
 vector generation (para poder ser usado pelo AG) mesmo
 depois de morto*/
void enemy::die(){
    alive = false;
    for(unsigned int i = 0; i < constraints.size(); i++){
        cpSpaceRemoveConstraint(spc, constraints[i]);
        cpConstraintFree(constraints[i]);
    }

    delete center;
    delete topPart;
    delete bottomPart;
    delete leftPart;
    delete rightPart;
}

//inverte o torque das rodas caso o robo bata na parede
void enemy::flip(){
    if(dna.top_part == WHEEL)
        cpBodySetTorque(topPart->partBody, -cpBodyGetTorque(topPart->partBody));

    if(dna.bottom_part == WHEEL)
        cpBodySetTorque(bottomPart->partBody, -cpBodyGetTorque(bottomPart->partBody));

    if(dna.left_part == WHEEL)
        cpBodySetTorque(leftPart->partBody, -cpBodyGetTorque(leftPart->partBody));

    if(dna.right_part == WHEEL)
        cpBodySetTorque(rightPart->partBody, -cpBodyGetTorque(rightPart->partBody));
}

void enemy::takeDamage(float dmg){
    life -= dmg*KNIFE_DAMAGE;
    score += 300;
}

//dano dado eh proporcional ao fator de escala do robo
void enemy::dealDamage(float dmg){
    score += dmg*dna.e_scale; //ganha muitos pontos por dar dmg
}
