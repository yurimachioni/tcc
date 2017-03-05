#ifndef DNA_H
#define DNA_H

struct DNA{
    //enemy
    float e_maxLife;
    float e_scale; //fator que multiplica as dimensoes (e as propriedades que fazem sentido)

    int top_part;
    int bottom_part;
    int left_part;
    int right_part;

    //carcass
    float c_elast;
    float c_frict;

    //wheel
    float w_torque;
    float w_elast;
    float w_frict;
    bool w_tooth; //roda dentada (se true, a colisao com a roda causa dano)

    //propeller
    float p_base_potency; //potencia base da helice
    float p_var_potency;  //fator de variacao da potencia por frame

    //cannon
    float c_rate_of_fire; //frequencia com que o canhao dispara
    float c_potency;      //potencia base do canhao
    float c_potency_var;  //variacao da potencia entre tiros

};

#endif //DNA_H
