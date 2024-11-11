#pragma once
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <Python.h>

const long double a_const = -50.5;
const long double b_const = 50.5;

const size_t K_const = 2; //количество интервалов разбиения [a,b]
const size_t N_const = 4;//кол во точек, чере
const size_t M_const = K_const*N_const;//общее количество узлов интерполяции
const size_t M_viz_const = 300;//Количество узлов равномерной сетки для отображения графика
const long double h_const = (b_const-a_const)/(M_const-1);//шаг равномерной сетки 

template <typename T>
void gen_arr_net_global(T *array, size_t M_const, T a_const, T b_const, T h_const) {
	for (size_t i = 0; i < M_const-1; i++)
		array[i] = a_const + h_const*i; //заполняем массив значениями координат сетки
    array[M_const-1]=b_const;
}

template <typename T>
void gen_arr_arr_net_local(T *array_net_global, T **array_net_local, size_t K_const, size_t N_const) {
    for (size_t i = 0; i < K_const; i++) {
        for (size_t j = 0; j < N_const; j++) {
            // Если это первый элемент текущего блока, и это не первый блок,
            // то берем последний элемент предыдущего блока
            if (i > 0 && j == 0) {
                array_net_local[i][j] = array_net_local[i-1][N_const-1]; // последний элемент предыдущего блока
            } else {
                // иначе берем элемент из одномерного массива
                array_net_local[i][j] = array_net_global[i * (N_const-1) + j];//чтобы смещение между блоками было на 1 элемент меньше, чем размер блока, чтобы обеспечить перекрытие
            }
        }
    }
}

template <typename T>
void gen_arr_sample(T *array_net_global, T *array_net_sample, size_t N_const) {//создание экземпляра локальных координат 
        for (size_t j = 0; j < N_const; j++) //все координаты выражаются через кординаты этого экземпляра умножением на индекс массива в котором они находятся
            array_net_sample[j] = array_net_global[j];
}

template <typename T>
void func(const T *array_x, T *arr_fx, size_t Length) {
	for (size_t i = 0; i < Length; i++)
		arr_fx[i] = pow(array_x[i], 4); //заданная функция
}

template <typename T>
void gen_arr_sealing(T *array_old, T *array_new, size_t Length_old, size_t Length_new, T a, T b){
	T h_viz = (b - a) / (Length_new - Length_old + 1);
	for(size_t i = 0; i <Length_old; i++){ //заполняем уплотненный массив значениями старого массива
		array_new[i] = array_old[i];
	}
	for(size_t i = Length_old, j = 1; i < Length_new; i++, j++){
		T value = a + h_viz*j;
		//находим позицию для в ставки при помощи бинарного поиска
		auto position = std::lower_bound(array_new, array_new + i, value);//находит наименьший элемент, больший данного
		std::rotate(position, array_new + i, array_new + i + 1);//циклически сдвигаем от позиции posit все элементы заполненного массива на 1
		*position = value;//присваиваем по найденному указателю вычисленное значение
	}

}

template <typename T>
void denominatorFun(const T *array_nodes_x, T *denominator, size_t N_const) {
	for(size_t i = 0; i < N_const; i++){
		T product = 1;
		for(size_t j = 0; j < N_const; j++){
			 if (i != j) 
            	product *= array_nodes_x[i] - array_nodes_x[j];
		}
		denominator[i] = product;
	}
}

template <typename T>
void lagrange4(const T *array_net_sample, const T *array_fx, const T *array_eval_points, T *array_lagrange, size_t N_const, size_t M_viz_local_const, size_t index_array, T h_const) {
    static T* denominator = nullptr;
    // Проверяем, был ли уже вычислен знаменатель
    if (!denominator) {
        denominator = new T[N_const];
        denominatorFun(array_net_sample, denominator, N_const); // Вычисляем знаменатель один раз
    }
	//Цикл по точкам, в которых хотим вычислить значения многочлена											 
	for (size_t i = 0; i < M_viz_local_const; i++) {
		T eval_point = array_eval_points[i];
		T sum = 0;
		//Цикл по узлам интерполяции
		for (size_t j = 0; j < N_const; j++) {
			T product = 1;
			//Цикл для вычисления базисного многочлена Лагранжа L_j(x)
			for (size_t k = 0; k < N_const; k++) {
				if (k != j) {
					product *= eval_point - (array_net_sample[k] + (N_const-1)*h_const*index_array); //чтобы попасть на необходимы эл-т блока, необходимо семпл * длинну блока * длину шага
				} // 1 отнимается из за того, что начало следующего блока = конец предыдущего
			}    
			sum += (array_fx[j] * product) / denominator[j]; //Добавляем вклад j-го базисного многочлена в результат
		}
		array_lagrange[i] = sum;
	}
	//delete[] denominator;
}

