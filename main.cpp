#include <iostream>
#include "header.hpp"

using namespace std;

int main(){
    //Строим сетку
    auto* grid_h = gen_uniform_grid(Task_const::H, Task_const::M, Task_const::A, Task_const::B);
    auto** grid_h_2d = gen_2d_arr_uniform(grid_h, Task_const::H, Task_const::N, Task_const::K, Task_const::A, Task_const::B);
    auto** func_h_2d = gen_func_2d_arr(grid_h_2d, Task_const::N, Task_const::K);
    //Строим интерполяцию
    std::size_t count_nodes_M_viz;
    auto** grid_M_viz_2d = gen_2d_uniform_between_nodes(true, grid_h_2d, count_nodes_M_viz, Task_const::STEP_M_viz, Task_const::N, Task_const::K);
    auto** lagrange_2d = gen_lagrange_2d(grid_h_2d, func_h_2d, grid_M_viz_2d, Task_const::K, Task_const::N, count_nodes_M_viz);

    auto** func_M_viz_2d = gen_func_2d_arr(grid_M_viz_2d, count_nodes_M_viz, Task_const::K);
    
    write_data_to_file(grid_h_2d, func_h_2d, grid_M_viz_2d, func_M_viz_2d, lagrange_2d, count_nodes_M_viz, Task_const::N, Task_const::K);

    // Готовим сетку h/100 для вычисления погрешностей
    std::size_t count_h_100_points_in_ever_elem; //Количество точек с шагом h/100 на каждом элементе
    auto** array_2d_h_100_points = gen_2d_uniform_between_nodes(false, grid_h_2d, count_h_100_points_in_ever_elem, Task_const::STEP_H_100, Task_const::N, Task_const::K); // Создаем сетку с шагом h_100
    auto** array_func_in_h_100_points = gen_func_2d_arr(array_2d_h_100_points, count_h_100_points_in_ever_elem, Task_const::K);
    auto** arr_approx_in_h_100_points = gen_lagrange_2d(grid_h_2d, func_h_2d, array_2d_h_100_points, Task_const::K, Task_const::N, count_h_100_points_in_ever_elem); // Вычисляем интерполяцию в сетке h/100
    
    //Вычисление погрешностей
    auto errors_h_100_points = calculate_errors(array_func_in_h_100_points, arr_approx_in_h_100_points, count_h_100_points_in_ever_elem, Task_const::K); // Вычисляем погрешности в точках h/100
    print_error_table(errors_h_100_points);

    system("python plotter.py");

    delete[] grid_h;
    delete_2d_array(grid_h_2d, Task_const::K);
    delete_2d_array(func_h_2d, Task_const::K);
    delete_2d_array(lagrange_2d, Task_const::K);
    delete_2d_array(grid_M_viz_2d, Task_const::K);
    delete_2d_array(func_M_viz_2d, Task_const::K);
    delete_2d_array(array_2d_h_100_points, Task_const::K);
    delete_2d_array(array_func_in_h_100_points, Task_const::K);
    delete_2d_array(arr_approx_in_h_100_points, Task_const::K);

    return 0;
}