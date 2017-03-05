#include "player.h"

player::player(cpSpace *space, SDL_Texture* playerSprites) : spc(space)
{
    dir = 1;
    vel.x = 0;
    vel.y = 0;
    accY = 0;
    angulo = 0;
    life = 500.0;
    score = 0.0;
    alive = true;
    inAir = true;
    breakableJump = false;
    stabbable = true;
    sprites = playerSprites;
    spriteCounter = 0;
    flip = 1;
    curSprite.x = 0; curSprite.y = 0; curSprite.w = PLAYER_WIDTH; curSprite.h = PLAYER_HEIGHT;
    dimensions.w = PLAYER_WIDTH; dimensions.h = PLAYER_HEIGHT;
    collideCount = 0;
    kn = NULL;
    lifeBar.x = 20; lifeBar.y = 20;
    genClears = 0;
    genRect.x = 550; genRect.y = 20; genRect.w = 5; genRect.h = 10;

    mass = 70;

    moment = INFINITY;

    playerBody = cpSpaceAddBody(space, cpBodyNew(mass, moment)); //cria um corpo rigido no espaco passado e retorna um ponteiro para ele
    playerShape = cpSpaceAddShape(space, cpBoxShapeNew(playerBody, dimensions.w-3, dimensions.h-3)); //cria uma shape sobre o corpo
    cpShapeSetCollisionType(playerShape, PLAYER_TYPE);        // type para gerenciar colisoes
    cpShapeSetFriction(playerShape, DEFAULT_PLAYER_FRICTION); //define algumas propriedades do shape (atrito maximo, zero elasticidade)
    cpShapeSetElasticity(playerShape, DEFAULT_PLAYER_ELASTICITY);

    cpBodySetPos(playerBody, cpv(START_POSX, START_POSY)); //seta posicao do corpo no espaco
    cpSpaceReindexShapesForBody(space, playerBody);
}

player::~player()
{
    cpSpaceRemoveShape(spc, playerShape);
    cpShapeFree(playerShape);
    cpSpaceRemoveBody(spc, playerBody);
    cpBodyFree(playerBody);
}

void player::update(){ //player perde vida o tempo todo
    life -= .02;
    if(life <= 0)
        alive = false;
}

void player::show(SDL_Renderer* gRenderer){
    if(kn != NULL) kn->show(gRenderer);

    cpVect pos = getDrawPos(playerBody); //pega a posicao do player no espaco
    draw_pos.x = pos.x; //copia as coordenadas pra um SDL_Rect pra poder passar pra funcao de render
    draw_pos.y = -pos.y;
    draw_pos.w = curSprite.w;
    draw_pos.h = curSprite.h;

    animate();
    angulo = CONVERT_TO_DEGREES(cpBodyGetAngle(playerBody));

    //gRenderer desenha a textura sprites (regiao definida por curSprite) girando de angulo em torno do centro (NULL)
    if(flip)
        SDL_RenderCopyEx( gRenderer, sprites, &curSprite, &draw_pos, angulo, NULL, SDL_FLIP_HORIZONTAL);
    else
        SDL_RenderCopyEx( gRenderer, sprites, &curSprite, &draw_pos, angulo, NULL, SDL_FLIP_NONE);

    showHUD(gRenderer);
}
//retorna a posicao correta para desenhar o player (a coordenada do corpo na engine fisica eh o centro
//de gravidade dele, e na biblioteca de desenho eh o canto superior esquerdo).
cpVect player::getDrawPos(cpBody* b){
     cpVect pos = cpBodyGetPos(b);
     pos.x -= dimensions.w/2;
     pos.y += dimensions.h/2;
     return pos;
}

//atualiza qual sprite sera desenhado
void player::animate(){
    spriteCounter++;
    cpVect vel = cpBodyGetVel(playerBody);

    if(vel.x > 5 || vel.x < 5){
        if(spriteCounter>10)
            curSprite.y = 64;
        if(spriteCounter>20)
            curSprite.y = 128;
        if(spriteCounter>30)
            curSprite.y = 192;
        if(spriteCounter>40){
            curSprite.y = 64;
            spriteCounter = 10;
        }
    }
    if(vel.x > 5)
        flip = 0;
    else if (vel.x < -5)
        flip = 1;
    else curSprite.y = 0;
}

