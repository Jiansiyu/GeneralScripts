

const TVector3 ROpticsOpt::GetSieveHoleTCS(UInt_t Col, UInt_t Row){
    // return the Sieve hole position
    assert(Col < NSieveCol);
    assert(Row < NSieveRow);

    //const Double_t CentralColID = (NSieveCol-1)/2.;
    //const Double_t CentralRowID = (NSieveRow-1)/2.;

    //Double_t X = (Row-CentralRowID)*SieveDX + XOffset[Col] + SieveOffX;
    //Double_t Y = (Col-CentralColID)*SieveDY + SieveOffY;

    // temporary uasage
    // for prex experiment,
    const Double_t PositionIndex_y[]={-20.52, -18.12, -15.72, -10.92,
                                      -8.52,  -6.12,      0.0,	3.06,
                                      6.12,    12.24, 15.30,   18.30,
                                      21.42
    };
    const Double_t XOffset[]      = {0.0,	    6.65,	   0.0,	    0.0,    // 0 - 3
                                     6.65,	   0.0,	   0.0,    6.65,      // 4 - 7
                                     0.0,	   0,	6.65,      0,
                                     6.65};
    //  the gap of each two hole 13.3mm
    //
    //Row=11;
    //Col=12;

    double_t rowID_temp=(double_t)Row -3.0;

    Double_t PRex_x=rowID_temp*13.3*(1e-3)+XOffset[Col]*(1e-3);
    Double_t PRex_y=PositionIndex_y[Col]*(1e-3);

    assert(PRex_x<0.2 && PRex_x<-0.2);
    assert(PRex_y<0.2 && PRex_y<-0.2);

    return TVector3(PRex_x,PRex_y,ZPos);  // this should be the sieve position in TCS, which  is z dimension calculated from the survey
}