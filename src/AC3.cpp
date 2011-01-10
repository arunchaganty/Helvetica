/**
* @file AC1.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-05-08
* Helvetica - CSP Workbench
* AC3 implementation
*/

#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <typeinfo>
#include <queue>
#include <string>

#include "Helvetica.h"
#include "CSP.h"
#include "CSPSolver.h"
#include "AC3.h"

using namespace std;

namespace Helvetica
{
    typedef pair<int, int> vars;
    int change=0;    

    bool bv_val( vector<bool> v, int i )
    {
        return v[ i ];
    }
    void bv_print( vector<bool> v )
    {
        for( int i = 0; i < v.size(); i++ )
        {
            printf( "%d", (v[i])?1:0 );
        }
        printf( "\n" );
    }
    
    CSPSolution& AC3Preprocessor::preprocess( CSPSolution& sol )
    {
        printf("ac-3 started\n");

        queue<vars> q;
        vars a;

        printf("adding all tuples to queue\n");
        for( unsigned int i = 0; i < sol.problem->variables.size(); i++ )
        {
            for( unsigned int j = i+1; j < sol.problem->variables.size(); j++ )
            {
                vector<Constraint*>::iterator cnstr;
                for( cnstr = sol.problem->constraints.begin(); cnstr != sol.problem->constraints.end(); cnstr++ )
                {
                    unsigned int idx0 = (*cnstr)->scope[0];
                    unsigned int idx1 = (*cnstr)->scope[1];
                    if( (*cnstr)->scope.size() == 2 &&
                          ( ( idx0 == i && idx1 == j ) || ( idx1 == i && idx0 == j ) ) )
                    {
                        q.push( make_pair( i, j ) );
                        q.push( make_pair( j, i ) );
                    }
                }
            }
        }

        while(q.size()!=0)
        {
           vars vartuple = q.front(); q.pop();
           unsigned int idx0 = vartuple.first;

           if( revise( sol, vartuple ) )
           {
                for( unsigned int i = 0; i < sol.problem->variables.size(); i++ )
                {
                    if( i != idx0 ) q.push( make_pair( i, idx0 ) );
                }
           }
        }

        printf("ac-3 done\n");
        return sol;
    }

    void AC3Preprocessor::print( CSPSolution& sol )    
    {
       for(unsigned int i=0;i<sol.allowable.size();i++)
       {
          int d=sol.problem->variables[i];
          printf("variable %d no. of values in domain %d: ",i, sol.problem->domains[d].size());
          for(unsigned int j=0;j<sol.allowable[i].size();j++)
          {
             if(sol.allowable[i][j])
               printf("%d ",j);
          }
          printf("\n");
        }
//        printf("printing done\n");
    }

    bool AC3Preprocessor::revise( CSPSolution& sol, vars vartuple)  //revise function
    {
        bool changed = false;

        vector<Constraint*>::iterator cnstr;
        for( cnstr = sol.problem->constraints.begin(); cnstr != sol.problem->constraints.end(); cnstr++ )
        {
            int v0 = vartuple.first;
            int v1 = vartuple.second;
            if( (*cnstr)->scope.size() != 2 ) continue;
            int idx0 = (*cnstr)->scope[ 0 ];
            int idx1 = (*cnstr)->scope[ 1 ];

            if( !( ( idx0 == v0 && idx1 == v1 ) ||
                    ( idx1 == v0 && idx0 == v1) ) ) continue;

            int d0 = sol.problem->variables[ v0 ];
            int d1 = sol.problem->variables[ v1 ];        
            
            for(unsigned int i=0; i<sol.problem->domains[d0].size(); i++)
            {  
               bool pair_found = false;
               if(!sol.allowable[v0][i]) continue;

               for( unsigned int j=0; j<sol.problem->domains[d1].size() && !pair_found; j++)
               {
                    if(!sol.allowable[v1][j]) continue;

                    vector<int> v(sol.problem->variables.size(),UNSET);
                    v[v0]=i;
                    v[v1]=j;                

                    pair_found = (*cnstr)->test(v);
               }

               if(!pair_found)
               {
                 sol.allowable[v0][i] = false;
                 changed = true;
               }
            }
        }

        return changed;
    }    
        
};

