#pragma once

#include "../montecarlo/montecarlo.hh"
#include "joueur.hh"
#include "joueur_random.hh"


using ligne=std::array<char,MAX_LARGEUR>;
using board=std::array<ligne,MAX_HAUTEUR>;



class Joueur_MonteCarlo_MBR : public Joueur
{
public:
  Joueur_MonteCarlo_MBR(std::string nom, bool joueur);
  //char nom_abbrege() const override;

  static arbreNaire _arbreFichier;//l'arbre contenant notre apprentissage , chargé dans le main
  void recherche_coup(Jeu, Brix & coup) override;
  void chargerArbre();
  void chargerCoupAdverse(const board &b);
  void selectionneMeilleurCoup(Brix &coup);
  int selectionneNoeud(const int &aX, const int &oX, const int &aO, const int &oO);
  std::vector<Brix> coupsPossibles (const Jeu &jeu)const;

private:
  bool _joueurX;
  bool _parcourArbreFini;
  int _id;
};
