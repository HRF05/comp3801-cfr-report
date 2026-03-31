import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("regret_metrics.csv")

plt.figure(figsize=(15, 10))

plt.plot(data['Iteration'], data['Avg_Regret_Per_Iteration'], label='Iteration')

plt.title('Average Cumulative Regret, Over Iterations')
plt.xlabel('Iterations (t)')
plt.ylabel('Average Cumulative Regret')
plt.grid(True)
plt.legend(loc="upper right")

plt.show()