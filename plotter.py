import json
import matplotlib.pyplot as plt

def flatten_2d_array(array_2d):
    """Функция для преобразования двумерного массива в одномерный"""
    return [item for sublist in array_2d for item in sublist]

def plot_graph(filename):
    # Чтение JSON данных из файла
    with open(filename, 'r') as file:
        data = json.load(file)

    # Преобразование двумерных массивов в одномерные
    arrX = flatten_2d_array(data['array_eval_points'])
    arrFx = flatten_2d_array(data['array_fx_large'])
    arrLagrange = flatten_2d_array(data['array_lagrange'])
    arrNodesX = flatten_2d_array(data['array_net_local'])
    valueInterpolation = flatten_2d_array(data['array_fx_local'])

    # Построение графика
    plt.figure(figsize =(15,15))
    plt.minorticks_on()
    plt.grid(which='major')
    plt.grid(which='minor')

    # Построение линий
    plt.plot(arrX, arrFx, color='blue', linewidth=2, label='fn')  # Синие линии для исходной функции
    plt.plot(arrX, arrLagrange, 'r--', linewidth=1.5, label='L')  # Красные линии для интерполяции Лагранжа

    # Отображение точек интерполяции
    plt.plot(arrNodesX, valueInterpolation, 'gs', markersize=3, label='Points')

    # Настройки осей и легенды
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.legend()
    plt.show()
