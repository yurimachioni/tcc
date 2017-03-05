#include "utils.h"
#include "player.h"
#include "enemy.h"


//define as funcoes a serem chamadas caso ocorra uma colisao entre
// 2 tipos de shapes de interesse, para fazer o tratamento
void setUpCollisionHandlers(void* p1, cpSpace* space){

    //colisoes do player
    {
        //define handler de colisao entre player e chao (chama funcao wrapper definida abaixo,
        // que chama funcao membro em player)
        cpSpaceAddCollisionHandler(space, PLAYER_TYPE, GROUND_TYPE,
                                    collidePlayerGroundWrapper, NULL, NULL, separatePlayerGroundWrapper, p1);

        cpSpaceAddCollisionHandler(space, PLAYER_TYPE, CARCASS_TYPE,
                                    collidePlayerGroundWrapper, NULL, NULL, separatePlayerGroundWrapper, p1);

        cpSpaceAddCollisionHandler(space, PLAYER_TYPE, WHEEL_TYPE,
                                    collidePlayerGroundWrapper, NULL, NULL, separatePlayerGroundWrapper, p1);

        cpSpaceAddCollisionHandler(space, PLAYER_TYPE, CANNON_TYPE,
                                    collidePlayerGroundWrapper, NULL, NULL, separatePlayerGroundWrapper, p1);

        //destruicao de corpos nao pode ser feito direto na funcao chamada pelo handler,
        // precisa adicionar uma postStep (que eh diferente da postSolve)
        cpSpaceAddCollisionHandler(space, PLAYER_TYPE, KNIFE_TYPE,
                                    collidePlayerKnifeWrapper, NULL, NULL, NULL, p1);

        cpSpaceAddCollisionHandler(space, PLAYER_TYPE, BULLET_TYPE,
                                    collidePlayerBulletWrapper, NULL, NULL, NULL, p1);

        cpSpaceAddCollisionHandler(space, PLAYER_TYPE, TOOTH_WHEEL_TYPE,
                                    collidePlayerToothWheelWrapper, NULL, NULL, NULL, p1);

        cpSpaceAddCollisionHandler(space, PLAYER_TYPE, PROPELLER_TYPE,
                                     collidePlayerPropellerWrapper, NULL, NULL, NULL, p1);
    }

    //colisoes dos enemies
    {
        cpSpaceAddCollisionHandler(space, WHEEL_TYPE, WALL_TYPE,
                                    collideEnemyWallWrapper, NULL, NULL, NULL, NULL);

        cpSpaceAddCollisionHandler(space, TOOTH_WHEEL_TYPE, WALL_TYPE,
                                    collideEnemyWallWrapper, NULL, NULL, NULL, NULL);

        cpSpaceAddCollisionHandler(space, PROPELLER_TYPE, WALL_TYPE,
                                    collideEnemyWallWrapper, NULL, NULL, NULL, NULL);

        cpSpaceAddCollisionHandler(space, KNIFE_TYPE, WHEEL_TYPE,
                                    collideKnifeWheelWrapper, NULL, NULL, NULL, NULL);

        cpSpaceAddCollisionHandler(space, KNIFE_TYPE, TOOTH_WHEEL_TYPE,
                                    collideKnifeWheelWrapper, NULL, NULL, NULL, NULL);

        cpSpaceAddCollisionHandler(space, KNIFE_TYPE, PROPELLER_TYPE,
                                    collideKnifePropellerWrapper, NULL, NULL, NULL, NULL);

        cpSpaceAddCollisionHandler(space, KNIFE_TYPE, CARCASS_TYPE,
                                    collideKnifeCarcassWrapper, NULL, NULL, NULL, NULL);

        cpSpaceAddCollisionHandler(space, KNIFE_TYPE, CANNON_TYPE,
                                    collideKnifeCannonWrapper, NULL, NULL, NULL, NULL);
    }
}

//collision handlers----------------------------------------------------------------------------------------

//(chama funcao definida em player.cpp)
int collidePlayerGroundWrapper(cpArbiter* arb, cpSpace* space, void* p1){
    ((player*)p1)->collidePlayerGround();
    return 1;
}

//chamado quando separa (player e chao)
void separatePlayerGroundWrapper(cpArbiter* arb, cpSpace* space, void* p1){
    ((player*)p1)->separatePlayerGround();
}

//destroi a faca quando ela retorna ao player
int collidePlayerKnifeWrapper(cpArbiter* arb, cpSpace* space, void* p1){
    CP_ARBITER_GET_SHAPES(arb, a, b);
    cpSpaceAddPostStepCallback(space, (cpPostStepFunc)postStepRemoveKnife, b, p1);
    return 1;
}

void postStepRemoveKnife(cpSpace *space, cpShape *shape, void* p1){
    ((player*)p1)->destroyKnife();
}

