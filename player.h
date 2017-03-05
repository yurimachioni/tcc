#ifndef PLAYER_H
#define PLAYER_H

#include <chipmunk.h>
#include <sdl.h>
#include <sdl_image.h>
#include "utils.h"

#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 64
#define DEFAULT_PLAYER_FRICTION .3
#define DEFAULT_PLAYER_ELASTICITY 0

class knife;

class player
{
   friend class knife;
   private:
        cpVect pos; SDL_Rect draw_pos; //posicao de desenho eh diferente da posicao do corpo no espaco fisico
        cpVect vel;
        float accY;
        float angulo;
        int dir;        //direcao q o cara ta virado, -1 eh esquerda, 1 eh direita
        float life;
        float score;
        bool alive;
        bool inAir;           //flag para permitir pulo
        bool breakableJump;   //flag para interromper pulo caso a tecla seja solta
        bool stabbable;       //flag para impedir spam de facadas
        SDL_Texture* sprites; //ponteiro para os sprites do player
        SDL_Rect curSprite;   //retangulo em torno do sprite atualmente sendo desenhado
        SDL_Rect dimensions;
        int spriteCounter;    //contador de frames (para animacao)
        int flip;
        int genClears;

        SDL_Rect lifeBar;
        SDL_Rect genRect;

        cpSpace *spc;         //ponteiro para o espaco onde o player esta
        cpFloat mass;
        cpFloat moment;
        cpBody *playerBody;   //estrutura que representa as propriedades do player na engine fisica
        cpShape *playerShape; //estrutura que representa a forma do player na engine fisica
        int collideCount;     //para controle do pulo (guarda um valor diferente de 0 se o player estiver tocando o chao)

        knife* kn;            //faca

        void animate();
        void showHUD(SDL_Renderer* gRenderer);
        cpVect getDrawPos(cpBody *b);

    public:
        player();
        virtual ~player();
        player(cpSpace*, SDL_Texture*);
        void update();
        void show(SDL_Renderer*);

    //keypresses --------------
        void jump();         //pula
        void breakJump();    //interrompe pulo
        void acc_left();     //move para a esquerda
        void acc_right();    //move para a direita
        void stabUp();       //define direcao da facada: para cima
    //fim keypresses ----------

        void stop();
        void stab(char);     //atira faca ou destroi a faca
        void destroyKnife();

        void collidePlayerGround();   //callbackas de colisao (define se pode ou nao pular)
        void separatePlayerGround();
        void takeDamage(float dmg);
        void gainBonusLife(float bonus);

        bool getBreakableJump(){
            return breakableJump;
        }
        cpVect getVel(){
            return cpBodyGetVel(playerBody);;
        }
        bool getAlive(){
            return alive;
        }
        void incGenClears(){
            genClears++;
        }

};

class knife{
    friend class player;
    private:
        player* p;      //ponteiro para o jogador dono da faca
        cpSpace* spc;
        cpVect pos; SDL_Rect draw_pos;
        SDL_Rect curSprite;
        SDL_Rect dimensions;
        cpFloat angulo;

        cpFloat mass;
        cpFloat moment;
        cpBody *knifeBody;
        cpShape *knifeShape;
        cpConstraint *c1;    //mola que mantem a faca presa ao jogador

    public:
        knife(player* p);
        ~knife();
        void stab();
        void show(SDL_Renderer* gRenderer);
};

#endif // PLAYER_H
