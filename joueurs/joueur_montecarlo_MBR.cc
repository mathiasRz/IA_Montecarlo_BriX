#include "joueur_montecarlo_MBR.hh"


Joueur_MonteCarlo_MBR::Joueur_MonteCarlo_MBR(std::string nom, bool joueur)
    :Joueur(nom,joueur)
{}

arbreNaire Joueur_MonteCarlo_MBR::_arbreFichier;

/*char Joueur_MonteCarlo_::nom_abbrege() const
{
    return 'M';
}
*/

//methode qui permet de sélectionner dans notre arbre d'aprentissage le coup qui vient d'etre jouer par notre adversaire, en récuperant son id
int Joueur_MonteCarlo_MBR::selectionneNoeud(const int &aX,const int &oX,const int &aO,const int &oO){

    if (_arbreFichier.getArbreNaire()[_id].getNoeudsDest().size()==0)
        return 0;
    for (const auto &a : _arbreFichier.getArbreNaire()[_id].getNoeudsDest()){
        if (a->getaX()==aX && a->getoX()==oX && a->getaO()==aO && a->getoO()==oO)
            return a->getId();
    }
    return 0;
}

// selectionne le meilleure coup a jouer selon notre arbre en fonction du nombre de gain cumulé de chaques coups possibles
void Joueur_MonteCarlo_MBR::selectionneMeilleurCoup(Brix &coup){

    if (_arbreFichier.getArbreNaire()[_id].getNoeudsDest().size()!=0){
        std::shared_ptr<noeud> noeud;
        noeud = _arbreFichier.getArbreNaire()[_id].getNoeudsDest()[0];
        for (const auto &a : _arbreFichier.getArbreNaire()[_id].getNoeudsDest()){
            if (_joueurX){
                //on choisit le coup avec le gain cumule le plus bas ( car dans notre cas quand X gagne on remonte -1 dans notre arbre  et 1 quand O gagne)
                if (a->getNbrGainCumule()<noeud->getNbrGainCumule())
                    noeud=a;
            }
            else {
                //on choisit le coup avec le gain cumule le plus haut
                if (a->getNbrGainCumule()>noeud->getNbrGainCumule())
                    noeud = a;
            }

        }
        //on selectionne l'id du coup choisit pour la suite
        _id = noeud->getId();
        //puis on joue le coup
        coup.setAx(noeud->getaX());
        coup.setOx(noeud->getoX());
        coup.setAo(noeud->getaO());
        coup.setOo(noeud->getoO());
        coup.setDefinie(true);

    }
    //sinon pas de coups possibles dans l'arbre on a donc plus de solutions dedans
    else
        _parcourArbreFini=true;
}

//renvoie la liste des coups possibles a un moment donné, methode copié sur joueur_random
std::vector<Brix> Joueur_MonteCarlo_MBR::coupsPossibles (const Jeu &jeu)const{
    std::vector<Brix> coupValide;
    Brix b_canditate;
    int tour = jeu.nbCoupJoue()+1;//la b_candidate devra être valide au tour auquel on va la jouer,i.e. au tour suivant

    for (int i = 0; i<MAX_LARGEUR;i++) //i abscisse donc numero de colonne
        {
            int j=0; //j ordonnee donc numero de ligne
            while(j<MAX_HAUTEUR && jeu.plateau()[j][i]!='0') //dans plateau l'ordonnee en premier
                {
                    j++;
                }
            if (j<MAX_HAUTEUR)//On est tombé sur une case vide, i.e. contenant '0'
                {
                    //Cherchons des coups valides à partir de cette case vide.
                    b_canditate.setAllCoord(i,j ,i,j+1); //brix verticale dont le bottom est X
                    if (jeu.coup_licite(b_canditate,tour))
                        {
                            coupValide.push_back(b_canditate);
                        }

                    b_canditate.setAllCoord(i,j+1,i,j); //brix verticale dont le bottom est O
                    if (jeu.coup_licite(b_canditate,tour))
                        {
                            coupValide.push_back(b_canditate);
                        }

                    b_canditate.setAllCoord(i,j ,i+1,j); //brix horizontale commençant par X
                    if (jeu.coup_licite(b_canditate,tour))
                        {
                            coupValide.push_back(b_canditate);
                        }

                    b_canditate.setAllCoord(i+1,j,i,j); //brix terminant commençant par X
                    if (jeu.coup_licite(b_canditate,tour))
                        {
                            coupValide.push_back(b_canditate);
                        }
                    //fin des brix valide à cette abscisse et cette ordonnee
                }
        }
    return coupValide;
}