int collidePlayerBulletWrapper(cpArbiter* arb, cpSpace* space, void* p1){
    CP_ARBITER_GET_BODIES(arb, a, b);
    enemy* e = (enemy*) cpBodyGetUserData(b);
    e->dealDamage(3000);
    ((player*)p1)->takeDamage(e->getDNA()->e_scale * 10);
    cpBodySetVel(b, cpv(0,0)); //para a bala que colidiu para q ela seja removida
    return 1;
}

int collidePlayerToothWheelWrapper(cpArbiter* arb, cpSpace* space, void* p1){
    CP_ARBITER_GET_SHAPES(arb, a, b);
    enemy* e = (enemy*) cpShapeGetUserData(b); //para a bala que colidiu para q ela seja removida
    ((player*)p1)->takeDamage(e->getDNA()->e_scale * 30);
    e->dealDamage(9000);
    return 1;
}

int collidePlayerPropellerWrapper(cpArbiter* arb, cpSpace* space, void* p1){
    CP_ARBITER_GET_SHAPES(arb, a, b);
    enemy* e = (enemy*) cpShapeGetUserData(b); //para a bala que colidiu para q ela seja removida
    ((player*)p1)->takeDamage(e->getDNA()->e_scale * 10);
    e->dealDamage(3000);
    return 1;
}

int collideEnemyWallWrapper(cpArbiter* arb, cpSpace* space, void* notused){
    cpShape *partA, *partB;
    cpArbiterGetShapes(arb, &partA, &partB);
    //pega o enemy a que o shape A da colisao pertence (shape A eh a parte e B eh a parede)
    enemy* e = (enemy*) cpShapeGetUserData(partA); //nao tem problema pegar o shape pq só o ponteiro pra ele eh privado em Part.
    e->flip();
    return 1;
}

int collideKnifeCarcassWrapper(cpArbiter* arb, cpSpace* space, void* notused){
    CP_ARBITER_GET_SHAPES(arb, a, b);
    enemy* e = (enemy*) cpShapeGetUserData(b);
    e->takeDamage(4.0);
    return 1;
}

int collideKnifeWheelWrapper(cpArbiter* arb, cpSpace* space, void* notused){
    CP_ARBITER_GET_SHAPES(arb, a, b);
    enemy* e = (enemy*) cpShapeGetUserData(b);
    e->takeDamage(0.5);
    return 1;
}

int collideKnifePropellerWrapper(cpArbiter* arb, cpSpace* space, void* notused){
    CP_ARBITER_GET_SHAPES(arb, a, b);
    enemy* e = (enemy*) cpShapeGetUserData(b);
    e->takeDamage(2.0);
    return 1;
}

int collideKnifeCannonWrapper(cpArbiter* arb, cpSpace* space, void* notused){
    CP_ARBITER_GET_SHAPES(arb, a, b);
    enemy* e = (enemy*) cpShapeGetUserData(b);
    e->takeDamage(1.0);
    return 1;
}

//collision handlers fim------------------------------------------------------------------------------------

void init(SDL_Window** gWindow, SDL_Renderer** gRenderer){
    //inicializa SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Erro ao iniciar SDL: %s\n", SDL_GetError() );
        exit(-1);
    }

    //cria janela para desenho grafico
    *gWindow = SDL_CreateWindow( "TCC", 50, 50, DEFAULT_RES_W, DEFAULT_RES_H, SDL_WINDOW_SHOWN );
    if( gWindow == NULL )
    {
        printf( "Erro ao criar janela: %s\n", SDL_GetError() );
        exit(-1);
    }

    //cria um renderer para desenhar na janela
    *gRenderer = SDL_CreateRenderer( *gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if( gRenderer == NULL )
    {
        printf( "Erro ao criar renderer: %s\n", SDL_GetError() );
        exit(-1);
    }

    //cor padrao para desenho
    SDL_SetRenderDrawColor( *gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

    //inicializa funcoes para usar imagens em PNG
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        exit(-1);
    }
}

SDL_Texture* loadTexture( std::string& path , SDL_Renderer* gRenderer)
{
    SDL_Texture* newTexture = NULL;  //textura que sera retornada (depois de otimizada)

    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );   //carrega imagem como uma surface (memoria RAM)
    if( loadedSurface == NULL )
        printf( "Erro ao carregar imagem %s! \n%s\n", path.c_str(), IMG_GetError() );
    else
    {
        //cria uma textura a partir da surface em memoria de video
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
        if( newTexture == NULL )
            printf( "Erro ao criar texture a partir da imagem %s! \n%s\n", path.c_str(), SDL_GetError() );
        SDL_FreeSurface( loadedSurface );    //libera imagem em RAM
    }

    return newTexture;
}

void close(SDL_Window* gWindow, SDL_Renderer* gRenderer)
{

    //Destroi renderer e janela
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    //Encerra subsistemas da biblioteca SDL
    IMG_Quit();
    SDL_Quit();
}