template <typename T>
void writeToFileArr2D(std::ofstream &out, T **array, size_t rows, size_t cols, const std::string &nameArray) {
    out << "\"" << nameArray << "\"" << ": ["; // Начало массива в формате JSON
    for (size_t i = 0; i < rows; ++i) {
        out << "[";  // Начало строки
        for (size_t j = 0; j < cols; ++j) {
            out << array[i][j];
            if (j != cols - 1) 
				out << ", ";  // Разделяем значения в строке
        }
        out << "]";  // Конец строки
        if (i != rows - 1) 
			out << ",\n";  // Разделяем строки
    }
    out << "]";  // Конец массива
}

template <typename T>
void writeDataToFile2D(T **array_net_local, T **array_fx_local, T **array_eval_points, T **array_lagrange, T **array_fx_large, size_t K_const, size_t N_const, size_t M_viz_const) {
    std::ofstream out;
    out.open("data.json");
    if (!out.is_open()) {
        throw std::runtime_error("Can't open file");
    } else {
        out << "{\n";
        writeToFileArr2D(out, array_net_local, K_const, N_const, "array_net_local");
        out << ",\n";
        writeToFileArr2D(out, array_fx_local, K_const, N_const, "array_fx_local");
        out << ",\n";
        writeToFileArr2D(out, array_eval_points, K_const, N_const + M_viz_const / K_const, "array_eval_points");
        out << ",\n";
        writeToFileArr2D(out, array_lagrange, K_const, N_const + M_viz_const / K_const, "array_lagrange");
        out << ",\n";
        writeToFileArr2D(out, array_fx_large, K_const, N_const + M_viz_const / K_const, "array_fx_large");
        out << "\n}";
        out.close();
    }
}

template <typename T>
void callPythonScript2(T **array_net_local, T **array_fx_local, T **array_eval_points, T **array_lagrange, T **array_fx_large, size_t K_const, size_t N_const, size_t M_viz_const) {
    writeDataToFile2D(array_net_local, array_fx_local, array_eval_points, array_lagrange, array_fx_large, K_const, N_const, M_viz_const);
    Py_Initialize(); //Инициализируем интерпретатор Python

    PyObject* sysPath = PySys_GetObject("path"); //Добавляем путь к директории с модулем plotter.py
    PyList_Append(sysPath, PyUnicode_FromString("."));

    PyObject* pName = PyUnicode_DecodeFSDefault("plotter"); //Загружаем модуль plotter
    PyObject* pModule = PyImport_Import(pName);
    Py_DECREF(pName);//уменьшаем счетчик ссылок на имя, оно больше не нужно

    if (pModule != nullptr) {//модуль существует
        PyObject* pFunc = PyObject_GetAttrString(pModule, "plot_graph"); //Загружаем функцию plot_graph
        if (pFunc && PyCallable_Check(pFunc)) { //нашли функцию и ее можно вызвать
            PyObject* pArgs = PyTuple_Pack(1, PyUnicode_FromString("data.json")); //Передаем имя файла в Python функцию
            PyObject* pValue = PyObject_CallObject(pFunc, pArgs);  //Вызываем функцию
            if (pValue != nullptr) { //Проверяем, выполнен ли вызов успешно
                Py_DECREF(pValue);
            } else {
                PyErr_Print();
                std::cerr << "Function call failed" << std::endl;
            }
            Py_DECREF(pArgs);//Убираем использованные объекты
        } else {
            PyErr_Print();
            std::cerr << "Failed to find or call function 'plot_graph'" << std::endl;
        }
        Py_XDECREF(pFunc); //Освобождаем память
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
        std::cerr << "Failed to load module 'plotter'" << std::endl;
    }
    Py_Finalize();//Завершаем интерпретатор Python
}

template <typename T>
void calculate_norm(T **array_lagrange, T **array_fx_large, size_t K_const, size_t N_const, T *norm_1, T *norm_2, T *norm_inf){
    for (size_t i = 0; i < K_const; i++)
        for (size_t j = 0; j < N_const; j++)
            {

            }

//сделать четкие границы у графика (без запаса)
//двумерный массив значений, одномерный массив координат
//вызывать лагранж на каждом подотрезке????
//ломается на отрезке -50 50
}