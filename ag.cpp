#include "ag.h"

using namespace std;

ag::ag(cpSpace* spc, SDL_Texture* enemySprites, bool elitism, float mutation_rate)
{
    this->spc = spc;
    this->enemySprites = enemySprites;
    this->elitism = elitism;
    this->mutation_rate = mutation_rate;
    bestScore = 0;
}

struct compareEnemies {
    bool operator ()(enemy* e1,enemy* e2) const {
        return (e1->Getscore() > e2->Getscore());
    }
} compare;

void ag::evolve(vector<enemy*>& generation, vector<DNA>& nextGeneration){
    unsigned int i, j;
    float totalScore = 0.0;
    float roulette;
    vector<float> cumulativeScore;

    if(elitism)
        if(generation[0]->Getscore() < bestScore)
            generation[0]->Setscore(bestScore); //se desta vez o score do melhor (elite) nao foi tao bom quanto na geracao anterior,
                                                //usa o score da geracao anterior para ele em vez.

    sort(generation.begin(), generation.end(), compare); //ordena a geracao de forma decrescente segundo o score (~fitness)

    if(elitism)                                //atualiza bestScore para o score do melhor individuo dessa geracao
        if(generation[0]->Getscore() > bestScore){
            bestScore = generation[0]->Getscore();
        }

    for(i = 0; i < generation.size(); i++)
        printf("%f\n", generation[i]->Getscore());
    printf("\n");

    for(i = 0; i < generation.size(); i++){     //calcula o score total da geracao (para calcular porcentagem)
        totalScore += generation[i]->Getscore();

    }
    cumulativeScore.clear();
    for(i = 0; i < generation.size(); i++){     //calcula soma cumulativa dos scores (para selecao por roleta)
        cumulativeScore.push_back(generation[i]->Getscore() / totalScore);
        if(i > 0)
            cumulativeScore[i] += cumulativeScore[i-1];
    }

    if(elitism){    //se elitismo == true, o melhor individuo esta na posicao 0. Ja define o dna do primeiro
        i = 0;  //individuo da proxima geracao para ser igual ao dele
        j = 1;
        nextGeneration.push_back( *(generation[0]->getDNA()) );
        while(nextGeneration.size() < generation.size())
        {
            nextGeneration.push_back(crossover(generation[i], generation[j]));
            j++;
        }
    } else {        //se elitism == false sorteia 2 individuos até que sejam distintos.
        do{
            roulette = (float) rand() / (float) RAND_MAX;
            for(i = 0; i < generation.size(); i++)
                if(cumulativeScore[i] > roulette)
                    break;

            roulette = (float) rand() / (float) RAND_MAX;
            for(j = 0; j < generation.size(); j++)
                if(cumulativeScore[j] > roulette)
                    break;
        }while(i == j);

        //preenche o vector de dna da proxima geracao com cruzamentos uniformes dos dois individuos selecionados
        while(nextGeneration.size() < generation.size())
            nextGeneration.push_back(crossover(generation[i], generation[j]));
    }
}