void player::showHUD(SDL_Renderer* gRenderer){
    //desenha lifebar
    lifeBar.w = life; lifeBar.h=10;
    if(life > 200)
        SDL_SetRenderDrawColor(gRenderer, 0, 220, 0, 255);
    else if(life > 100)
        SDL_SetRenderDrawColor(gRenderer, 220, 220, 0, 255);
    else
        SDL_SetRenderDrawColor(gRenderer, 220, 0, 0, 255);
    SDL_RenderFillRect(gRenderer, &lifeBar);

    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    genRect.x = 550;
    for(int i = 0; i<genClears; i++){
        genRect.x += 8;
        SDL_RenderFillRect(gRenderer, &genRect);
    }
}

void player::jump(){
    //zera atrito para o movimento ser mais fluido
    cpShapeSetFriction(playerShape, DEFAULT_PLAYER_FRICTION);

    //so permite pulo se nao estiver no ar
    if(!inAir){
        breakableJump = true;

        vel = cpBodyGetVel(playerBody);
        cpBodySetVel(playerBody, cpv(vel.x,0));

        //corpo, impulso e offset do centro de gravidade (x,y)
        cpBodyApplyImpulse(playerBody, cpv(0,13000), cpv(0,0));
    }
}

void player::breakJump(){ //cancela o pulo se soltar a tecla
    vel = cpBodyGetVel(playerBody);
    if(vel.y > 0){
        cpBodySetVel(playerBody, cpv(vel.x,vel.y/2));
        breakableJump = false;
    }
}

void player::collidePlayerGround(){ //atualiza variaveis para saber se o player esta tocando o chao
    collideCount++;
    inAir = false;
    breakableJump = false;
}

void player::separatePlayerGround(){
    collideCount--;
    if(collideCount == 0)
        inAir = true;

}

void player::acc_left(){ //move para a esquerda
    vel = cpBodyGetVel(playerBody);

    cpShapeSetFriction(playerShape, DEFAULT_PLAYER_FRICTION);
    if(inAir && vel.x >-100){ //aceleracao menor se estiver no ar
        cpBodySetVel(playerBody, cpv(vel.x-1,vel.y));
    }
    else if(vel.x > -100){
        cpBodySetVel(playerBody, cpv(vel.x-5,vel.y));
    }
    dir = -1;
}

void player::acc_right(){//move para a direita
    vel = cpBodyGetVel(playerBody);

    cpShapeSetFriction(playerShape, DEFAULT_PLAYER_FRICTION);
    if(inAir && vel.x <100){
        cpBodySetVel(playerBody, cpv(vel.x+1,vel.y));
    }
    else if(vel.x < 100){
        cpBodySetVel(playerBody, cpv(vel.x+5,vel.y));
    }
    dir = 1;
}

void player::stabUp(){
    dir = 0;
}

void player::stop(){ //aumenta o atrito para desacelerar o player caso nao esteja sendo movido
    cpShapeSetFriction(playerShape, .85);
}

void player::takeDamage(float dmg){ //diminui a vida e da um leve impulso para cima ao receber dano
    life -= dmg;
    cpBodyApplyImpulse( playerBody, cpv(0, 7000), cpv(0,0));
}

void player::gainBonusLife(float bonus){
    life += bonus;
    if(life > 500)
        life = 500;
}

void player::stab(char c){ //cria uma faca e a atira
    if(c == 'p' && stabbable){ //press
        if(kn != NULL)
        delete kn;
        kn = new knife(this);
        kn->stab();
        stabbable = false;
    }
    else if (c == 'r') // release
        stabbable = true;

}

void player::destroyKnife(){
    delete kn;
    kn = NULL;
}

//----------------------------------------------------------KNIFE-------------------------------------------------------------------------------------

