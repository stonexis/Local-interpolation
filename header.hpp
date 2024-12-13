#pragma once
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <map>
#include <iomanip>

namespace Task_const {
    inline constexpr long double A = -15.6;
    inline constexpr long double B = 3.562;
    inline constexpr std::size_t K = 10; ///количество интервалов разбиения [a,b]
    inline constexpr std::size_t N = 4; ///Количество узлов конечного элемента (Степень полинома на 1 меньше)
    inline constexpr std::size_t M_viz = 1000; ///Количество точек равномерной сетки для отображения графика

    //Неизменяемые параметры
    inline constexpr std::size_t M = K * (N - 1) + 1; ///Общее количество узлов сетки на [a,b], K*(N-1) + 1, в каждом элементе теряем 1 узел из за перекрытия, кроме первого элемента
    inline const long double H = std::abs(B-A)/(M-1); ///Шаг равномерной сетки
    inline const long double STEP_H_100 = H / 100; /// Шаг сетки h/100
    inline const long double STEP_M_viz = std::abs(B-A)/(M_viz - 1); /// Шаг сетки M_viz
}

/**
 * @brief Функция для генерации массива значений заданной функции на отрезке
 * @tparam T Тип данных (float, double, long double).
 * @param array_x Массив, для которого генерируются значения функции (Массив случайных точек).
 * @param length Длина массива точек.
 * @return T* Указатель на массив значений функции.
 * @note Имеет размер length.
 */
template <typename T>
T* gen_func_arr(const T *array_x, const std::size_t length) {
    if (array_x == nullptr) throw std::invalid_argument("array_x is null");
    if (length <= 0) throw std::invalid_argument("Invalid length values");

    T* arr_func = new T[length]{};
    for (std::size_t i = 0; i < length; i++)
        arr_func[i] = sin(array_x[i]); //заданная функция
    
    return arr_func;
}
/**
 * @brief Функция для создания двумерного массива значений.
 * @tparam T Тип данных (float, double, long double).
 * @param array_2d_x Двумерный массив, для которого вычисляются значения функции.
 * @param length_internal Количетсво точек на каждом подотрезке внутри двумерного массива.
 * @param length_external Количество подтрезков (По умолчанию Task_const::K).
 * @return Указатель на двумерный массив значений функции. Первая размерность - индекс КЭ, вторая размерность - индекс значения функции в точке.
 * @note Внешний массив имеет размер length_external (По умолчанию Task_const::K). Внутренний — length_internal.
 */
template <typename T>
T** gen_func_2d_arr(T **array_2d_x, const std::size_t length_internal, const std::size_t length_external){
    if (array_2d_x == nullptr) throw std::invalid_argument("array_2d_x is null");
    if (length_external <= 0) throw std::invalid_argument("Invalid length_external values");

    T** arr_2d_func = new T*[length_external]{}; 
    for (std::size_t i = 0; i < length_external; i++)
        arr_2d_func[i] = gen_func_arr(array_2d_x[i], length_internal);
    return arr_2d_func;
}
/**
 * @brief Функция для генерации равномерной сетки на отрезке [a,b]
 * @tparam T Тип данных (float, double, long double).
 * @param step - Шаг равномерной сетки. (По умолчанию Task_conts::H)
 * @param count_nodes - Количество узлов сетки. (По умолчанию Task_const::M)
 * @param a Начало отрезка. (По умолчанию Task_const::A)
 * @param b Конец отрезка. (По умолчанию Task_const::B)
 * @return T* Указатель на массив равномерной сетки.
 * @note Массив имеет размер count_nodes. (По умолчанию Task_const::M)
 */
template <typename T>
const T* gen_uniform_grid(const T step, const std::size_t count_nodes, const T a, const T b) {
    if (count_nodes <= 0) throw std::invalid_argument("Invalid count_nodes values");
    if ((b - a) < std::numeric_limits<T>::epsilon()) throw std::invalid_argument("Invalid a, b values");
    T* array = new T[count_nodes]{}; 
    for (std::size_t i = 0; i < count_nodes; i++) 
        array[i] = a + step * i; // Заполняем значения, включая последний узел, равный b
    if (array[count_nodes - 1] != b)
        array[count_nodes - 1] = b;
    return array;
}

