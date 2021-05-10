#include "arbre.hh"

const std::vector<relationNaire> &arbreNaire::getArbreNaire() const{
    return _arbreNaire;
}

std::vector<relationNaire> &arbreNaire::setArbreNaire(){
    return _arbreNaire;
}

//ajoute une relationNaire dans l'arbre
void arbreNaire::ajout(relationNaire &r){
    _arbreNaire.push_back(r);
}

//retourne l'id du noeud pere du noeud courant
int arbreNaire::getIdPere(const int &id) const{
    for (auto &r : _arbreNaire){
        for (auto &d : r.getNoeudsDest())
            if (d->getId()==id) return r.getNoeudOrigine()->getId();
        }
}

//ajoute un noeud fils a un noeud a partir de son ID
void arbreNaire::ajoutfils(const int &origine, std::shared_ptr<noeud> &n){
    _arbreNaire[origine].setNoeudsDest(n);
}


//sert pour l'aprentissage, on met a jour tout le noeuds parcourus apres un roll-out
void arbreNaire::completeArbre(const int &result, const std::vector<int> &IdNoeudsParcourus){
    for (auto &i : IdNoeudsParcourus){
        _arbreNaire[i].setNoeudOrigine()->setNbrFoisTraverse();
        _arbreNaire[i].setNoeudOrigine()->setNbrGainCummule(result);
    }
}

//sert pour l'apprentissage , calcul l'UBC sur les noeuds d' une profondeur et selectionne celui qui a l'UBC maximum
int arbreNaire::rechercheNoeudDescente(const int &indNoeudOrigine) const{
    float max = 0;
    int i = 0;
    int indNoeud = 0;
    for (const auto &r : _arbreNaire[indNoeudOrigine].getNoeudsDest()){
        float val = (r->getNbrGainCumule()/r->getNbrFoisTraverse()) + sqrt(2*(log(_arbreNaire[indNoeudOrigine].getNoeudOrigine()->getNbrFoisTraverse())/r->getNbrFoisTraverse()));
        if (val > max){
            max=val;
            indNoeud=i;
        }
        ++i;
    }
    return indNoeud;
}

