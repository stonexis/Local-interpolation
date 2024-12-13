import json
import seaborn as sns
import matplotlib.pyplot as plt

def flatten_2d_array(array_2d):
    """Функция для преобразования двумерного массива в одномерный"""
    return [item for sublist in array_2d for item in sublist]

def plot_graph_seaborn(filename):
    # Чтение JSON данных из файла
    with open(filename, 'r') as file:
        data = json.load(file)

    # Преобразование двумерных массивов в одномерные
    array_nodes = flatten_2d_array(data['array_nodes'])
    array_f_nodes = flatten_2d_array(data['array_f_nodes'])
    array_x = flatten_2d_array(data['array_x'])
    array_fx = flatten_2d_array(data['array_fx'])
    array_approximation = flatten_2d_array(data['array_approximation'])

    # Установка темы и стиля
    sns.set_theme(style="whitegrid", palette="muted", font="serif", font_scale=1.2)

    # Построение графика
    plt.figure(figsize=(14, 8))

    # Построение линий для исходной функции и аппроксимации
    sns.lineplot(x=array_x, y=array_fx, label='f(x)', linewidth=2, zorder=1)
    sns.lineplot(x=array_x, y=array_approximation, label='L(x)', linestyle='--', linewidth=1.5, zorder=2)

    # Добавление точек интерполяции
    sns.scatterplot(x=array_nodes, y=array_f_nodes, s=40, label='Points', marker='o', zorder=3)

    # Настройки графика
    plt.xlabel("X", fontsize=14)
    plt.ylabel("Y", fontsize=14)
    plt.xticks(fontsize=12)
    plt.yticks(fontsize=12)

    # Настройка сетки
    plt.grid(which='major', linestyle='-', linewidth=0.5)  # Основная сетка
    plt.grid(which='minor', linestyle=':', linewidth=0.5)  # Мелкая сетка
    plt.minorticks_on()  # Включение дополнительной сетки
    plt.tight_layout()

    # Легенда
    plt.legend(fontsize=12, loc="best", fancybox=True)

    # Отображение графика
    plt.show()

# Вызов функции
plot_graph_seaborn("data.json")


