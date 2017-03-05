#include <stdio.h>
#include <chipmunk.h>
#include <sdl.h>
#include <sdl_image.h>
#include <math.h>
#include <time.h>
#include <vector>

#include "utils.h"
#include "player.h"
#include "chao.h"
#include "enemy.h"
#include "DNA.h"
#include "ag.h"

#define RANDOM_SPAWN rand()%6

void printUsage(){
    printf("usage: jogo [elitism(1|0) mutation_rate(float E [0,1]) generation_size (int >3)\n");
    printf("example: jogo 1 0.07 6\n\n");
    exit(0);
}

void reset(cpSpace*& spc, ag*& algoritmo, bool elitism, float mutation_rate,
           unsigned int gen_size, vector<enemy*>& gen, vector<DNA>& nextgen,
           int& nextDNA, player*& p1, enemy*& e1, enemy*& e2, enemy*& e3, chao*& chao1,
           SDL_Texture* playerSprites, SDL_Texture* enemySprites);

void pause();

int main(int argc, char* argv[]){
    SDL_Window* gWindow = NULL;         //janela grafica do jogo
    SDL_Renderer* gRenderer = NULL;     //renderer (em placa de video)
    SDL_Texture* playerSprites = NULL;  //ponteiro para texturas (
    SDL_Texture* enemySprites = NULL;
    SDL_Event event;
    Uint32 FST, FET, timePassed = 0; //frame start time, frame end time
    ag* algoritmo;
    int elitism = 1;
    unsigned int gen_size = 6;
    float mutation_rate = 0.15;

    //interpretacao da linha de comando
    if(argc > 1){
        if(argc != 4)
            printUsage();
        else {
            if(sscanf(argv[1], "%d", &elitism) != 1)
                printUsage();
            if(sscanf(argv[2], "%f", &mutation_rate) != 1)
                printUsage();
            if(sscanf(argv[3], "%d", &gen_size) != 1)
                printUsage();
            if(elitism != 1 && elitism != 0)
                elitism = 1;
            if(mutation_rate < 0.0 || mutation_rate > 1.0)
                mutation_rate = 0.15;
            if(gen_size < 3)
                gen_size = 6;
        }
    }

    srand(time(NULL));
    bool quit = false;

    init(&gWindow, &gRenderer);

    std::string path = "img/player.png";
    playerSprites = loadTexture(path, gRenderer); //carrega os sprites uma unica vez e passa para os construtures posteriormente
    path = "img/enemy.png";
    enemySprites = loadTexture(path, gRenderer);
    SDL_SetTextureBlendMode(enemySprites, SDL_BLENDMODE_BLEND);

    // Cria um espaco de simulacao fisica
    cpSpace *space = cpSpaceNew();
    cpSpaceSetGravity(space, GRAVITY);

    algoritmo = new ag(space, enemySprites, (bool)elitism, mutation_rate); //inicializa o objeto que aplica o algoritmo genetico

    player *p1 = new player(space, playerSprites);
    vector<enemy*> generation;
    vector<DNA> nextGeneration;
    int nextDNA = 0; //indice do proximo DNA a ser usado na criacao de um enemy

    for(unsigned int i = 0; i < gen_size; i++)
        nextGeneration.push_back(algoritmo->getRandomDNA()); //cria gen_size DNAs aleatorios para serem passados para os enemies

    //cria os tres primeiros enemies e os insere no vector generation (geracao atual)
    generation.push_back( new enemy(RANDOM_SPAWN, space, enemySprites, &nextGeneration[nextDNA++]));
    generation.push_back( new enemy(RANDOM_SPAWN, space, enemySprites, &nextGeneration[nextDNA++]));
    generation.push_back( new enemy(RANDOM_SPAWN, space, enemySprites, &nextGeneration[nextDNA++]));

    //ponteiros para os inimigos atualmente vivos da geracao atual
    enemy* e1 = generation[0];
    enemy* e2 = generation[1];
    enemy* e3 = generation[2];

    //cria o chao
    chao* chao1 = new chao(space);

    //inicializa handlers de colisoes (implementado em utils.c)
    setUpCollisionHandlers(p1, space);

    //quantidade de tempo que a engine fisica ira passar por iteracao (utils.h)
    cpFloat timeStep = TIMESTEP;

    //mapa de teclas pressionadas
    bool keys[15]={false};

    while(!quit){
        int killCount = 0; //contador de inimigos destruidos
        int phys_it = 0;   //contador de iteracoes da engine fisica
        while( phys_it < 4 ) //4 steps da engine fisica para cada update da tela
        {
            FST = SDL_GetTicks(); //pega quantos milissegundos ja passaram desde o começo do programa (para calculo do fps)

            if(!p1->getAlive())   //reseta o jogo se o jogador morreu
                reset(space, algoritmo, (bool)elitism, mutation_rate, gen_size, generation, nextGeneration, nextDNA,
                                   p1, e1, e2, e3, chao1, playerSprites, enemySprites);


            //pega eventos da fila para tratar
            while( SDL_PollEvent( &event ) ){
                if( event.type == SDL_QUIT ) //fecha o programa se clicar no X
                    quit = true;

                if (event.type == SDL_KEYDOWN){   //trata eventos de tecla pressionada
                    switch(event.key.keysym.sym){ //ve qual tecla foi pressionada

                        case SDLK_ESCAPE:         // fecha o programa se apertar esc
                            quit = true; break;
                        case SDLK_UP:
                            keys[KEY_UP] = true; break; //atualiza mapa de teclas pressionadas
                        case SDLK_LEFT:
                            keys[KEY_LEFT] = true; break;
                        case SDLK_RIGHT:
                            keys[KEY_RIGHT] = true; break;
                        case SDLK_2:
                            keys[KEY_2] = true; break;
                        case SDLK_3:
                            p1->stab('p'); break;
                        case SDLK_RETURN:
                            pause(); break;
                        case SDLK_r:
                            reset(space, algoritmo, (bool) elitism, mutation_rate, gen_size, generation, nextGeneration, nextDNA,
                                   p1, e1, e2, e3, chao1, playerSprites, enemySprites);
                            break;

                    }
                }else if (event.type == SDL_KEYUP){  //trata eventos de release de tecla
                    switch(event.key.keysym.sym){

                        case SDLK_UP:
                            keys[KEY_UP] = false; break;
                        case SDLK_LEFT:
                            keys[KEY_LEFT] = false; break;
                        case SDLK_RIGHT:
                            keys[KEY_RIGHT] = false; break;
                        case SDLK_2:
                            if(p1->getBreakableJump())
                                p1->breakJump();
                            keys[KEY_2] = false; break;
                        case SDLK_3:
                            p1->stab('r'); break;
                        case SDLK_0:
                            p1->gainBonusLife(50);
                    }
                }
            }

            p1->stop(); //aumenta o atrito do player para que ele pare

            if(keys[KEY_2])
                p1->jump();

            if(keys[KEY_LEFT]) //faz o player ir para esquerda (abaixa o atrito para o movimento ser mais fluido)
                p1->acc_left();

            if(keys[KEY_RIGHT])
                p1->acc_right();

            if(keys[KEY_UP])
                p1->stabUp();

            //checa se algum inimigo morreu e cria um novo se sim.
            if(e1->Getlife() <= 0 && e1->Getalive()){
                killCount++;
                p1->gainBonusLife(5); //player ganha um pouco de vida ao destruir o inimigo
                e1->die();
                if(generation.size() < gen_size){
                    generation.push_back(new enemy(RANDOM_SPAWN, space, enemySprites, &nextGeneration[nextDNA]));
                    e1 = generation[nextDNA++];
                }
            }
            if(e2->Getlife() <= 0 && e2->Getalive()){
                killCount++;
                p1->gainBonusLife(5);
                e2->die();
                if(generation.size() < gen_size){
                    generation.push_back(new enemy(RANDOM_SPAWN, space, enemySprites, &nextGeneration[nextDNA]));
                    e2 = generation[nextDNA++];
                }
            }
            if(e3->Getlife() <= 0 && e3->Getalive()){
                killCount++;
                p1->gainBonusLife(5);
                e3->die();
                if(generation.size() < gen_size){
                    generation.push_back(new enemy(RANDOM_SPAWN, space, enemySprites, &nextGeneration[nextDNA]));
                    e3 = generation[nextDNA++];
                }
            }

            {   //checa se todos da geracao atual morreram para iniciar uma nova
                bool everyBodyDead = true;
                for(unsigned int j = 0; j < generation.size(); j++)
                    if(generation[j]->Getalive()){
                        everyBodyDead = false;
                        break;
                    }
                if(everyBodyDead){
                    nextDNA = 0;
                    p1->incGenClears();

                    nextGeneration.clear(); //limpa o vector de DNAs dessa geracao (para o AG colocar novos)

                    //cria uma nova geracao a partir da atual (preenche o vector nextGeneration com novos DNAs)
                    algoritmo->evolve(generation, nextGeneration);

                    for(unsigned int j = 0; j < generation.size(); j++){ //deleta geracao atual
                        delete generation[j];
                    }
                    generation.clear();

                    //cria os 3 novos enemies iniciais da geracao atual com base nos novos DNAs e atualiza os ponteiros para eles
                    generation.push_back( new enemy(RANDOM_SPAWN, space, enemySprites, &nextGeneration[nextDNA++])); //comeca nova geracao
                    generation.push_back( new enemy(RANDOM_SPAWN, space, enemySprites, &nextGeneration[nextDNA++]));
                    generation.push_back( new enemy(RANDOM_SPAWN, space, enemySprites, &nextGeneration[nextDNA++]));

                    e1 = generation[0];
                    e2 = generation[1];
                    e3 = generation[2];
                }
            }

            //atualiza player (apenas faz perder vida. as colisoes sao tratadas pela engine fisica
            //e a logica das colisoes importantes eh tratada por callbacks
            p1->update();

            //atualiza enemies (faz cannons atirarem se houver, varia empuxo dos propellers)
            e1->update();
            e2->update();
            e3->update();

            //passa um step de tempo na engine
            cpSpaceStep(space, timeStep);
            phys_it++;

        } //só sai aqui depois de 4 iteracoes fisicas (simulacao mais precisa do que usar timestep grande)

        //limpa a tela
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );


        //atualiza renderer
        e1->show( gRenderer );
        e2->show( gRenderer );
        e3->show( gRenderer );
        p1->show( gRenderer );
        chao1->show( gRenderer );

        //redesenha a tela
        SDL_RenderPresent( gRenderer );

        //pega o tempo desde o inicio do programa e faz uma conta para determinar o delay
        //(para completar o tempo do frame e manter a taxa a 60fps)
        FET = SDL_GetTicks();
        timePassed = FET - FST;
        //printf("t:%d, delay: %d\n", timePassed, TARGET_FRAME_TIME - timePassed);
        if(timePassed < TARGET_FRAME_TIME)
            SDL_Delay(TARGET_FRAME_TIME - timePassed);
    } // fim do loop de jogo

    //limpa a bagunca
    delete p1;
    delete chao1;
    for(unsigned int i = 0; i < generation.size(); i++){
        if(generation[i]->Getalive())
            generation[i]->die();
        delete generation[i];
    }
    delete algoritmo;
    generation.clear();
    nextGeneration.clear();
    cpSpaceFree(space);
    SDL_DestroyTexture( playerSprites );
    SDL_DestroyTexture( enemySprites );
    close(gWindow, gRenderer);

    return 0;
}

