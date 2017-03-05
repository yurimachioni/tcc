#ifndef UTILS_H
#define UTILS_H

#include <chipmunk.h>
#include <sdl.h>
#include <sdl_image.h>
#include <string>
#include <stdio.h>

#define CONVERT_TO_DEGREES(x) -57.295779513*x
#define PI 3.1415926535897932
#define DEFAULT_RES_W 1400
#define DEFAULT_RES_H 768

#define TARGET_FPS 60
#define TARGET_FRAME_TIME 16 //1000/60 ms

#define GRAVITY cpv(0, -100)
#define TIMESTEP 0.01666666666 // 1/60
#define START_POSX 300
#define START_POSY -300

#define PLAYER_TYPE 1
#define GROUND_TYPE 2
#define CARCASS_TYPE 3
#define PROPELLER_TYPE 4
#define WHEEL_TYPE 5
#define TOOTH_WHEEL_TYPE 6
#define WALL_TYPE 7
#define KNIFE_TYPE 8
#define CANNON_TYPE 9
#define BULLET_TYPE 10

enum Keys
{
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_2,
    KEY_3,
    KEY_ESC
};

//partes de inimigos
enum Parts
{
    NOTHING,
    WHEEL,
    PROPELLER,
    CANNON
};

//posicoes onde as partes dos inimigos podem ser colocadas
enum Positions
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
};

//collision callbacks ------------------------------------------------------------------------------------
void setUpCollisionHandlers(void* p1, cpSpace* space);

    //colisoes do player
int collidePlayerGroundWrapper(cpArbiter* arb, cpSpace* space, void* p1);
int collidePlayerKnifeWrapper(cpArbiter* arb, cpSpace* space, void* p1);
void postStepRemoveKnife(cpSpace *space, cpShape *shape, void* p1);
void separatePlayerGroundWrapper(cpArbiter* arb, cpSpace* space, void* p1);
int collidePlayerDamageWrapper(cpArbiter* arb, cpSpace* space, void* p1);
int collidePlayerBulletWrapper(cpArbiter* arb, cpSpace* space, void* p1);
int collidePlayerToothWheelWrapper(cpArbiter* arb, cpSpace* space, void* p1);
int collidePlayerPropellerWrapper(cpArbiter* arb, cpSpace* space, void* p1);

    //colisoes do inimigo
int collideEnemyWallWrapper(cpArbiter* arb, cpSpace* space, void* e1);
int collideKnifeCarcassWrapper(cpArbiter* arb, cpSpace* space, void* notused);
int collideKnifeWheelWrapper(cpArbiter* arb, cpSpace* space, void* notused);
int collideKnifePropellerWrapper(cpArbiter* arb, cpSpace* space, void* notused);
int collideKnifeCannonWrapper(cpArbiter* arb, cpSpace* space, void* notused);

//fim collision callbacks ------------------------------------------------------------------------------------

//funcoes de suporte para carregar imagens e inicializar biblioteca grafica
SDL_Texture* loadTexture(std::string&, SDL_Renderer*);
void close(SDL_Window*, SDL_Renderer*);
void init(SDL_Window**, SDL_Renderer**);

#endif // UTILS_H
