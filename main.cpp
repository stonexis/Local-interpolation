#include <iostream>
#include <Python.h>
#include "header.hpp"
using namespace std;

int main(){

    auto array_net_global = new long double[M_const]{};
    auto array_net_sample = new long double[N_const]{};
    long double **array_net_local = new long double*[K_const]{};
    long double **array_fx_local = new long double*[K_const]{};

    long double **array_eval_points = new long double*[K_const]{};
    long double **array_lagrange = new long double*[K_const]{};
    long double **array_fx_large = new long double*[K_const]{};

    for(size_t i = 0; i<K_const;i++){
        array_net_local[i] = new long double[N_const]{};
    }
    for(size_t i = 0; i<K_const;i++){
        array_fx_local[i] = new long double[N_const]{};
    }
    for(size_t i = 0; i<K_const;i++){
        array_eval_points[i] = new long double[(N_const + M_viz_const/K_const)]{};
    }
    for(size_t i = 0; i<K_const;i++){
        array_lagrange[i] = new long double[(N_const + M_viz_const/K_const)]{};
    }
    for(size_t i = 0; i<K_const;i++){
        array_fx_large[i] = new long double[(N_const + M_viz_const/K_const)]{};
    }
    gen_arr_net_global(array_net_global,M_const, a_const, b_const, h_const);
    gen_arr_arr_net_local(array_net_global, array_net_local, K_const, N_const);
    gen_arr_sample(array_net_global, array_net_sample, N_const);
    for(size_t i=0; i<K_const;i++)
        func(array_net_local[i], array_fx_local[i], N_const);
    for(size_t i=0; i<K_const;i++){
            long double a = array_net_local[i][0];
            long double b = array_net_local[i][N_const-1];
            gen_arr_sealing(array_net_local[i], array_eval_points[i], N_const, (N_const + M_viz_const/K_const), a, b);
        }

    for(size_t index_array=0; index_array < K_const; index_array++){ 
        lagrange4(array_net_sample, array_fx_local[index_array], array_eval_points[index_array], array_lagrange[index_array], N_const,(N_const + M_viz_const/K_const), index_array, h_const);
    }
    
    for(size_t i=0; i<K_const;i++)
        func(array_eval_points[i], array_fx_large[i], N_const + M_viz_const/K_const);
/*
    for(size_t i=0; i<K_const;i++)
        for(size_t j=0; j<N_const + M_viz_const/K_const;j++)
            cout << array_fx_large[i][j] << " " << array_eval_points[i][j] <<endl;
    for(size_t i=0; i<K_const;i++)
        for(size_t j=0; j<N_const;j++)
            cout <<" |" << array_net_local[i][j] << " |"  <<endl;
*/          
    callPythonScript2(array_net_local, array_fx_local, array_eval_points, array_lagrange, array_fx_large, K_const, N_const, M_viz_const);
        
    return 0;
}