void Joueur_MonteCarlo_MBR::recherche_coup(Jeu jeu, Brix &coup)
{
    // si c'est le premier coup, on a les X
    if (jeu.nbCoupJoue()==0){
        //on initialise les variables utiles pour la partie
        _joueurX=true;
        _parcourArbreFini=false;
        _id=0;

        //et on cherche le meilleur coup
        this->selectionneMeilleurCoup(coup);
    }
    // si c'est le deuxieme coup, on a les O
    else if (jeu.nbCoupJoue()==1){
        //on initialise les variables utiles pour la partie
        _joueurX=false;
        _parcourArbreFini=false;
        _id=0;

        //on charge le premier coup joué par l'adversaire
        _id=selectionneNoeud(coup.getAx(),coup.getOx(),coup.getAo(),coup.getOo());

        // on cherche le meilleur coup
        this->selectionneMeilleurCoup(coup);
    }
    //sinon les variables sont initialisées, on sait pour quel camp on joue
    else {
        //on charge le dernier coup joué par l'adversaire, si notre arbre a encore des solutions
        if (!_parcourArbreFini){
            _id = selectionneNoeud(coup.getAx(),coup.getOx(),coup.getAo(),coup.getOo());
            //on regarde si apres le coup de l'adversaire notre arbre peut nous donné une solution
            if (_id==0)
                _parcourArbreFini=true;
        }

        //on on cherche le meilleur coup, si l'arbre a des solutions
        if (!_parcourArbreFini){
            this->selectionneMeilleurCoup(coup);
        }

        //si on a plus de solutions on cherche alors les meilleures coups en anticipants le coup prochain de l'adversaire
        if (_parcourArbreFini) {
            //on charge les coup possibles à ce moment
            std::vector<Brix> coupValide = this->coupsPossibles(jeu);

            //On regarde si il y a un coup qui peut nous faire gagner, si oui on le joue
            bool coupTrouve = false;
            for (const auto & c : coupValide){
                Jeu jeuCopie = jeu;
                jeuCopie.joue(c);
                if (_joueurX){
                    if (jeuCopie.partie_X()){
                        coup.setAllCoord(c.getAx(),c.getOx(),c.getAo(),c.getOo());
                        coupTrouve=true;
                        break;
                    }
                }
                else {
                    if (jeuCopie.partie_O()){
                        coup.setAllCoord(c.getAx(),c.getOx(),c.getAo(),c.getOo());
                        coupTrouve=true;
                        break;
                    }
                }
            }


            /*si c'est pas le cas on choisit le premier coup qui ne nous fait pas perdre et qui ne fait pas partie nulle
            * et qui ne risque pas de nous faire perdre par le prochain coup adverse
            * si parmis les solutions trouvées on ne trouve pas de coups qui risquent pas de nous faire perdre lors du tours de l'adversaire
            * on joue le dernier coup possible qui ne nous fait pas perdre et qui ne fait pas partie nulle lors de ce tour */
            if (!coupTrouve){
                int i=0;
                for (const auto & c : coupValide){
                    Jeu jeuCopie = jeu;
                    jeuCopie.joue(c);
                    if (_joueurX){
                        if (!jeuCopie.partie_O() && !jeuCopie.partie_nulle()){
                            std::vector<Brix> coupValide1 = this->coupsPossibles(jeuCopie);
                            bool coupBon = true;
                            for (const auto & c1 : coupValide1){
                                Jeu jeuCopieCopie = jeuCopie;
                                jeuCopieCopie.joue(c1);
                                if (jeuCopieCopie.partie_O()){
                                    coupBon = false;
                                    break;
                                }
                            }
                            if (coupBon || i==coupValide.size()-1){
                                coup.setAllCoord(c.getAx(),c.getOx(),c.getAo(),c.getOo());
                                coupTrouve=true;
                                break;
                            }
                        }
                    }
                    else {
                        if (!jeuCopie.partie_X() && !jeuCopie.partie_nulle()){
                            std::vector<Brix> coupValide1 = this->coupsPossibles(jeuCopie);
                            bool coupBon = true;
                            for (const auto & c1 : coupValide1){
                                Jeu jeuCopieCopie = jeuCopie;
                                jeuCopieCopie.joue(c1);
                                if (jeuCopieCopie.partie_X()){
                                    coupBon = false;
                                    break;
                                }
                            }
                            if (coupBon || i==coupValide.size()-1){
                                coup.setAllCoord(c.getAx(),c.getOx(),c.getAo(),c.getOo());
                                coupTrouve=true;
                                break;
                            }
                        }
                    }
                    ++i;
                }
            }

            //si on a pas trouvé, on joue le premier coup qui fait partie nulle
            if (!coupTrouve){
                for (const auto & c : coupValide){
                    Jeu jeuCopie = jeu;
                    jeuCopie.joue(c);
                    if (jeuCopie.partie_nulle()){
                        coup.setAllCoord(c.getAx(),c.getOx(),c.getAo(),c.getOo());
                        coupTrouve=true;
                        break;
                    }
                }
            }


            //Sinon on choisit le premier coup qui nous fait perdre, pas d'autres choix possibles
            if (!coupTrouve){
                int aO= coupValide[0].getAo();
                int oO= coupValide[0].getOo();
                int aX= coupValide[0].getAx();
                int oX= coupValide[0].getOx();
                coup.setAllCoord(aX, oX, aO, oO);
            }

        }
    }
}






