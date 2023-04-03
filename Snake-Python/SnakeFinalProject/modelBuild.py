from keras.models import Sequential
from keras.layers import Dense
import tensorflow as tf

'''
def createModel(useBias):
    initializer = tf.keras.initializers.RandomUniform(minval=-1, maxval=1, seed=None)
    model = Sequential()
    model.add(
        Dense(20, input_dim=28, kernel_initializer=initializer, bias_initializer=initializer, activation='relu',
              use_bias=useBias))
    model.add(
        Dense(4, activation='sigmoid', kernel_initializer=initializer, bias_initializer=initializer, use_bias=useBias))
    model.compile(loss='mse', optimizer='adam')

    return model
'''

def createModel(useBias):
    initializer = tf.keras.initializers.RandomUniform(minval=-1, maxval=1, seed=None)
    model = Sequential()
    model.add(
        Dense(20, input_dim=28, kernel_initializer=initializer, bias_initializer=initializer, activation='relu',
              use_bias=useBias))
    model.add(
        Dense(12, activation='relu', kernel_initializer=initializer, bias_initializer=initializer, use_bias=useBias))
    model.add(
        Dense(4, activation='sigmoid', kernel_initializer=initializer, bias_initializer=initializer, use_bias=useBias))
    model.compile(loss='mse', optimizer='adam')

    return model
