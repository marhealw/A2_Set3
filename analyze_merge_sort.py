import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results.csv")

print("Данные загружены:", len(df), "строк")
print("Типы данных:", df["Type"].unique())
print("Алгоритмы:", df["Algorithm"].unique())

def plot_merge_only(data):
    """Строит графики стандартного MergeSort для всех типов массивов"""
    plt.figure(figsize=(10, 6))
    for t in data["Type"].unique():
        subset = data[(data["Type"] == t) & (data["Algorithm"] == "MergeSort")]
        plt.plot(subset["Size"], subset["Time(ms)"], label=t)
    plt.title("Эмпирический анализ стандартного Merge Sort")
    plt.xlabel("Размер массива")
    plt.ylabel("Время выполнения (мс)")
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.show()


def plot_merge_one_type(data, arr_type):
    """График MergeSort для одного типа массива"""
    subset = data[(data["Type"] == arr_type) & (data["Algorithm"] == "MergeSort")]
    plt.figure(figsize=(10, 6))
    plt.plot(subset["Size"], subset["Time(ms)"], label=f"{arr_type} массив")
    plt.title(f"MergeSort — {arr_type}")
    plt.xlabel("Размер массива")
    plt.ylabel("Время выполнения (мс)")
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.show()


def plot_hybrid_thresholds(data, arr_type):
    """Графики гибридного Merge+Insertion для одного типа массива"""
    subset = data[(data["Type"] == arr_type) & (data["Algorithm"] == "MergeHybrid")]

    plt.figure(figsize=(10, 6))
    for threshold in sorted(subset["Threshold"].unique()):
        cur = subset[subset["Threshold"] == threshold]
        plt.plot(cur["Size"], cur["Time(ms)"], label=f"threshold = {threshold}")

    plt.title(f"Merge + Insertion Sort ({arr_type} массив)")
    plt.xlabel("Размер массива")
    plt.ylabel("Время выполнения (мс)")
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.show()

# Графики стандартного MergeSort (все типы)
plot_merge_only(df)

# Для одного типа (например, Random)
plot_merge_one_type(df, "Random")

# Графики для гибридного алгоритма
plot_hybrid_thresholds(df, "Random")
plot_hybrid_thresholds(df, "Reverse")
plot_hybrid_thresholds(df, "AlmostSorted")

