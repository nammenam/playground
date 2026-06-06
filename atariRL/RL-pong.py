import matplotlib.pyplot as plt
import numpy as np
import gym
import matplotlib
matplotlib.use('TKAgg')


def logistic(x):
    return 1/(1+np.exp(-x))

def logistic_diff(x):
    return logistic(x)*(1-logistic(x))

def ReLU(x):
    return np.where(x > 0, x, 0)

def ReLU_diff(x):
    return np.where(x > 0, 1, 0)





class NN():
    """
    A multi-layer neural network with one hidden layer
    """
    
    def __init__(self, dim_hidden = 6, eta=0.001, epochs = 100,  tol=0.01, n_epochs_no_update=10):
        # Intialize the hyperparameters
        self.dim_hidden = dim_hidden
        self.eta = eta
        self.epochs = epochs
        self.tol = tol
        self.n_epochs_no_update = n_epochs_no_update
        
        self.activ = ReLU
        self.activ_diff = logistic_diff
        



    def fit(self, X_train, t_train, X_val=None, t_val=None,  batch_size=5):
        (N, m) = X_train.shape
        batches = int(N/batch_size)
        dim_in = m 
        dim_out = t_train.shape[1]
        self.init_weights_and_biases(dim_in, dim_out)
        
        if (X_val is None) or (t_val is None): 
            for e in range(self.epochs):
                for _ in range(batches):
                    random_index = batch_size*np.random.randint(batches)
                    self.backpropagation(
                        X_train[random_index:random_index+batch_size], 
                        t_train[random_index:random_index+batch_size], N
                    )

        else:
            self.loss = np.zeros(self.epochs)
            self.accuracies = np.zeros(epochs)

            for e in range(self.epochs):
                for _ in range(batches):
                    random_index = batch_size*np.random.randint(batches)
                    self.backpropagation( 
                        X_train[random_index:random_index+batch_size],
                        t_train[random_index:random_index+batch_size], N
                    )
                self.loss[e] = MSE(self.weights, X_val, t_val)
                self.accuracies[e]= accuracy(self.predict(X_val), t_val)


                if e > self.n_epochs_no_update and np.abs(self.loss[e-self.n_epochs_no_update] - self.loss[e]) < self.tol:
                    self.loss[e:] = self.loss[e]
                    print(f"Early stopping at epoch {e}")
                    return
                print("\rDid not converge")


        
            
    def forward(self, X):
        hidden_activations = self.activ(X @ self.weights1)
        outputs = hidden_activations @ self.weights2
        return hidden_activations, outputs
    



    def predict(self, X):
        forw = self.forward(Z)[1]
        score= forw[:, 0]
        return (score > 0.5).astype('int')
    



    def predict_probability(self, X):
        return self.forward(Z)[1][:, 0]
        


    def init_weights_and_biases(self, dim_in, dim_out):
        self.hidden_weights = np.random.randn(dim_in, self.dim_hidden)
        self.hidden_bias = np.zeros(self.dim_hidden) + 0.01
        self.output_weights = np.random.randn(self.dim_hidden, dim_out)
        self.output_bias = np.zeros(dim_out) + 0.01


    def backward(self, X, T, N):
        hidden_outs, outputs = self.forward(X)
        out_deltas = (outputs - T)
        hiddenout_diffs = out_deltas @ self.output_weights.T
        hiddenact_deltas = (hiddenout_diffs[:, 1:] * self.activ_diff(hidden_outs[:, 1:]))
        return hidden_outs, outputs, out_deltas, hiddenout_diffs, hiddenact_deltas


    def backpropagation(self, X, T, N):
        # One epoch
        hidden_outs, outputs = self.forward(X)
        # The forward step
        out_deltas = (outputs - T)
        # The delta term on the output node
        hiddenout_diffs = out_deltas @ self.weights2.T
        # The delta terms at the output of the jidden layer
        hiddenact_deltas = (hiddenout_diffs[:, 1:] * self.activ_diff(hidden_outs[:, 1:])) # first index is bias hence [:, 1:]

        self.weights2 -= self.eta * hidden_outs.T @ out_deltas
        self.weights1 -= self.eta * X.T @ hiddenact_deltas 





def preprocess(observation):
    """
    Preprocess the observation
    """
    X = observation[34:194, :, 0]
    X = X[::2, ::2]
    X[X == 144] = 0
    X[X == 109] = 0
    X[X != 0] = 1
    return X
    

if __name__ == "__main__":
    env = gym.make('ALE/Pong-v5', render_mode='human')
    env.reset()
    for _ in range(1000):
        env.render()
        (observation, reward, terminated, truncated, info) = env.step(env.action_space.sample()) # take a random action
        print(terminated)
        if terminated:
            observation = env.reset()
    env.close()