/**
 * @brief Функция для разбиения глобальной сетки на локальные подсетки, с учетом перекрытия в 1 элемент
 * @tparam T Тип данных (float, double, long double).
 * @param array_global Базовый массив глобальной равномерной сетки.
 * @param step - Шаг равномерной сетки. (По умолчанию Task_conts::H)
 * @param length_internal - Количество узлов на каждом КЭ. (По умолчанию Task_const::N)
 * @param length_external - Количество подотрезков. (По умолчанию Task_const::K)
 * @param a Начало отрезка. (По умолчанию Task_const::A)
 * @param b Конец отрезка. (По умолчанию Task_const::B)
 * @return T** Указатель на двумерный массив локальной равномерной сетки, первая размерность - индекс КЭ, вторая размерность - локальный индекс узла КЭ.
 * @note Внешний массив имеет размер length_external (По умолчанию Task_const::K). Внутренний — length_internal (По умолчанию Task_const::N).
 */
template <typename T>
T** gen_2d_arr_uniform(
                const T *array_global, 
                const T step, 
                const std::size_t length_internal, 
                const std::size_t length_external, 
                const T a, const T b
                ){
    if (array_global == nullptr) throw std::invalid_argument("array_global is null");
    if (length_external <= 0 || length_internal <= 0) throw std::invalid_argument("Invalid length_external values");

    T **array_2d_uniform = new T*[length_external]{}; //Выделяем память под массив массивов

    for (std::size_t i = 0; i < length_external; i++) {
        array_2d_uniform[i] = new T[length_internal]{}; //Выделяем память под каждый массив в массиве (Точки локального отрезка)
        for (std::size_t j = 0; j < length_internal; j++) {
            if (i > 0 && j == 0) //Если это первый элемент текущего блока, и это не первый блок, то берем последний элемент предыдущего блока
                array_2d_uniform[i][j] = array_2d_uniform[i-1][length_internal-1]; // последний элемент предыдущего блока
               else 												// иначе берем элемент из одномерного массива
                array_2d_uniform[i][j] = array_global[i * (length_internal - 1) + j];//чтобы смещение между блоками было на 1 элемент меньше, чем размер блока, чтобы обеспечить перекрытие  
            }
        }
    return array_2d_uniform;
}

/**
 * @brief Функция для генерации более мелкой, равномерной сетки, внутри существующей
 * @tparam T Тип данных (float, double, long double).
 * @param content_orig_mesh Должен ли новый массив содержать в себе исходную сетку? (True/False)
 * @param arr_old Интервал, внутри которого строится мелкая сетка
 * @param length_old Размер массива, внутри которого строится сетка 
 * @param[out] length_out Длинна нового массива (Заполняемый параметр)
 * @param step Шаг новой сетки
 * @return T* Указатель на массив новой равномерной сетки.
 * @note Массив имеет размер length_new. (Массив не содержит значения исходной сетки, внутри которой строился)
 */
template <typename T>
T* gen_uniform_arr_in_local(bool content_orig_mesh, const T* arr_old, const std::size_t length_old, std::size_t& length_out, const T step) {
    if (arr_old == nullptr) throw std::invalid_argument("array_old is null");
    if (length_old < 2) throw std::invalid_argument("length_old must be at least 2");
    if (step < std::numeric_limits<T>::epsilon()) throw std::invalid_argument("Incorrect step");
    //if (step > std::abs(arr_old[length_old - 1] - arr_old[0])) throw std::invalid_argument("Incorrect step"); // Опциональная проверка(Если выключена, то добавление точек при выполнении условия не происходит)
    
    T interval_length = std::abs(arr_old[length_old - 1] - arr_old[0]); // Длина интервала
    //Округление вверх, поскольку нужно захватить весь интервал 
    std::size_t count_new_nodes = static_cast<std::size_t>(std::ceil(interval_length / step)) + 1; // Рассчитываем количество узлов с фиксированным шагом
    T* arr_new = nullptr;
    if (content_orig_mesh == false){
        arr_new = new T[count_new_nodes]{};
        T a = arr_old[0]; //Начало отсчета
        for (std::size_t i = 0; i < count_new_nodes; i++)
            arr_new[i] = a + step * i; // Заполняем массив равномерными узлами
        length_out = count_new_nodes;
    }
    else {
        arr_new = new T[length_old + count_new_nodes - 1]{}; // Учёт старых узлов и новых точек
        std::copy(arr_old, arr_old + length_old, arr_new); // Копируем старую сетку
        // Добавляем новые точки равномерно, с учётом step
        T a = arr_old[0];
        std::size_t insert_index = length_old; // Индекс для вставки новых точек
        for (std::size_t i = 1; i < count_new_nodes; i++) {
            T value = a + step * i;
            auto position = std::lower_bound(arr_new, arr_new + insert_index, value); // Найти позицию для вставки
            std::rotate(position, arr_new + insert_index, arr_new + insert_index + 1); // Сдвинуть элементы
            *position = value; // Вставить новую точку
            insert_index++;
        }
        length_out = count_new_nodes + length_old - 2; // Отнимаем 2 поскольку строили новую сетку, на основе старой, в которой есть 1я точка и последняя точка
    }
    return arr_new;
}

