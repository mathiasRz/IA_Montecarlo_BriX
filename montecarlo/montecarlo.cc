#include "montecarlo.hh"

arbreNaire montecarlo::getArbre() const{
    return _arbre;
}


//fonction qui enregistre dans un fichier l'arbre donné par l'apprentissage terminé
void montecarlo::enregistrerFichier() const{
    std::ofstream monFlux("../projet/data.txt");  //On essaye d'ouvrir le fichier

    if(monFlux)  //On teste si tout est OK
    {
        for (const auto &i : _arbre.getArbreNaire()){
            monFlux << i.getNoeudOrigine()->getId() << " " << i.getNoeudOrigine()->getaX() << " " << i.getNoeudOrigine()->getoX() << " " << i.getNoeudOrigine()->getaO() << " "
                    << i.getNoeudOrigine()->getoO() << " "<< i.getNoeudOrigine()->getNbrGainCumule() << " " << i.getNoeudOrigine()->getIdPere() << std::endl;

        }
    }
    else
    {
        std::cout << "ERREUR: Impossible d'ouvrir le fichier." << std::endl;
    }
}


//fonction qui charge l'arbre contenu dans un fichier
arbreNaire montecarlo::chargerFichier(){
    std::ifstream monFlux("../projet/data.txt");  //Ouverture d'un fichier en lecture
    arbreNaire arbre;
    if(monFlux) {
        while (monFlux.good()){
            std::vector<int> e;

            for (int i=0;i<7;++i){
                int x;
                monFlux >> x;
                e.push_back(x);
            }

            std::shared_ptr<noeud> val(new noeud(e[0],e[1],e[2],e[3],e[4],e[6]));
            val->setNbrGainCummule(e[5]);
            std::vector<std::shared_ptr<noeud>> noeudDest;
            relationNaire r (val,noeudDest);
            arbre.ajout(r);
            if (e[6]!=-1)
                arbre.ajoutfils(e[6],val);
        }
        monFlux.close();
    }
    else {
        std::cout << "lecture impossible, impossible d'ouvrir le fichier !" << std::endl;
    }
    return arbre;
}



