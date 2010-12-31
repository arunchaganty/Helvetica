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
    
    CSPSolution& AC3Preprocessor::preprocess( CSPSolution& sol )
    {
        queue<vars> q;
        vars a;
        // adding all tuples  (xi,xj) into queue.
        for(unsigned int i=0; i< sol.problem->constraints.size();i++)
        {
           for(unsigned int j=0;j< sol.problem->constraints[i]->scope.size();j++)
              for(unsigned int k=j+1;k< sol.problem->constraints[i]->scope.size();k++)
              {
                 a.first=j;
                 a.second=k;
                 q.push(a);
                 a.first=k;
                 a.second=j;
                 q.push(a);
              }
        }

        int change=0;
        vars vartuple;
        while(q.size()!=0)
        {
           vartuple = q.front();
           q.pop();
           int idx0=vartuple.first;
           int idx1=vartuple.second;
           for(unsigned int i=0;i<sol.problem->constraints.size();i++)
           {
              int count=0;
              for(unsigned int j=0;j<sol.problem->constraints[i]->scope.size();j++)
                 if((idx0==sol.problem->constraints[i]->scope[j])||(idx1==sol.problem->constraints[i]->scope[j]))
                   count++;
              if(count==2)
              {
                 revise(sol,(*sol.problem->constraints[i]),change);
                 if(change==1)
                 {
                   for(unsigned int l=0;l<sol.problem->variables.size();l++)
                   {
                      if(l!=(unsigned int)idx0)
                      {
                         vartuple.first=l;
                         vartuple.second=idx0;
                         q.push(vartuple);
                      }
                   }
                 }
              }
           }
        }
        
        return sol;
    }
        
    void AC3Preprocessor::revise( CSPSolution& sol, Constraint& cnstr, int change)  //revise function
    {
        int idx0 = cnstr.scope[ 0 ];
        int idx1 = cnstr.scope[ 1 ];
        
        if( cnstr.arity != 2 ) return;
        
        for(unsigned int i=0;i<sol.problem->domains[idx0].size();i++)
        {
           int pair_found=0;
           if(!sol.allowable[idx0][i]==UNSET)continue;
           for(unsigned int j=0;j<sol.problem->domains[idx1].size();j++)
           {
                if(!sol.allowable[idx1][j]==UNSET) continue;
                vector<int> v(sol.problem->variables.size(),UNSET);
                v[idx0]=sol.allowable[idx0][i];
                v[idx1]=sol.allowable[idx1][j];                
                if(cnstr.test(v))
                {
                     pair_found=1;
                     break;
                }
           }
           if(pair_found==0)
           {
             sol.allowable[idx0][i]=UNSET;
             change=1;
           }
        }
    }    
        
/*
        int unset_idx = (sol.assn[ idx0 ] == UNSET) ? idx0 : idx1;

        Domain& dom = sol.problem->domains[ sol.problem->variables[ unset_idx ] ];

        for( unsigned int i = 0; i < dom.size(); i++ )
        {
            if( !sol.allowable[ unset_idx ][ i ] ) continue;

            sol.assn[ unset_idx ] = i;
            disabled[ unset_idx ][ i ] = disabled[ unset_idx ][ i ] | !cnstr.test( sol.assn );
            sol.allowable[ unset_idx ][ i ] = sol.allowable[ unset_idx ][ i ] & cnstr.test( sol.assn );
        }
        sol.assn[ unset_idx ] = UNSET;
        */
};

