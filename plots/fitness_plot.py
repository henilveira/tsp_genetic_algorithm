import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results/fitness_data.csv")

fig, ax1 = plt.subplots()

ax1.plot(df["generation"], df["best_distance"], color="tab:blue", label="Distância")
ax1.set_xlabel("Geração")
ax1.set_ylabel("Distância", color="tab:blue")

ax2 = ax1.twinx()
ax2.plot(df["generation"], df["best_fitness"], color="tab:red", label="Fitness")
ax2.set_ylabel("Fitness", color="tab:red")

plt.title("Evolução do Fitness e da Distância")
plt.savefig("Figura 1", )
plt.show()
