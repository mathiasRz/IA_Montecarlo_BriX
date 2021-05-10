#pragma once
#include "noeud.hh"
#include <vector>

class relationNaire {
    public:
        relationNaire(const std::shared_ptr<noeud> &noeudOrigine, const std::vector<std::shared_ptr<noeud> > &noeudsDest);

        const std::shared_ptr<noeud> &getNoeudOrigine() const;
        std::shared_ptr<noeud> &setNoeudOrigine();
        const std::vector<std::shared_ptr<noeud> > &getNoeudsDest() const;
        std::vector<std::shared_ptr<noeud>> &setNoeudsDest();
        void setNoeudsDest(std::shared_ptr<noeud> &n);
        void setNoeudOrigin (const int &result);

    private:
        std::shared_ptr<noeud> _noeudOrigine;
        std::vector<std::shared_ptr<noeud>> _noeudsDest;
};