//retorna um dna baseado nos 2 passados, escolhendo cada caracteristica aleatoriamente de um dos dois
DNA ag::crossover(enemy* e1, enemy* e2){
    DNA dna;
    int coin;

    coin = rand() % 3;
    if(coin == 0){
        dna.e_maxLife = e1->getDNA()->e_maxLife;
        dna.e_scale = e1->getDNA()->e_scale ;
    }else if(coin == 1){
        dna.e_scale = e2->getDNA()->e_scale;
        dna.e_maxLife = e2->getDNA()->e_maxLife;
    }else if(coin == 2){
        dna.e_scale = (e2->getDNA()->e_scale + e1->getDNA()->e_scale) / 2;
        dna.e_maxLife = (e2->getDNA()->e_maxLife + e1->getDNA()->e_maxLife) / 2;
    }

    coin = rand() % 2;
    coin == 1 ? dna.top_part = e1->getDNA()->top_part :
                dna.top_part = e2->getDNA()->top_part;

    coin = rand() % 2;
    coin == 1 ? dna.bottom_part = e1->getDNA()->bottom_part :
                dna.bottom_part = e2->getDNA()->bottom_part;

    coin = rand() % 2;
    coin == 1 ? dna.left_part = e1->getDNA()->left_part :
                dna.left_part = e2->getDNA()->left_part;

    coin = rand() % 2;
    coin == 1 ? dna.right_part = e1->getDNA()->right_part :
                dna.right_part = e2->getDNA()->right_part;

    coin = rand() % 3;
    if(coin == 0){
        dna.w_tooth = e1->getDNA()->w_tooth;
        dna.w_elast = e1->getDNA()->w_elast;
        dna.w_frict = e1->getDNA()->w_frict;
    }else if(coin == 1){
        dna.w_tooth = e2->getDNA()->w_tooth;
        dna.w_elast = e2->getDNA()->w_elast;
        dna.w_frict = e2->getDNA()->w_frict;
    }else if(coin == 2){
        dna.w_tooth = e1->getDNA()->w_tooth;
        dna.w_elast = (e2->getDNA()->w_elast + e1->getDNA()->w_elast) / 2;
        dna.w_frict = (e2->getDNA()->w_frict + e1->getDNA()->w_frict) / 2;
    }

    coin = rand() % 2;
    coin == 1 ? dna.c_elast = e1->getDNA()->c_elast :
                dna.c_elast = e2->getDNA()->c_elast;

    coin = rand() % 2;
    coin == 1 ? dna.c_frict = e1->getDNA()->c_frict :
                dna.c_frict = e2->getDNA()->c_frict;

    coin = rand() % 3;
    if(coin == 0){
        dna.w_torque = e1->getDNA()->w_torque;
    }else if(coin == 1){
        dna.w_torque = e2->getDNA()->w_torque;
    }else if(coin == 2){
        dna.w_torque = (e2->getDNA()->w_torque + e1->getDNA()->w_torque) / 2;
    }

    coin = rand() % 2;
    coin == 1 ? dna.p_base_potency = e1->getDNA()->p_base_potency :
                dna.p_base_potency = e2->getDNA()->p_base_potency;

    coin = rand() % 2;
    coin == 1 ? dna.p_var_potency = e1->getDNA()->p_var_potency :
                dna.p_var_potency = e2->getDNA()->p_var_potency;

    coin = rand() % 3;
    if(coin == 0){
        dna.c_rate_of_fire = e1->getDNA()->c_rate_of_fire;
    }else if(coin == 1){
        dna.c_rate_of_fire = e2->getDNA()->c_rate_of_fire;
    }else if(coin == 2){
        dna.c_rate_of_fire = (e2->getDNA()->c_rate_of_fire + e1->getDNA()->c_rate_of_fire) / 2;
    }

    coin = rand() % 3;
    if(coin == 0){
        dna.c_rate_of_fire = e2->getDNA()->c_rate_of_fire;
    }else if(coin == 1){
        dna.c_rate_of_fire = e2->getDNA()->c_rate_of_fire;
    }else if(coin == 2){
        dna.c_rate_of_fire = (e2->getDNA()->c_rate_of_fire + e1->getDNA()->c_rate_of_fire) / 2;
    }

    coin = rand() % 3;
    if(coin == 0){
        dna.c_potency = e1->getDNA()->c_potency;
    }else if(coin == 1){
        dna.c_potency = e2->getDNA()->c_potency;
    }else if(coin == 2){
        dna.c_potency = (e1->getDNA()->c_potency + e2->getDNA()->c_potency) / 2;
    }

    coin = rand() % 2;
    coin == 1 ? dna.c_potency_var = e1->getDNA()->c_potency_var :
                dna.c_potency_var = e2->getDNA()->c_potency_var;

    mutate(&dna);

    return dna;
}

//altera genes por valor aleatorio (num range especifico).
void ag::mutate(DNA* dna){
    if(mutation_rate > 0){
         if((float)rand() / (float)RAND_MAX < mutation_rate){
            swap(dna->top_part, dna->bottom_part);
         }

        if((float)rand() / (float)RAND_MAX < mutation_rate) //menos provavel mutar o tamanho
            dna->e_scale = 0.7 + ((float) rand() / (float) RAND_MAX)*0.7;

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->e_maxLife = rand() % MAX_ENEMY_LIFE;

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->top_part = rand() % 4;

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->bottom_part = rand() % 4;

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->left_part = rand() % 4;

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->right_part = rand() % 4;

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->w_tooth = rand() % 2 == 0 ? false : true;

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->w_torque = (((float) rand() / (float) RAND_MAX) * 200000  - 100000) * dna->e_scale; //-100,000 ~ +100,000

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->w_elast = dna->w_tooth ? 0 : (float) rand() / (float) RAND_MAX;

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->w_frict = dna->w_tooth ? 1 : (float) rand() / (float) RAND_MAX;

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->p_base_potency = (12000 + rand() % 8000)*dna->e_scale*dna->e_scale;

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->p_var_potency = rand() % 2000;

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->c_rate_of_fire = (1 + (rand() % 12))*.2;

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->c_potency = 10 + rand() % 15;

        if((float)rand() / (float)RAND_MAX < mutation_rate)
            dna->c_potency_var = rand()%8;
    }
}

//cria um DNA aleatorio
DNA ag::getRandomDNA(){
    DNA dna;
    dna.e_scale = 1;
    dna.e_maxLife = (rand()%MAX_ENEMY_LIFE)/2;
    dna.top_part = rand()%4;
    dna.bottom_part = rand()%4;
    dna.left_part = rand()%4;
    dna.right_part = rand()%4;
    dna.c_elast = (float) rand() / (float) RAND_MAX;
    dna.c_frict = (float) rand() / (float) RAND_MAX;
    dna.w_tooth = rand() % 2 == 0 ? false : true;
    dna.w_torque = MAX_WHEEL_TORQUE * (((float) rand() / (float) RAND_MAX) * 2 - 1);
    dna.w_elast = dna.w_tooth ? 0 : (float) rand() / (float) RAND_MAX;
    dna.w_frict = dna.w_tooth ? 1 : (float) rand() / (float) RAND_MAX*2 + 0.5;
    dna.p_base_potency = 12000 + rand() % 6000;
    dna.p_var_potency = rand() % 1000;
    dna.c_rate_of_fire = (2 + (rand() % 5))*.2;
    dna.c_potency = 10 + rand() % 10;
    dna.c_potency_var = rand()%8;
    return dna;
}