/**
 * @brief Функция для генерации более мелкой, равномерной сетки, внутри существующей
 * @tparam T Тип данных (float, double, long double).
 * @param content_orig_mesh Должен ли новый массив содержать в себе исходную сетку? (True/False)
 * @param array_nodes Двумерный массив узлов.
 * @param[out] length_new Длина нового массива (Заполняемый параметр)
 * @param step Шаг новой сетки
 * @param length_internal Количество узлов старой сетки на каждом КЭ. (По умолчанию Task_const::N)
 * @param length_external - Количество подотрезков. (По умолчанию Task_const::K)
 * @return T** Указатель на двумерный массив новой равномерной сетки.
 * @note Внешний массив имеет размер length_external (По умолчанию Task_const::K). Внутренний — length_new.
 *  (Массив не содержит значения исходной сетки, внутри которой строился)
 */
template <typename T>
T** gen_2d_uniform_between_nodes(
                            bool content_orig_mesh, 
                            T **array_nodes, 
                            std::size_t& length_new, 
                            const T step, 
                            const std::size_t length_internal, 
                            const std::size_t length_external
                            ){
    if (array_nodes == nullptr) throw std::invalid_argument("Input array cannot be null");
    if (step < std::numeric_limits<T>::epsilon()) throw std::invalid_argument("Incorrect step");
    if (length_internal <= 0 || length_external <= 0) throw std::invalid_argument("Size arrays cannot be 0");

    T** array_2d_uniform = new T*[length_external]{};
    for(std::size_t k = 0; k < length_external; k++)
            array_2d_uniform[k] = gen_uniform_arr_in_local(content_orig_mesh, array_nodes[k], length_internal, length_new, step); // Заполняем внутренние массивы
    return array_2d_uniform;
}
/**
 * @brief Функция для вычисления и кеширования знаменателя.
 * @tparam T Тип данных (float, double, long double).
 * @param array_nodes_x Массив узлов конечного элемента.
 * @param count_nodes_points Количество узлов (По умолчанию Task_const::N)
 * @return T* Указатель на массив значений знаменателя.
 * @note Массив имеет размер count_nodes_points.
 */
template <typename T>
T* denominator_fun(const T *array_nodes_x, const std::size_t count_nodes_points) {
    T* denominator = new T[count_nodes_points]{};
    for(std::size_t i = 0; i < count_nodes_points; i++){
        T product = 1;
        for(std::size_t j = 0; j < count_nodes_points; j++){
             if (i != j){
                // Проверка на совпадение узлов
                if (std::abs(array_nodes_x[i] - array_nodes_x[j]) < std::numeric_limits<T>::epsilon()) {
                    //std::cout << array_nodes_x[i] << " " << array_nodes_x[j] << "\n";
                    delete[] denominator; // Очищаем выделенную память перед исключением
                    throw std::runtime_error("Duplicate nodes detected in array_nodes_x");
                }
                product *= array_nodes_x[i] - array_nodes_x[j];
             }
        }
        denominator[i] = product;
    }
    return denominator;
}

template <typename T>
T* lagrange(
    const T *array_net_sample, 
    const T *array_fx, 
    const T *array_eval_points, 
    std::size_t count_nodes, 
    std::size_t count_eval_points, 
    std::size_t index_array, 
    const T step
    ){
    static T* denominator = nullptr;
    // Проверяем, был ли уже вычислен знаменатель
    if (!denominator) 
        denominator = denominator_fun(array_net_sample, count_nodes); // Вычисляем знаменатель один раз
    T* array_lagrange = new T[count_eval_points]{};
	//Цикл по точкам, в которых хотим вычислить значения многочлена											 
	for (size_t i = 0; i < count_eval_points; i++) {
		T eval_point = array_eval_points[i];
		T sum = 0;
		//Цикл по узлам интерполяции
		for (size_t j = 0; j < count_nodes; j++) {
			T product = 1;
			//Цикл для вычисления базисного многочлена Лагранжа L_j(x)
			for (size_t k = 0; k < count_nodes; k++) {
				if (k != j) {
					product *= eval_point - (array_net_sample[k] + (count_nodes-1)*step*index_array); //чтобы попасть на необходимы эл-т блока, необходимо семпл * длинну блока * длину шага
				} // 1 отнимается из за того, что начало следующего блока = конец предыдущего
			}    
			sum += (array_fx[j] * product) / denominator[j]; //Добавляем вклад j-го базисного многочлена в результат
		}
		array_lagrange[i] = sum;
	}
	return array_lagrange;
}

