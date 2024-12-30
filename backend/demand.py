import requests
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.preprocessing import MinMaxScaler
from sklearn.model_selection import train_test_split
from sklearn.neighbors import KNeighborsRegressor


channel_id = '985835'
api_key = '7Q9NK03UEVYUN3ZS'


url = f'https://api.thingspeak.com/channels/{channel_id}/feeds.json?api_key={api_key}'


response = requests.get(url)


data = response.json()


df = pd.DataFrame(data['feeds'])


df['created_at'] = pd.to_datetime(df['created_at'])
df = df[['field1', 'field3', 'field5', 'created_at']].dropna()


df.sort_values(by='created_at', inplace=True)
df.set_index('created_at', inplace=True)


scaler = MinMaxScaler()
df[['field1', 'field3', 'field5']] = scaler.fit_transform(df[['field1', 'field3', 'field5']])


sequence_length = 10


X = df[['field1', 'field5']].values
y = df['field3'].values


X_train, X_val, y_train, y_val = train_test_split(X, y, test_size=0.2, random_state=42)


k = 2  
model = KNeighborsRegressor(n_neighbors=k)


model.fit(X_train, y_train)

y_pred = model.predict(X_val)


y_pred = y_pred.reshape(-1, 1)


plt.figure(figsize=(12, 6))
plt.plot(df.index[-len(y_val):], X_val[:, 0], label='Field1', color='blue')
plt.plot(df.index[-len(y_pred):], y_pred[:, 0], label='Predicted Field1', color='red')
plt.xlabel('Timestamp')
plt.ylabel('Field1')
plt.legend()
plt.title('Actual vs. Predicted Field1 (KNN)')
plt.grid(True)
plt.savefig('img.png')
plt.show()

