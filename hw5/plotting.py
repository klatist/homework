# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt

# Antal processer
processes = [2, 4, 6, 8]

# Exempeldata: median-tider (i sekunder)
# Fyll i dina beräknade medianvärden för varje algoritm och rundor
centralized = {
    1: [1.2e-5, 1.2e-2, 1.3e-4, 3.9e-4],  # R = 1
    2: [1.3e-5, 2.4e-2, 2.4e-4, 2.2e-4],  # R = 2
    3: [1.4e-5, 4.5e-5, 2.3e-4, 3.0e-4],  # R = 3
}

symmetric = {
    1: [1.3e-5, 1.2e-2, 3.7e-4, 5.6e-4],
    2: [8.5e-6, 3.5e-2, 2.9e-4, 4.3e-4],
    3: [1.0e-5, 3.8e-2, 3.0e-4, 5.3e-4],
}

circular = {
    1: [1.3e-5, 5.3e-5, 1.3e-4, 3.0e-4],
    2: [3.6e-6, 1.5e-5, 7.9e-5, 1.9e-4],
    3: [3.7e-6, 7.7e-6, 9.2e-5, 2.0e-4],
}

# Rita grafen
plt.figure(figsize=(10,6))

colors = {1:'blue', 2:'green', 3:'red'}

for r in [1, 2, 3]:
    plt.plot(processes, centralized[r], marker='o', color=colors[r], linestyle='-', label=f'Centralized R={r}')
    plt.plot(processes, symmetric[r], marker='s', color=colors[r], linestyle='--', label=f'Symmetric R={r}')
    plt.plot(processes, circular[r], marker='^', color=colors[r], linestyle='-.', label=f'Circular R={r}')

plt.xlabel("Number of processes")
plt.yscale('log')
plt.ylabel("Median execution time (s)")
plt.title("Execution time vs Number of processes and rounds")
plt.legend()
plt.grid(True, which="both", linestyle="--", linewidth=0.5 )
plt.show()