//methode utilisé lors de l'apprentissage, pour la selection de chaque coup lors du roll-out
void montecarlo::coupSemiRandom (Jeu jeu,Brix &coup) const{
    std::vector<Brix> coupValide;
    Brix b_canditate;
    int tour = jeu.nbCoupJoue()+1;//la b_candidate devra être valide au tour auquel on va la jouer,i.e. au tour suivant
    int place_coup_joue, taille;
    int aX, oX, aO, oO; //coordonnees de la Brix que l'on va joué

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
        }//fin du for.

    //On regarde si il y a un coup qui peut faire gagner le joueur a qui c'est le tour, si oui on le joue
    //cela ameliorera les resultats de notre arbre au lieu de finir la partie complètement aléatoirement
    bool coupTrouve = false;
    for (const auto & c : coupValide){
        Jeu jeuCopie = jeu;
        jeuCopie.joue(c);
        if (tour%2!=0){
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


    //sinon on choisit un coup au hazard
    if (!coupTrouve){
        taille =coupValide.size();
        place_coup_joue= taille == 1 ?  0 : rand() % (taille-1);
        aO= coupValide[place_coup_joue].getAo();
        oO= coupValide[place_coup_joue].getOo();
        aX= coupValide[place_coup_joue].getAx();
        oX= coupValide[place_coup_joue].getOx();
        coup.setAllCoord(aX, oX, aO, oO);
    }
}


// methode de gestion du roll-out, termine la partie et transmets aux noeuds pacourus de l'arbre les infomations sur la fin de cette partie
void montecarlo::simulePartie (Jeu &jeu, const std::vector<int> &IdNoeudsParcourus){
    Brix coup;
    while (!jeu.fini()){
        coupSemiRandom(jeu,coup);
        jeu.joue(coup);
    }
    if (jeu.partie_O()) {
        _arbre.completeArbre(1,IdNoeudsParcourus);
    }
    else if (jeu.partie_X()) {
        _arbre.completeArbre(-1,IdNoeudsParcourus);
    }
    else if (jeu.partie_nulle()){
        _arbre.completeArbre(0,IdNoeudsParcourus);
    }
}


//renvoie la liste des coups possibles a un moment donné, methode copié sur joueur_random
std::vector<Brix> montecarlo::coupsPossibles (const Jeu &jeu)const{
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


    //On regarde si il y a un coup qui peut faire gagner le joueur a qui c'est le tour, si oui on le joue
    //cela est sensé améliorer la qualité de notre arbre contre un joueur intelligent supposé jouer un coup gagnant si il y en a un
    std::vector<Brix> coupVictorieux;
    bool coupTrouve = false;
    for (const auto & c : coupValide){
        Jeu jeuCopie = jeu;
        jeuCopie.joue(c);
        if (tour%2!=0){
            if (jeuCopie.partie_X()){
                b_canditate.setAllCoord(c.getAx(),c.getOx(),c.getAo(),c.getOo());
                coupVictorieux.push_back(b_canditate);
                coupTrouve=true;
                break;
            }
        }
        else {
            if (jeuCopie.partie_O()){
                b_canditate.setAllCoord(c.getAx(),c.getOx(),c.getAo(),c.getOo());
                coupVictorieux.push_back(b_canditate);
                coupTrouve=true;
                break;
            }
        }
    }
    if (coupTrouve)
        return coupVictorieux;
    else
        return coupValide;
}

//la methode d'apprentissage qui construit notre arbre montecarlo
void montecarlo::apprentissage (Jeu &jeu, const int &nbParties) {
    // construction de la racine de l'arbre
    int id = 0;
    auto racine = std::make_shared<noeud>(0,0,0,0,0,-1);
    id++;
    std::vector<std::shared_ptr<noeud>> vide;
    relationNaire r(racine,vide);
    _arbre.ajout(r);
    int NbSimulation=0;
    //premier acroissement pas de descente, on roll-out directement
    std::vector<int> IdNoeudsParcourusUNIQ;//unique utilisation pour le premier coup
    IdNoeudsParcourusUNIQ.push_back(0);
    simulePartie(jeu,IdNoeudsParcourusUNIQ);
    int i;
    //ensuite on deroule l'algorithme avec descente/growth/roll out pour un nombre de parties donné
    while (NbSimulation<nbParties){
        std::vector<int> IdNoeudsParcourus;//vecteur d'id qui va nous permettre de retracer les noeuds parcourus lors d'une itération de l'algorithme et ainsi pouvoir mettre a jour les infos sur ces noeuds
        IdNoeudsParcourus.push_back(0);
        i=0;
        jeu.reset();
        //1ere et deuxieme etape, la descente et le growth
        bool growth = false;
        while (!growth)
        {
            //on regarde le nombre de coup possibles a cette etape
            std::vector<Brix> cp = coupsPossibles(jeu);
            //si les noeuds de l'arbre n'ont pas deja tous les coups possibles a cette etape alors on peut growth
            if (_arbre.getArbreNaire()[i].getNoeudsDest().size() < cp.size()){
                //growth
                int j = _arbre.getArbreNaire()[i].getNoeudsDest().size();
                auto fils = std::make_shared<noeud>(id,cp[j].getAx(),cp[j].getOx(),cp[j].getAo(),cp[j].getOo(),i);
                IdNoeudsParcourus.push_back(id);
                id++;
                //ajout en tant que noeud dans l'arbre
                std::vector<std::shared_ptr<noeud>> vide;
                relationNaire r(fils,vide);
                _arbre.ajout(r);
                //ajout en tant que fils
                _arbre.ajoutfils(i,fils);
                //on joue le coup dans le plateau
                jeu.joue(cp[j]);
                growth=true;
            }
            //sinon on fait la selection exploration/enfoncement selon le calcul de l'UBC, si on est pas sur une feuille
            else if (_arbre.getArbreNaire()[i].getNoeudsDest().size()!=0){
                //on selectionne le coup ayant l' UBC le plus haut
                int indNoeud = _arbre.rechercheNoeudDescente(i);
                //on se place donc au niveau du nouveau noeud choisi, donc on joue le coup
                int aX=_arbre.getArbreNaire()[i].getNoeudsDest()[indNoeud]->getaX();
                int oX=_arbre.getArbreNaire()[i].getNoeudsDest()[indNoeud]->getoX();
                int aO=_arbre.getArbreNaire()[i].getNoeudsDest()[indNoeud]->getaO();
                int oO=_arbre.getArbreNaire()[i].getNoeudsDest()[indNoeud]->getoO();
                //on joue le coup dans le plateau
                Brix coup (aX,oX,aO,oO);
                jeu.joue(coup);
                //on se place dans l'arbre au niveaux du coup choisit pour la prochaine itération
                i = _arbre.getArbreNaire()[i].getNoeudsDest()[indNoeud]->getId();
                IdNoeudsParcourus.push_back(i);
            }
        }
        // la descente et le growth ont été fait, on rollout
        simulePartie(jeu,IdNoeudsParcourus);
        NbSimulation++;
        //on remonte l'information ce qui est fait avec la methode simulePartie
     }

}