knife::knife(player* p) : p(p){
    spc = p->spc;
    curSprite.x = 32; curSprite.y = 0; curSprite.w = 32; curSprite.h = 32;
    dimensions.w = 32; dimensions.h = 32;
    angulo = 0;

    mass = 1;
    moment = cpMomentForBox(mass, dimensions.w, dimensions.h);

    knifeBody = cpSpaceAddBody(spc, cpBodyNew(mass, moment)); //cria um corpo rigido no espaco passado e retorna um ponteiro para ele
    knifeShape = cpSpaceAddShape(spc, cpBoxShapeNew(knifeBody, dimensions.w, dimensions.h)); //cria uma shape sobre o corpo
    cpShapeSetCollisionType(knifeShape, KNIFE_TYPE); // type para gerenciar colisoes
    cpShapeSetFriction(knifeShape, .5); //define algumas propriedades do shape (atrito maximo, zero elasticidade)
    cpShapeSetElasticity(knifeShape, .5);

    if(p->dir == 0)
        cpBodySetPos(knifeBody, cpBodyGetPos(p->playerBody) + cpv(0, p->dimensions.h));
    else
        cpBodySetPos(knifeBody, cpBodyGetPos(p->playerBody) + cpv(p->dir * p->dimensions.w, 0)); //seta posicao em posicao do player + largura dele
    cpSpaceReindexShapesForBody(spc, knifeBody);
}

knife::~knife(){
    cpSpaceRemoveConstraint(spc, c1);
    cpConstraintFree(c1);
    cpSpaceRemoveShape(spc, knifeShape);
    cpShapeFree(knifeShape);
    cpSpaceRemoveBody(spc, knifeBody);
    cpBodyFree(knifeBody);
}

void knife::stab(){
    //constraint de mola entre player e faca
    //argumentos:              body1          body2      anchr1    anchr2    restLen  stiffness damping
    if(p->dir == 1){ //facada pra direita
        c1 = cpDampedSpringNew(p->playerBody, knifeBody, cpv(0,0), cpv(0,0), 0,       20,       1);
        cpSpaceAddConstraint(spc, c1);
        cpBodyApplyImpulse(knifeBody, cpv(800, 0), cpv(0,0));
    }
    else if(p->dir == 0){ //facada pra cima (damped string mais fraca para ir mais alto)
        c1 = cpDampedSpringNew(p->playerBody, knifeBody, cpv(0,0), cpv(0,0), 0,       5,       1);
        cpSpaceAddConstraint(spc, c1);
        cpBodyApplyImpulse(knifeBody, cpv(0, 1400), cpv(0,0));
    }
    else { //facada pra esquerda
        c1 = cpDampedSpringNew(p->playerBody, knifeBody, cpv(0,0), cpv(0,0), 0,       20,       1);
        cpSpaceAddConstraint(spc, c1);
        cpBodyApplyImpulse(knifeBody, cpv(-800, 0), cpv(0,0));
    }
}

//desenha faca
void knife::show(SDL_Renderer* gRenderer){
    cpVect pos = p->getDrawPos(knifeBody); //pega a posicao do player no espaco
    draw_pos.x = pos.x; //copia as coordenadas pra um SDL_Rect pra poder passar pra funcao de render
    draw_pos.y = -pos.y;
    draw_pos.w = curSprite.w;
    draw_pos.h = curSprite.h;

    if(p->dir == -1) angulo = cpBodyGetAngle(knifeBody) + PI;
    else if(p->dir == 0) angulo = cpBodyGetAngle(knifeBody) + PI/2;
    else angulo = cpBodyGetAngle(knifeBody) + 0;
    angulo = CONVERT_TO_DEGREES(angulo);

    //gRenderer desenha a textura sprites (regiao definida por curSprite) no retangulo dest, girando de angulo em torno do centro (NULL)
    SDL_RenderCopyEx( gRenderer, p->sprites, &curSprite, &draw_pos, angulo, NULL, SDL_FLIP_NONE );
}


