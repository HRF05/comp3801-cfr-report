import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('rpsB.csv')


plt.figure(figsize=(15, 10))


plt.plot(df.iloc[:10000, 0], label=df.columns[0])
plt.plot(df.iloc[:10000, 1], label=df.columns[1])
plt.plot(df.iloc[:10000, 2], label=df.columns[2])


plt.xlabel('Iterations (t)')
plt.ylabel('Probability values')
plt.title('Average of Strategies, Over Iterations')
plt.legend()
plt.grid(True)

plt.show()