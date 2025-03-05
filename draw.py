import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import seaborn as sns
import numpy as np
import pandas as pd

class Draw:
    """
    This class provides tools for visualizing experimental data using 2D and 3D plots. 

    The `Draw` class supports three types of plots:
    1. Time-based plots: Useful for visualizing execution time.
    2. Acceleration-based plots: Useful for speedup visualization by comparing the initial result with subsequent values.
    3. 3D scatter plots: Useful for presenting data points in three-dimensional space.

    Methods:
    --------
    1. draw_grafic:
        Plots a single 2D graph.
        Parameters:
            - dir (str): Path to the file containing data values.
            - name (str): Type of the graph ('Time' or 'Accelerate').
            - size_input_data (int, optional): The size of the input data involved in the experiment.
            - x (list, optional): Custom x-axis values; if not provided, defaults to an enumeration from 1 to n.
            - info (str, optional): Additional information for the graph title.
            - x_label (str, optional): Label for the x-axis.
            - y_label (str, optional): Label for the y-axis.

    2. draw_grafics:
        Plots multiple 2D graphs on the same figure.
        Parameters:
            - dirs (list[str]): List of file paths containing data for multiple experiments.
            - labels (list[str]): Labels for each dataset to be used in the legend.
            - name (str): Type of the graph ('Time' or 'Accelerate').
            - size_input_data (int, optional): Size of the input data.
            - x (list, optional): Custom x-axis values.
            - info (str, optional): Additional information for the graph title.
            - x_label (str, optional): Label for the x-axis.
            - y_label (str, optional): Label for the y-axis.

    3. draw_3D_grafic:
        Creates a 3D scatter plot.
        Parameters:
            - dir (str): Path to the file containing data with x, y, and z values.
            - x_label (str): Label for the x-axis.
            - y_label (str): Label for the y-axis.
            - z_label (str): Label for the z-axis.
            - title (str): Title of the graph.
    
    Features:
    ---------
    - Data is read from text files, where each line represents a value (for 2D plots) or a set of coordinates (for 3D plots).
    - Acceleration graphs calculate speedup as the ratio between the first value and subsequent values.
    - Titles, axis labels, and grid settings are customizable.
    - For 3D graphs, annotations and vertical lines to the z=0 plane provide additional context.
    """
   
    @staticmethod
    def draw_grafic(dir: str,
                    name: str,
                    size_input_data: int = None,
                    x: list = None,
                    info: str = None,
                    x_label: str = None,
                    y_label: str = None
                    ) -> None:

        y = list()
        with open(dir, 'r') as file:
            for line in file:
                y.append(float(line))

        if name == 'Accelerate':
            simple = y[0]
            y = [simple / res if res > 0.0 else 0.0 for res in y]
            
        if not x:
            x = [(k + 1) for k in range(len(y))]
            
        plt.plot(x, y, color="blue", marker="o")
        
        plt.xlabel("num processors" if not x_label else x_label)
        if name == 'Time':
           plt.ylabel(f"{name} (seconds)" if not y_label else y_label)
        else:
            plt.ylabel(name if not y_label else y_label)
         
        if size_input_data:
            name = name + f", N={str(size_input_data)}"
        if info is not None:
            name = name + ", " + info
        
        plt.title(name)
        plt.grid(True)
        
    @staticmethod
    def draw_grafics(dirs: list[str],
                    labels: list[str],
                    name: str,
                    size_input_data: int = None,
                    x: list = None,
                    info: str = None,
                    x_label: str = None,
                    y_label: str = None
                    ) -> None:
        
        for dir, label in zip(dirs, labels):
            y = []
            with open(dir, 'r') as file:
                for line in file:
                    y.append(float(line))
                    
            if not x:
                x = [(k + 1) for k in range(len(y))]
            
            if name == 'Accelerate':
                simple = y[0]
                y = [simple / res if res > 0.0 else 0.0 for res in y]
                
            plt.plot(x, y, label=label, marker="o")
            
        plt.xlabel("num processors" if not x_label else x_label)
        if name == 'Time':
           plt.ylabel(f"{name} (seconds)" if not y_label else y_label)
        else:
            plt.ylabel(name if not y_label else y_label)
        
        if size_input_data:
            name = name + f", N={str(size_input_data)}"
        if info is not None:
            name = name + ", " + info
        
        plt.legend()
        plt.title(name)
        plt.grid(True)
        
    @staticmethod
    def draw_3D_grafic(dir: str,
                       x_label: str,
                       y_label: str,
                       z_label: str,
                       title: str,
                       normalize: int = None
        ) -> None:
        
        fig = plt.figure(figsize=(12, 9))
        ax = fig.add_subplot(111, projection='3d')
        
        x = list()
        y = list()
        z = list()
            
        
        with open(dir, 'r') as file:
            for line in file:
                line = line.split(' ')
                x.append(int(line[0]))
                y.append(int(line[1]))
                z.append(float(line[2]))
                
        if normalize:
            z = [r / normalize for r in z]
        
        ax.scatter(x, y, z, c='r', marker='o')
        
        # Добавление аннотаций и линий к каждой точке
        for i in range(len(x)):
            # Линия от точки к плоскости z=0
            ax.plot([x[i], x[i]], [y[i], y[i]], [0, z[i]], color='gray', linestyle='--')
            
            # Аннотация с координатами
            label = f'({z[i]:.2f})'
            ax.text(x[i], y[i], z[i], label, fontsize=8, color='blue')
        
        # Настройки осей
        ax.set_xlabel(x_label, labelpad=0)
        ax.set_ylabel(y_label, labelpad=0)
        ax.set_zlabel(z_label, labelpad=0)
        ax.set_title(title) 
    
        
        plt.tight_layout()
        
        
    