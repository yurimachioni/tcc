#ifndef ENEMY_H
#define ENEMY_H

#include <vector>
#include <chipmunk.h>
#include <sdl.h>
#include <sdl_image.h>
#include "utils.h"
#include "player.h"
#include "math.h"
#include "DNA.h"

#define ENEMY_WIDTH 48
#define ENEMY_HEIGHT 48
#define ENEMY_PARTS_GROUP 1

#define KNIFE_DAMAGE 10
#define MAX_ENEMY_LIFE 500
#define MAX_WHEEL_TORQUE 100000

using namespace std;

class part;
class bullet;

class enemy
{
    //parts do inimigo podem acessar variaveis private do enemy livremente
    friend class part;
    friend class partFactory;
    friend class carcass;
    friend class wheel;
    friend class propeller;
    friend class cannon;
    friend class bullet;

    private:

        int pos;        //fator que define a posicao inicial de cada robo
        struct DNA dna; //DNA de um robo

        cpVect start_pos;
        cpVect vel;
        float life;
        float score;    //(fitness)usado pelo AG para avaliar o desempenho do robo
        bool alive;
        bool inAir;

        cpSpace* spc; //espaco onde o inimigo esta contido

        SDL_Texture* sprites;
        SDL_Rect dimensions;
        float t; // contador de iteracoes

        //vector com as constraints que prendem as partes de um robo a carcassa.
        std::vector<cpConstraint*> constraints;
        int nparts;

        //ponteiro para cada parte do robo
        part* center;
        part* topPart;
        part* bottomPart;
        part* leftPart;
        part* rightPart;

    public:

        enemy(int pos, cpSpace *space, SDL_Texture* enemySprites, DNA* dna); //construtor de inimigo baseado no DNA passado
        void show(SDL_Renderer* gRenderer);
        void update();
        virtual ~enemy();
        void die();
        cpVect getPos();
        void takeDamage(float dmg);
        void dealDamage(float dmg);

        void setDNA(DNA* dna){
            this->dna = *dna;
        }
        DNA* getDNA(){
            return &dna;
        }
        void flip();

        cpVect Getvel() { return vel; }
        void Setvel(cpVect val) { vel = val; }
        float Getlife() { return life; }
        void Setlife(float val) { life = val; }
        float Getscore() { return score; }
        void Setscore(float val) { score = val; }
        bool Getalive() { return alive; }
        void Setalive(bool val) { alive = val; }

};

//---------------------------------------------------------------PART---------------------------------------------------------------------------

class part{
    friend class enemy;
    friend class bullet;

    protected:
        enemy* e;
        cpVect offset; //posicao que a parte esta em relacao a carcassa do enemigo a que pertence
        float mass;
        SDL_Rect curSprite;
        cpShape *partShape;
        cpFloat moment;
        cpBody* partBody;
        SDL_Rect draw_pos;
        float angulo;  //angulo para desenho
        float angulo_rad; //angulo relativo a posicao em que esta anexa a carcassa
        float angulo_rel;

        virtual void attach(enemy* e, int position) = 0;

    public:

        part(enemy* enem) : e(enem) { };
        virtual ~part(){};
        virtual void show(SDL_Renderer* gRenderer);
        virtual void update() = 0;
        virtual void color() = 0;
        cpBody* getBody(){
            return partBody;
        }

    private:
        cpVect getDrawPos();
};

//---------------------------------------------------------------PARTFACTORY---------------------------------------------------------------------------

//lida com os detalhes da criacao das parts para manter o construtor limpo
class partFactory{
    public:
        //cria a part na posicao passada.
        static part* newPart(enemy* e, int position, int partno);
};

//---------------------------------------------------------------WHEEL---------------------------------------------------------------------------

//roda do robo
class wheel : public part {
    friend class enemy;

    public:
        wheel(enemy* e, cpVect offset);
        virtual ~wheel();

        void color();
        void update();
        void attach(enemy* e, int position); //funcao que anexa a roda a carcassa
};

//---------------------------------------------------------------CARCASS---------------------------------------------------------------------------

//carcassa do robo (o quadrado no centro)
class carcass : public part {
    friend class enemy;

    public:
        carcass(enemy* e);
        virtual ~carcass();

        void attach(enemy* e, int position){};

        void color();
        void update(){};
        void show();
};

//---------------------------------------------------------------PROPELLER---------------------------------------------------------------------------

//helice
class propeller : public part {
    friend class enemy;

    public:
        propeller(enemy* e, cpVect offset);
        virtual ~propeller();

        void color();
        void update();
        void attach(enemy* e, int position);

        float potency;     //determina forca com q o propeller puxa o enemy
        int spriteCounter; //para gerenciar a animacao da helice
};

//---------------------------------------------------------------CANNON---------------------------------------------------------------------------

//canhao
class cannon : public part {
    friend class enemy;
    friend class bullet;

    public:
        cannon(enemy* e, cpVect offset);
        virtual ~cannon();
        void show(SDL_Renderer* gRenderer);

        void color();
        void update();
        void attach(enemy* e, int position);

        float rateOfFire;  //tiros por segundo
        float potency;     //fator q multiplica o impulso do tiro quando sai
        float potencyVar;  //variacao do impulso de saida entre os tiros.
        int bulletDelay;
        int bulletCounter; //conta frames até que a proxima bullet possa ser disparada (quando == bulletDelay)
        std::vector<bullet*> bullets; //vector com as bullets que o canhao disparou
};

//---------------------------------------------------------------BULLET---------------------------------------------------------------------------

//tiro do canhao
class bullet
{
    friend class cannon;
    friend class enemy;
    public:
        bullet(cannon* p);
        virtual ~bullet();
        void show(SDL_Renderer* gRenderer);
        cpVect getDrawPos();
        cpBody* getBody(){
            return bulletBody;
        }
        enemy* getEnemy(){
            return e;
        }
    private:
        enemy* e;
        cannon* cann;
        cpBody* bulletBody;
        cpShape* bulletShape;
        cpVect pos;
        SDL_Rect draw_pos;
        float direction;       //direcao pra onde ela sera lancada
        SDL_Texture* sprites;
        SDL_Rect curSprite;
        SDL_Rect dimensions;
        cpFloat mass;
        cpFloat moment;
};

#endif // ENEMY_H
