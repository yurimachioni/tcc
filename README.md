# tcc
This game prototype was developed as a Term Paper for my CS Major at USP São Carlos (University of São Paulo). It's objetcitve is to show the viablity of using evolutive algorithms as a game mechanic and difficulty calibration mechanism.

INTRODUCTION

This is a simple action game with no victory condition in which your objectives are to destroy the enemies as many times as you can and avoid being killed by them. The enemies characteristics are randomly determined at first, but an Evolutive Algorithm adapts the next hordes based in their aptitude to harm you and vice versa to maximazie the game difficulty. There is no mechanism to prevent the game of becoming too difficult. It is expected that after a few generations the enemies become better shielded, have more Health Points and be equipped with weapons that you showed more difficulty in dodging.

The terrain is randomly generated every game to help to diversify the enemies. 

CONTROLS

->  moves character to the right
<-  moves character to the left
^   aims weapon upwards (up arrow)
3   shoot weapon
2   jump
R   reset game
0   more life (cheat)

PHYSICS

All bodies and characters in the game are physically simulated with chipmunk physics to create richer interactions, since the bots dont have an AI, their movement is determined exclusively base on the physics interaction of the player and the forces applied by their body parts (wheels, propellers, cannons).

The robots are just a square shaped physical body that can have 0 to 4 parts attached (one to each side of the square). Since this square is the weakest spot (damage multiplier = 4), it is very common that the Evolutive Algorithm chooses robots that happen to evolve shielding parts around this square core (carcass). The possible parts are:
  1- Wheels: move the robot (if positioned adequately) and absorbs damage (0.5 multiplier)
  2- Saw: move the robot, absorbs damage (x0.5) and deals damage on hit (x3)
  3- Propeller: Applies a force in the opposite direction, takes double damage, deals 1x damage
  4- Cannon: Deals 1x damage per shot, takes 1x damage
These characteristics are not part of the DNA of the individual, therefore the AE does not act on them.

EVOLUTIVE ALGORITHM

The DNA of the enemies (the characteristics that the evolutive algorithm selects and improves) is like follows

struct DNA{
    //enemy
    float e_maxLife;
    float e_scale; //bot size

    int top_part;    //which part is attached to the top of the robot
    int bottom_part;
    int left_part;
    int right_part;

    //carcass
    float c_elast;  
    float c_frict;

    //wheel
    float w_torque;   //characteristics of all the wheels 
    float w_elast;
    float w_frict;
    bool w_tooth; //changes the wheels to a circular saw if true. causes damage in addition to moving

    //propeller
    float p_base_potency; //base potency of the propellers
    float p_var_potency;  //variation of the potency with time (parameter to senoidal function)

    //cannon
    float c_rate_of_fire; 
    float c_potency;     
    float c_potency_var;  
};

SELECTION

This structure is passed as argument to the constructor of each enemy after the Evolutive Algorithm is done with it.
The EA is command line parameterizable in the following way:

"jogo.exe [elitism(1|0) mutation_rate(float E [0,1]) generation_size (int >3)"

For instance, "jogo.exe 1 0.07 6" will enable elitism, set 0.07 as the mutation rate and the generation size will be 6.
That means that after killing 6 enemies, the AE will step in, evaluate the scores of each enemy and start the crossover process, which functions the following way if elitism is used:

  1- Pick the bot with the best score (fitness) to be a parent
  2- Crossover its genes with those of each other bot
  3- With a chance of 7%, randomize each gene
  4- Generate a new 6 individual generation with inherited genes from the best bot of the previous generation crossed with the others, with a (7%) chance of a new characteristic (that does not come from either parent, but from random mutation). The Elite individual is always copied to the next generation too, so it will only be removed from the genetic pool if a better individual appears.
  5- After the player kills everyone again, go to (1) with the new generation.
  
If you choose not to use elitism, roulette selection will be used instead. That goes as follows:
  
  1- Assign each bot a numerical interval between 0 and 1 proportional in size to its score (fitness) in a way that there is no intersections in the intervals, and the union of them is equal to the whole [0,1] interval. Example:
(generation size = 3)

0.00---------bot1-------------0.3-----bot2-----0.4---------------------------------bot3-------------------------1.0

  2- Pick a random number between 0 and 1.
  3- Select the bot whose interval contains that random number to be a parent.
  4- Go to (2) until a new bot is chosen.
  5- Crossover the same two selected bots as many times as needed to fill the next generation (each crossover randomly selects the genes either from parent1 or from parent2, or a mix of the two, so even though the parents are the same, the new individuals are very unlikely to be equal to one another).
  6- With a chance of (mutation_rate) randomize each gene of each son
  7- After the player kills everyone again, go to (1) with the new generation.

FITNESS FUNCTION

The fitness function is not explicitly defined anywhere in the code, but its tied to several difficulty related aspects of the gameplay. Each bot has an internal score to which is added a value any time it shows to be performing well (in the task of killing/not being killed). For instance, if a bot hits the player with a cannon shot, it scores points. The actions that reward points to the score of a bot are:
  -Being hit and not dying
  -Harming the player
  -Survive for one more second
  
When the generation ends (everyone is killed by the player) it is passed to the AE with their score set. The score is used as a fitness measure upon which the selection algorithm is going to act. That means that an enemy that hit the player a lot, survived being hit a lot of times or was just neglected by the player for a long time will have a high fitness and therefore a high chance of being picked as a parent.

After enough generations, we see that the enemies start getting tougher, with more Health Points, their cannons start firing more frequently, their wheels become circular saws and no weak spots are left uncovered. Depending on the difficulty It is not uncommon that the game becomes unplayable or unenjoyable due to the difficulty. Some patterns that occur frequently are:
  - Propeller on top (flying bot) with cannon downward or every other side (high damage, hard to hit)
  - Circular saw on every side (heavy armor, moving, damage if the player misses the jump)
  - Wheels or saws on both sides (doesn't move, but cant be easily hit since you cant hit downwards)
  - Propeller downward, cannon up, cannon front (sticks to ground because of the propeller, shoots everywhere with little recoil)
  - Cannon everywhere (bullet hell)