template <typename T>
T** gen_lagrange_2d(
        T** array_nodes, 
        T** array_func_in_nodes, 
        T** eval_points, 
        const std::size_t length_external, 
        const std::size_t length_nodes_internal, 
        const std::size_t length_points_internal
        ){
    if (array_nodes == nullptr || array_func_in_nodes == nullptr || eval_points == nullptr) throw std::invalid_argument("arrays is null");
    if (length_external < 2 || length_nodes_internal < 2 || length_points_internal < 2) throw std::invalid_argument("lengths must be at least 2");        
    
    T** array_lagrange_2d = new T*[length_external]{};

    for(std::size_t i = 0; i < length_external; i++)
        array_lagrange_2d[i] = lagrange(array_nodes[0], array_func_in_nodes[i], eval_points[i], length_nodes_internal, length_points_internal, i, Task_const::H);

    return array_lagrange_2d;
}

/**
 * @brief Функция для записи двумерного массива в файл в формате json. 
 * @tparam T Тип данных (float, double, long double).
 * @param out Указатель на поток ввода.
 * @param array Записываемый двумерный массив.
 * @param name_array Имя массива.
 * @param length_internal Внутренняя размерность массива.
 * @param length_external Внешняя размерность массива (По умолчанию - Task_const::K)
 * @note {"name1" : [[value1, value2, ], [...], ...], "name2" : [[value1, value2, ], [...], ...], ...}
 */
template <typename T>
void write_to_file_arr_2d(std::ofstream &out, T **array, const std::string &name_array, const std::size_t length_internal, const std::size_t length_external){
    out << "\"" << name_array << "\"" << ": ["; // Начало массива в формате JSON
    for (std::size_t i = 0; i < length_external; ++i) {
        out << "[";  // Начало строки
        for (std::size_t j = 0; j < length_internal; ++j) {
            out << array[i][j];
            if (j != length_internal - 1) 
                out << ", ";  // Разделяем значения в строке
        }
        out << "]";  // Конец строки
        if (i != length_external - 1) 
            out << ",\n";  // Разделяем строки
    }
    out << "]";  // Конец массива
}
/**
 * @brief Функция для записи данных в файл, для последующего построения функций при помощи Python
 * @tparam T Тип данных (float, double, long double).
 * @param array_nodes Двумерный массив узлов аппроксимации.
 * @param array_f_nodes Двумерный массив значений функции в узлах.
 * @param array_x Двумерный массив точек, в которых будут строится графики сравнения функций.
 * @param arr_fx Двумерный массив значений функции.
 * @param arr_approximation Двумерный массив значений функции аппроксимации.
 * @param length_internal_values Количество точек, в которых будут строится графики сравнения функций.
 * @param length_internal_nodes Количество узлов аппроксимации внутри каждого элемента (По умолчанию Task_const::N).
 * @param length_external Внешняя размерность массива. Количество КЭ (По умолчанию Task_const::K)
 */
template <typename T>
void write_data_to_file(
            T **array_nodes,
            T **array_f_nodes, 
            T **array_x, 
            T **array_fx, 
            T **array_approximation, 
            const std::size_t length_internal_values, 
            const std::size_t length_internal_nodes,
            const std::size_t length_external
            ) {
    std::ofstream out;
    out.open("data.json");
    if (!out.is_open()) {
        throw std::runtime_error("Can't open file");
    } else {
        out << "{\n";
        write_to_file_arr_2d(out, array_nodes, "array_nodes", length_internal_nodes, length_external);
        out << ",\n";
        write_to_file_arr_2d(out, array_f_nodes, "array_f_nodes", length_internal_nodes, length_external);
        out << ",\n";
        write_to_file_arr_2d(out, array_x, "array_x", length_internal_values, length_external);
        out << ",\n";
        write_to_file_arr_2d(out, array_fx, "array_fx", length_internal_values, length_external);
        out << ",\n";
        write_to_file_arr_2d(out, array_approximation, "array_approximation", length_internal_values, length_external);
        out << "\n}";
        out.close();
    }
}

/**
 * @brief Функция для вычисления абсолютных погрешностей. Вычисляет абсолютные погрешности для норм: L1 = sum{abs(f(x_i) - l(x_i))}, L2 = sqrt(sum{[f(x_i) - l(x_i)]^2}), L_inf = max(f(x_i) - l(x_i))
 * @tparam T Тип данных (float, double, long double).
 * @param points_fx Двумерный массив значений заданной функции.
 * @param points_lx Двумерный массив значений приближающей функции.
 * @param length_internal Количество внутренних точек.
 * @param length_external Количество КЭ (По умолчанию Task_const::K)
 * @return Пара pair(map(absolute_norms),map(relative_norms)), где в каждой map содеражтся соответсвующие значения L_1, L_2, L_inf
 */
