import matplotlib.pyplot as plt

# Данные для графика
x = [1, 2, 3, 4, 5]
y = [2, 4, 6, 8, 10]

# Создание графика
plt.plot(x, y, label="y = 2x", color="blue", marker="o")

# Добавление подписи осей
plt.xlabel("X")
plt.ylabel("Y")

# Заголовок графика
plt.title("Пример линейного графика")

# Добавление легенды
plt.legend()

# Отображение графика
plt.grid(True)  # Включение сетки
plt.show()