void reset(cpSpace*& spc, ag*& algoritmo, bool elitism, float mutation_rate,
           unsigned int gen_size, vector<enemy*>& gen, vector<DNA>& nextgen,
           int& nextDNA, player*& p1, enemy*& e1, enemy*& e2, enemy*& e3, chao*& chao1,
           SDL_Texture* playerSprites, SDL_Texture* enemySprites){

//deleta tudo

    delete p1;
    delete chao1;
    for(unsigned int i = 0; i < gen.size(); i++){
        if(gen[i]->Getalive())
            gen[i]->die();
        delete gen[i];
    }
    delete algoritmo;
    gen.clear();
    nextgen.clear();
    cpSpaceFree(spc);

//recria tudo

    spc = cpSpaceNew();
    cpSpaceSetGravity(spc, GRAVITY);

    algoritmo = new ag(spc, enemySprites, elitism, mutation_rate);

    nextDNA = 0;

    for(unsigned int i = 0; i < gen_size; i++)
        nextgen.push_back(algoritmo->getRandomDNA());

    gen.push_back( new enemy(RANDOM_SPAWN, spc, enemySprites, &nextgen[nextDNA++]));
    gen.push_back( new enemy(RANDOM_SPAWN, spc, enemySprites, &nextgen[nextDNA++]));
    gen.push_back( new enemy(RANDOM_SPAWN, spc, enemySprites, &nextgen[nextDNA++]));

    e1 = gen[0];
    e2 = gen[1];
    e3 = gen[2];

    p1 = new player(spc, playerSprites);

    chao1 = new chao(spc);

    setUpCollisionHandlers(p1, spc);
}

void pause(){
    bool quit = false;
    SDL_Event event;
    SDL_Delay(500);
    while(!quit){
        while( SDL_PollEvent( &event ) ){
            if( event.type == SDL_QUIT ) //fecha o programa se clicar no X
                quit = true;

            if (event.type == SDL_KEYDOWN){   //trata eventos de tecla pressionada
                switch(event.key.keysym.sym){ //ve qual tecla foi pressionada

                    case SDLK_ESCAPE:         // fecha o programa se apertar esc
                        quit = true; break;
                    case SDLK_RETURN:
                        quit = true;
                }
            }
        }
    }
}
