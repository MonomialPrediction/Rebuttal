#include<iostream>
#include<cstdio>
#include<bitset>
#include<vector>
#include<set>
#include<map>
#include<cmath>
#include<fstream>
#include<chrono>
#include"gurobi_c++.h" 

using namespace std;

int depth = 0;

const int MAX = 200000000; // the maximum value of PoolSearchMode, P625

string getCurrentSystemTime()
{
    auto tt = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm* ptm = localtime(&tt);
    char date[60] = { 0 };
    sprintf(date, "%d-%02d-%02d-%02d:%02d:%02d", (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
                                        (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return string(date);
}

struct cmp288
{
    bool operator()( const bitset<288> & a, const bitset<288> & b ) const
    {
        for ( int i = 0; i < 288; i++ )
            if ( a[i] < b[i] ) return true;
            else if ( a[i] > b[i] ) return false;
        return false; // equal
    }
};

struct cmp285
{
    bool operator()( const bitset<285> & a, const bitset<285> & b ) const
    {
        for ( int i = 0; i < 285; i++ )
            if ( a[i] < b[i] ) return true;
            else if ( a[i] > b[i] ) return false;
        return false; // equal
    }
};

void triviumCore(GRBModel& model, vector<GRBVar>& x, int i1, int i2, int i3, int i4, int i5)
{
    GRBVar y1 = model.addVar(0, 1, 0, GRB_BINARY);
    GRBVar y2 = model.addVar(0, 1, 0, GRB_BINARY);
    GRBVar y3 = model.addVar(0, 1, 0, GRB_BINARY);
    GRBVar y4 = model.addVar(0, 1, 0, GRB_BINARY);
    GRBVar y5 = model.addVar(0, 1, 0, GRB_BINARY);

    GRBVar z1 = model.addVar(0, 1, 0, GRB_BINARY);
    GRBVar z2 = model.addVar(0, 1, 0, GRB_BINARY);

    GRBVar a = model.addVar(0, 1, 0, GRB_BINARY);

    // note copy is different from that in CRYPTO 2017 paper
    //copy
    model.addConstr(y1 <= x[i1]);
    model.addConstr(z1 <= x[i1]);
    model.addConstr(y1 + z1 >= x[i1]);

    //copy
    model.addConstr(y2 <= x[i2]);
    model.addConstr(z2 <= x[i2]);
    model.addConstr(y2 + z2 >= x[i2]);

    //copy
    model.addConstr(y3 <= x[i3]);
    model.addConstr(a <= x[i3]);
    model.addConstr(y3 + a >= x[i3]);
    
    //copy
    model.addConstr(y4 <= x[i4]);
    model.addConstr(a <= x[i4]);
    model.addConstr(y4 + a >= x[i4]);
    //xor
    model.addConstr(y5 == x[i5] + a + z1 + z2);

    x[i1] = y1;
    x[i2] = y2;
    x[i3] = y3;
    x[i4] = y4;
    x[i5] = y5;
}

int SecondBackExpandPolynomial( int rounds, bitset<288> final, vector<bitset<288> > & term )
{
    // Create the environ
    GRBEnv env = GRBEnv();
    env.set(GRB_IntParam_LogToConsole, 0);
    env.set(GRB_StringParam_LogFile, "solutions.log" );

    env.set(GRB_IntParam_Presolve, 0);
    env.set(GRB_IntParam_MIPFocus, 1);
    env.set(GRB_IntParam_PoolSearchMode, 1);//focus on finding additional solutions 
    env.set(GRB_IntParam_PoolSolutions, MAX); // try to find 2000000

    GRBModel model = GRBModel(env);

    vector<GRBVar> s(288);
    for (int i = 0; i < 288; i++)
        s[i] = model.addVar(0, 1, 0, GRB_BINARY);

    // Round function
    //  store the initial state 
    vector<GRBVar> works = s;
    for (int r = 0; r < rounds; r++) 
    {
        triviumCore(model, works, 65, 170, 90, 91, 92);
        triviumCore(model, works, 161, 263, 174, 175, 176);
        triviumCore(model, works, 242, 68, 285, 286, 287);
            
        vector<GRBVar> temp = works;
        for (int i = 0; i < 288; i++) 
            works[(i + 1) % 288] = temp[i];
    }

    // Output constraint
    //GRBLinExpr nk = 0;
    //for ( int i = 0; i < 288; i++ )
    //    if ( (i == 65) || (i == 92) || (i == 161) || (i == 176) || (i == 242) || (i == 287))
    //        nk += works[i];
    //    else 
    //        model.addConstr( works[i] == 0);
    //model.addConstr( nk == 1 );
    for ( int i = 0; i < 288; i++ )
        if ( final[i] == 0 )
            model.addConstr( works[i] == 0 );
        else
            model.addConstr( works[i] == 1 );

    model.update();
    model.optimize();

    map<bitset<288>, int, cmp288> counterMap; 

    if( model.get( GRB_IntAttr_Status ) == GRB_OPTIMAL )
    {
        double time = model.get(GRB_DoubleAttr_Runtime );
        cout << "Time Used: " << time << "sec" << endl;
        
        int solCount = model.get(GRB_IntAttr_SolCount);
        cout << "Raw Solutions: " << solCount << endl;

        bitset<288> start;
        for ( int i = 0; i < solCount; i++ )
        {
            model.set(GRB_IntParam_SolutionNumber, i );

            for ( int j = 0; j < 288; j++ ) 
                if ( round( s[j].get( GRB_DoubleAttr_Xn ) ) == 1 )  
                    start[j] = 1;
                else 
                    start[j] = 0;
            counterMap[start]++;
        }
    }
    else if( model.get( GRB_IntAttr_Status ) == GRB_INFEASIBLE )
    {
        cout << "No terms " << endl;
        exit(-2);
    }
    else
    {
        cout << "Other status " << GRB_IntAttr_Status <<  endl;
        exit(-1);
    }

    for ( auto it : counterMap )
        if ( it.second % 2 == 1 )
            term.push_back( it.first );
    cout << "Exact terms: " << term.size() << endl;
}

int  MidSolutionCounter( int rounds, bitset<80> cube, const bitset<288> & last, 
map<bitset<285>, int, cmp285> & counterMap, ostream & f = cout )
{
    //setting the enviroment
    GRBEnv env = GRBEnv();
    env.set(GRB_IntParam_LogToConsole, 0);
    env.set(GRB_IntParam_Threads, 48);
    env.set(GRB_StringParam_LogFile, "solutions.log" );

    //env.set(GRB_IntParam_Presolve, 0);
    env.set(GRB_IntParam_PoolSearchMode, 2);//focus on finding additional solutions 
    env.set(GRB_IntParam_MIPFocus, 3);
    env.set(GRB_IntParam_PoolSolutions, MAX); // try to find 2000000
        
    // Create the model
    GRBModel model = GRBModel(env);

    // Create variables
    vector<GRBVar> s(288);
    for (int i = 0; i < 288; i++)
        s[i] = model.addVar(0, 1, 0, GRB_BINARY);

    for ( int i = 0; i < 80; i++ )
        if ( cube[i] == 0 )
            model.addConstr( s[i + 93] == 0 );
        else
            model.addConstr( s[i + 93] == 1 );

    // key, last three bits
    for ( int i = 80; i < 93; i++ )
        model.addConstr( s[i] == 0 );
    for ( int i = 93 + 80; i < 285; i++ )
        model.addConstr( s[i] == 0 );

    // other bits are free
    // Round function
    vector<GRBVar> works = s;

    for (int r = 0; r < rounds; r++) 
    {
        triviumCore(model, works, 65, 170, 90, 91, 92);
        triviumCore(model, works, 161, 263, 174, 175, 176);
        triviumCore(model, works, 242, 68, 285, 286, 287);
            
        // or the works cannot work
        vector<GRBVar> temp = works;
        for (int i = 0; i < 288; i++) 
            works[(i + 1) % 288] = temp[i];
    }

    // Output constraint
    for ( int i = 0; i < 288; i++ )
        if ( last[i] == 1)
            model.addConstr( works[i] == 1 );
        else
            model.addConstr( works[i] == 0 );
    // 
    GRBLinExpr nk = 0;
    for ( int i = 0; i < 80; i++ )
        nk += s[i];
    model.setObjective( nk, GRB_MINIMIZE );


//    if ( rounds > 550 )
//        model.set(GRB_DoubleParam_TimeLimit, 600.0 );
//=======
    cout << getCurrentSystemTime() << endl;
    f << getCurrentSystemTime() << endl;
    if ( rounds > 600 )
         model.set(GRB_DoubleParam_TimeLimit, 120.0 );
    else if ( rounds > 500 )
         model.set(GRB_DoubleParam_TimeLimit, 180.0 );
    else if ( rounds > 400 )
         model.set(GRB_DoubleParam_TimeLimit, 240.0 );
    else if ( rounds > 300 )
         model.set(GRB_DoubleParam_TimeLimit, 600.0 );

    model.optimize();

    if ( model.get( GRB_IntAttr_Status ) == GRB_TIME_LIMIT )
    {
        cout << "-------------------------------------------------------------- EXPAND" << endl;
        f << "-------------------------------------------------------------- EXPAND" << endl;
        int c = 0;  
        vector<bitset<288>> T;
        int re = 0;
        do 
        {
            T.clear();
            re++;
            SecondBackExpandPolynomial(re, last, T );  
            cout << "T Size: " << T.size() << " RE:" << re <<  endl;
            f << "T Size: " << T.size() << " RE:" << re <<  endl;
        }while ( T.size() <= 16 && (re + 10) < rounds ); 
        int tsize = T.size();

        int count = 0;
	depth++;
        for ( auto it : T )
        {
            cout << c << " out of " << tsize << "| Depth " << depth << endl;
            f << c << " out of " << tsize << "| Depth " << depth << endl;
            c++;
            MidSolutionCounter( rounds - re, cube, it,  counterMap, f );    
        }
	depth --;
    }
    else
    {
        double time = model.get(GRB_DoubleAttr_Runtime );
        cout << "Rounds: " << rounds << "  Time Used: " << time << "sec" << endl;
        f << "Rounds: " << rounds << "  Time Used: " << time << "sec" << endl;
        
        int solCount = model.get(GRB_IntAttr_SolCount);
        cout << "---------------------------------------Raw Solutions: " << solCount << endl;
        f << "----------------------------------------Raw Solutions: " << solCount << endl;

        bitset<285> start;
        for ( int i = 0; i < solCount; i++ )
        {
            model.set(GRB_IntParam_SolutionNumber, i );
            for ( int j = 0; j < 285; j++ ) 
                if ( round( s[j].get( GRB_DoubleAttr_Xn ) ) == 1 )  
                    start[j] = 1;
                else 
                    start[j] = 0;
            counterMap[start]++;
        }
    }
}

int BackExpandPolynomial( int rounds, vector<bitset<288> > & term )
{
    // Create the environ
    GRBEnv env = GRBEnv();
    env.set(GRB_IntParam_LogToConsole, 0);
    env.set(GRB_StringParam_LogFile, "solutions.log" );

    env.set(GRB_IntParam_Presolve, 0);
    env.set(GRB_IntParam_MIPFocus, 1);
    env.set(GRB_IntParam_PoolSearchMode, 1);//focus on finding additional solutions 
    env.set(GRB_IntParam_PoolSolutions, MAX); // try to find 2000000

    GRBModel model = GRBModel(env);

    // Create variables
    vector<GRBVar> s(288);
    for (int i = 0; i < 288; i++)
        s[i] = model.addVar(0, 1, 0, GRB_BINARY);

    // Round function
    //  store the initial state 
    vector<GRBVar> works = s;
    for (int r = 0; r < rounds; r++) 
    {
        triviumCore(model, works, 65, 170, 90, 91, 92);
        triviumCore(model, works, 161, 263, 174, 175, 176);
        triviumCore(model, works, 242, 68, 285, 286, 287);
            
        vector<GRBVar> temp = works;
        for (int i = 0; i < 288; i++) 
            works[(i + 1) % 288] = temp[i];
    }

    // Output constraint
    GRBLinExpr nk = 0;
    for ( int i = 0; i < 288; i++ )
        if ( (i == 65) || (i == 92) || (i == 161) || (i == 176) || (i == 242) || (i == 287))
            nk += works[i];
        else 
            model.addConstr( works[i] == 0);
    model.addConstr( nk == 1 );

    model.update();
    model.optimize();

    map<bitset<288>, int, cmp288> counterMap; 

    if( model.get( GRB_IntAttr_Status ) == GRB_OPTIMAL )
    {
        double time = model.get(GRB_DoubleAttr_Runtime );
        cout << "Time Used: " << time << "sec" << endl;
        
        int solCount = model.get(GRB_IntAttr_SolCount);
        cout << "Raw Solutions: " << solCount << endl;

        bitset<288> start;
        for ( int i = 0; i < solCount; i++ )
        {
            model.set(GRB_IntParam_SolutionNumber, i );

            for ( int j = 0; j < 288; j++ ) 
                if ( round( s[j].get( GRB_DoubleAttr_Xn ) ) == 1 )  
                    start[j] = 1;
                else 
                    start[j] = 0;
            counterMap[start]++;
        }
    }
    else if( model.get( GRB_IntAttr_Status ) == GRB_INFEASIBLE )
    {
        cout << "No terms " << endl;
        exit(-2);
    }
    else
    {
        cout << "Other status " << GRB_IntAttr_Status <<  endl;
        exit(-1);
    }

    for ( auto it : counterMap )
        if ( it.second % 2 == 1 )
            term.push_back( it.first );
    cout << "Exact terms: " << term.size() << endl;
}


int main()
{
    ofstream IV;
/*****************************************************************************
    int MID = 1;
    int ROUND = 841;
    int I[] = { 74, 76, 78 };
    IV.open( string ( "RESULT/EXP_74_76_78_" ) + to_string(ROUND) );
    cout << "This is for trivium-841 with 74 76 78" << endl;
    IV << "This is for trivium-841 with 74 76 78"  << endl;
****************************************************************************/
/*****************************************************************************
    int MID = 200;
    int ROUND = 840;
    int I[] = { 66, 68, 70, 72, 74, 76, 78 };
    IV.open( string ( "RESULT/IV_66_68_andmore_EXP_" ) + to_string( ROUND ) );
    cout << "This is for trivium-840 with I" << endl;
    IV << "This is for trivium-840 with I" << endl;
*****************************************************************************/
/*****************************************************************************/
    int MID = 1;
    int ROUND = 841;
    int I[] = { 74, 76, 78 };
    IV.open( string ( "RESULT/841_74_76_78_AGAIN" ) + to_string(ROUND) );
    cout << "This is for trivium-841 with 74 76 78" << endl;
    IV << "This is for trivium-841 with 74 76 78"  << endl;
/****************************************************************************/
/*****************************************************************************
    int MID = 1;
    int ROUND = 843;
    int I[] = { 72, 74 };
    IV.open( string ( "RESULT/EXP_72_74_" ) + "843" );
    cout << "This is for trivium-843 with 72 74" << endl;
    IV << "This is for trivium-843 with 74 74"  << endl;
****************************************************************************/

/*****************************************************************************
    int MID = 100;
    int ROUND = 840;
    int I[37] = { 0, 2, 4, 6, 8, 10, 12, 15, 17, 19, 21, 23, 25, 27, 30, 32, 34, 
                 36, 38, 40, 42, 45, 47, 49, 51, 53, 55, 57, 60, 62, 64, 66, 68, 
                 70, 72, 75, 79};
    IV.open( string ( "RESULT/IVEXP840_I_" ) + "840" );
    cout << "This is for trivium-840 with I" << endl;
    IV << "This is for trivium-840 with I" << endl;
****************************************************************************/

/*****************************************************************************
    int MID = 250;
    int ROUND = 843;
    int I[] = { 72, 74 };
    IV.open( string ( "RESULT/IV_" ) + to_string(ROUND) + "_" + to_string(I[0]) );
    cout << "This is for trivium-843  EXPAND 250 with 72 74" << endl;
    IV << "This is for trivium-843 with EXPAND 250 72 74" << endl;
*****************************************************************************/
    cout << getCurrentSystemTime() << endl;
    IV << getCurrentSystemTime() << endl;
    auto start = chrono::steady_clock::now();
    vector<bitset<288>> midTerms;
    vector<bitset<80>> initialTerm;
    map< bitset<285>, int, cmp285 > gross, temp;

    bitset<80> cube;
    for ( int i = 0; i < 80; i++ )
        cube[i] = 1;
    for ( auto it : I )
        cube.reset(it); 

    cout << "Cube: " << cube << endl;
    IV << "Cube: " << cube << endl;

    BackExpandPolynomial( MID, midTerms );
    cout << "MidTerms: " << midTerms.size() << endl;
    IV << "MidTerms: " << midTerms.size() << endl;

    int count = 0;
    for ( auto it : midTerms )
    {	
	    depth = 0;
        cout << "Trivium-" << ROUND << "   Term: " << count << "-th" << endl;
        IV << "Trivium-" << ROUND << "   Term: " << count << "-th" << endl;
	    count++;
        //if ( count < 40 )
        //    continue;
        //IV << "Term: " << count++ << "-th" << endl;
        temp.clear();
        MidSolutionCounter(ROUND - MID, cube, it, temp, IV); 
        for ( auto jt : temp )
            gross[jt.first] += jt.second;
    }

    //IV.open( "IV840_EXP" );
    cout << "Raw Terms " << gross.size() << endl;
    IV << "Raw Terms " << gross.size() << endl;

cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
IV << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    for ( auto it: gross )
    {
        cout << it.first << " [" << it.second << "]" << endl;
        IV << it.first << " [" << it.second << "]" << endl;
    }

cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
IV << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

    for ( auto it : gross )
        if ( it.second % 2 == 1 )
        {
            bitset<80> tmp(0);
            for ( int i = 0; i < 80; i++ )
                tmp[i] = it.first[i];
            
            initialTerm.push_back( tmp );
        }

    IV << "ROUND : " << ROUND << endl 
        << "Cube: " << cube << endl
        << "Terms: " << initialTerm.size() << endl;

    cout << "ROUND : " << ROUND << endl 
        << "Cube: " << cube << endl
        << "Terms: " << initialTerm.size() << endl;
    
    ofstream res;
	res.open( "result_841_3" );

    for ( auto it : initialTerm )
    {
        cout << it << endl;
        IV << it << endl;
	res << it << endl;
    }
    res.close();
    auto end = chrono::steady_clock::now();
    auto time = chrono::duration<double> ( end - start );
    cout << getCurrentSystemTime() << endl;
    IV << getCurrentSystemTime() << endl;
    cout << "Time: " << time.count() << " seconds" << endl;
    IV << "Time: " << time.count() << " seconds" << endl;
    IV.close();
}

