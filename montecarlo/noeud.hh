#pragma once
#include <string>
#include <iostream>
#include <memory>


class noeud
{
    public:
        noeud() =default;
        noeud(const unsigned int &id, const int &aX, const int &oX, const int &aO, const int &oO, const int &idPere);

        const int &getId() const ;

        const int &getNbrGainCumule() const;
        const int &getNbrFoisTraverse() const;
        void setNbrGainCummule (const int &n);
        void setNbrFoisTraverse ();
        void setNbrFoisTraverse (const int &v);
        const int &getaX() const;
        const int &getoX() const;
        const int &getaO() const;
        const int &getoO() const;
        const int &getIdPere()const;
    private:
        int _id;
        int _idPere;
        //les 4 données pour un brix
        int _aX;
        int _oX;
        int _aO;
        int _oO;
        int _nbrGainCummule;
        int _nbrFoisTraverse;
};
