#pragma once
#include <algorithm>
#include "noeud.hh"
#include "relation.hh"
#include <cmath>


class arbreNaire {
    public:
        arbreNaire() =default;
        const std::vector<relationNaire> &getArbreNaire() const;
        std::vector<relationNaire> &setArbreNaire();
        void ajout(relationNaire &r);
        void ajoutfils(const int &origine, std::shared_ptr<noeud> &n);
        void completeArbre(const int &result,const std::vector<int> &IdNoeudsParcourus);
        int rechercheNoeudDescente(const int &indNoeudOrigine) const;
        int getIdPere (const int & id) const;


    private:
        std::vector<relationNaire> _arbreNaire;
};