template <typename T>
std::pair<std::map<std::string, T>, std::map<std::string, T>> calculate_errors(
                                                                            T** points_fx,
                                                                            T** points_lx,
                                                                            const std::size_t length_internal,
                                                                            const std::size_t length_external
                                                                            ) {
    if (points_fx == nullptr || points_lx == nullptr) 
        throw std::invalid_argument("Input arrays cannot be null");
    if (length_internal == 0 || length_external == 0) 
        throw std::invalid_argument("Array sizes must be greater than 0");

    std::map<std::string, T> absolute_norms = {{"L_1", 0.0}, {"L_2", 0.0}, {"L_inf", 0.0}};
    std::map<std::string, T> relative_norms = {{"L_1", 0.0}, {"L_2", 0.0}, {"L_inf", 0.0}};

    T sum_abs = 0.0, sum_2_abs = 0.0, max_abs = 0.0;
    T sum_rel = 0.0, sum_2_rel = 0.0, max_rel = 0.0;

    for (std::size_t k = 0; k < length_external; k++) {
        std::size_t start_idx = (k == 0) ? 0 : 1; // Пропускаем первый узел на стыках
        for (std::size_t i = start_idx; i < length_internal; i++) {
            T abs_error = std::abs(points_fx[k][i] - points_lx[k][i]);
            sum_abs += abs_error;
            sum_2_abs += abs_error * abs_error;
            max_abs = std::max(max_abs, abs_error);

            if (std::abs(points_fx[k][i]) > std::numeric_limits<T>::epsilon() ) {
                T rel_error = abs_error / std::abs(points_fx[k][i]);
                sum_rel += rel_error;
                sum_2_rel += rel_error * rel_error;
                max_rel = std::max(max_rel, rel_error);
            }
        }
    }

    absolute_norms["L_1"] = sum_abs;
    absolute_norms["L_2"] = std::sqrt(sum_2_abs);
    absolute_norms["L_inf"] = max_abs;

    relative_norms["L_1"] = sum_rel;
    relative_norms["L_2"] = std::sqrt(sum_2_rel);
    relative_norms["L_inf"] = max_rel;

    return std::make_pair(absolute_norms, relative_norms);
}

/**
 * @brief Функция вывода значений абсолютной и относительной погрешностей в формате таблицы
 * @tparam T Тип данных (float, double, long double).
 * @param errors_h_100 Пара двух map(absolute, relative), содержащих значения ошибок в точках с шагом h/100
 */
template <typename T>
void print_error_table(const std::pair<std::map<std::string, T>, std::map<std::string, T>> errors_h_100){
    // Функция для вывода одной строчки таблицы
    auto print_row = [](const std::string& label, const std::map<std::string, T>& data) { // Первый аргумент лямбда функции - названии выводимой строки, второй - данные
            std::cout << std::left // Выравнивание текста влево
                      << std::setw(18) << label // Устанавливает фиксированную ширину вывода
                      << std::scientific << std::setprecision(6) //  Устанавливает точность
                      << std::setw(15) << data.at("L_1")
                      << std::setw(15) << data.at("L_2")
                      << std::setw(15) << data.at("L_inf") << "\n" ;
        };
    // Заголовок таблицы
    std::cout << std::left
              << std::setw(18) << " "
              << std::setw(15) << "L_1"
              << std::setw(15) << "L_2"
              << std::setw(15) << "L_inf" << "\n";

    std::cout << std::string(61, '-') << "\n";

    // Заголовок для Absolute error
    std::cout << "Absolute error" << "\n";
    print_row("h/100 points", errors_h_100.first);

    std::cout << std::string(61, '-') << "\n";

    // Заголовок для Relative error
    std::cout << "Relative error" << "\n";
    print_row("h/100 points", errors_h_100.second);
}

/**
 * @brief Функция для очистки памяти двумерного массива.
 * @tparam T Тип данных (float, double, long double).
 * @param array Двумерный массив для очистки.
 * @param length_external Внешняя размерность массива.(По умолчанию Task_const::K)
 */
template <typename T>
void delete_2d_array(T**& array, std::size_t length_external){
    if (array == nullptr) return;
    if (length_external == 0) return;
    for (std::size_t i = 0; i < length_external; i++)
        delete[] array[i];
    delete[] array;
    array = nullptr;
}