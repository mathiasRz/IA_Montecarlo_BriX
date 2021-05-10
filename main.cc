#include <iostream>
#include "arbitre.hh"


int main()
{
    //initialise la graine du générateur aléatoire
    std::srand(std::time(nullptr));


    // §§§§§§ 3 lignes obligatoires pour charger l'arbre d'aprentissage dans notre joueur Joueur_MonteCarlo_MBR §§§§§§
    montecarlo mc;
    Jeu jeu;
    Joueur_MonteCarlo_MBR::_arbreFichier = mc.chargerFichier();
    // §§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§


    // création de l'Arbitre (joueur jouant en 1er la premiere partie, joueur jouant en 2eme celle-ci , nombre de parties)
    Arbitre a (player::RAND,player::M_1,1000);
    // commence le challenge
    int r= a.challenge();
   return r;

   //return 0;

}
