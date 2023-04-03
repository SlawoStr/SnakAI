import Simulation
import multiprocessing
import time
import numpy as np
import random


# Pick parents using roulette Wheel
def rouletteWheel(fitnessList, parentsNumber):
    fitnessSum = np.sum(fitnessList)
    probabilities = []
    cumulative_probabilities = []
    parent_list = []

    # Calculate percentages of fitness in comparison to whole generation
    for i in range(len(fitnessList)):
        probabilities.append(fitnessList[i] / fitnessSum)

    # Calculate
    for i in range(len(fitnessList)):
        cumulative = 0
        counter = 0
        while counter <= i:
            cumulative = cumulative + probabilities[counter]
            counter = counter + 1
        cumulative_probabilities.append(cumulative)
    while len(parent_list) < parentsNumber:
        randomValue = random.uniform(0, 1)
        parent = None
        for i in range(0, len(cumulative_probabilities) - 1):
            if randomValue < cumulative_probabilities[0]:
                parent = 0
                break
            elif cumulative_probabilities[i] <= randomValue <= cumulative_probabilities[i + 1]:
                parent = i + 1
                break
        if parent not in parent_list:
            parent_list.append(parent)
    return parent_list


# Pick best parents from whole generations
def bestFitness(fitnessList, parentsKept):
    ind = np.argpartition(fitnessList, -parentsKept)[-parentsKept:]
    return ind


class MyProcess(multiprocessing.Process):

    def __init__(self, processID, child_conn, model_number, load_models, start_index):
        multiprocessing.Process.__init__(self)
        self.processID = processID
        self.start_index = start_index
        self.child_cone = child_conn
        self.model_number = model_number
        self.load_models = load_models
        self.model_pool = []

    def run(self):
        # library must be imported for every process
        from modelBuild import createModel
        import tensorflow as tf

        # Limit memory allocation for each process
        physical_devices = tf.config.list_physical_devices('GPU')
        tf.config.experimental.set_memory_growth(physical_devices[0], True)

        # Create models
        for i in range(self.model_number):
            model = createModel(True)
            self.model_pool.append(model)
        # Load models weights
        if self.load_models:
            for i in range(self.model_number):
                self.model_pool[i].load_weights("SavedModels/model" + str(self.start_index + i) + ".keras")
        # main loop of processes
        while True:
            value = self.child_cone.recv()
            operationNumber = value[0]
            # Prediction
            if operationNumber == 0:
                results = []
                # Predict data (skip first value)
                for i in range(self.model_number):
                    if(len(value[i+1]) < 5):
                        results.append(-1)
                        continue
                    numpy_values = np.atleast_2d(value[i + 1])
                    predict = self.model_pool[i](numpy_values)
                    output_prob = np.argmax(predict)
                    results.append(output_prob)
                self.child_cone.send(results)
            # Save Model
            elif operationNumber == 1:
                for i in range(self.model_number):
                    self.model_pool[i].save_weights("SavedModels/model" + str(self.start_index + i) + ".keras")
                print("saved")
                break
            # Mutation
            elif operationNumber == 2:
                parentList = value[1:]
                weights = []
                for i in range(len(parentList)):
                    ind = parentList[i] - self.start_index
                    if 0 <= ind < self.model_number:
                        weights.append(self.getWeight(ind, True))
                self.child_cone.send(weights)

                weights = self.child_cone.recv()
                best_parents = self.child_cone.recv()
                self.modifyPopulation(weights, best_parents)

                self.child_cone.send([])

    def getWeight(self, modelIndex, bias):
        """

        :param modelIndex: lokal index of model
        :param bias: model w/wo bias
        :return: list of weights
        """
        results = []
        weights = self.model_pool[modelIndex].get_weights()
        for i in range(0, len(weights)):
            for j in range(0, len(weights[i])):
                if bias and i % 2 != 0:
                    results.append(weights[i][j])
                else:
                    for k in range(0, len(weights[i][j])):
                        results.append(weights[i][j][k])
        return results

    def setModelWeight(self, index, weights, use_bias):
        current_model = self.model_pool[index].get_weights()
        counter = 0
        for i in range(0, len(current_model)):
            for j in range(0, len(current_model[i])):
                if use_bias and i % 2 != 0:
                    current_model[i][j] = weights[counter]
                    counter = counter + 1
                else:
                    for k in range(0, len(current_model[i][j])):
                        current_model[i][j][k] = weights[counter]
                        counter = counter + 1
        self.model_pool[index].set_weights(current_model)

    def modifyPopulation(self, weight, best_parents):
        parentsNumber = len(weight)

        kidNumber = self.model_number

        if kidNumber % 2 != 0:
            kidNumber = kidNumber + 1

        new_weight = []

        for i in range(int(kidNumber / 2)):
            firstParent = random.randint(0, parentsNumber - 1)
            secondParent = random.randint(0, parentsNumber - 1)
            while secondParent == firstParent:
                secondParent = random.randint(0, parentsNumber - 1)

            childs = self.modelCrossover(weight[firstParent], weight[secondParent])
            self.mutate(childs[0])
            self.mutate(childs[1])

            new_weight.append(childs[0])
            new_weight.append(childs[1])

        for i in range(self.model_number):
            if i+self.start_index in best_parents:
                print("Number:",i+self.start_index)
                continue
            else:
                self.setModelWeight(i, new_weight[i], True)

    def modelCrossover(self, weight1, weight2):
        # One point crossover
        value = random.randint(0, len(weight1) - 1)

        new_weight1 = weight1[:value]
        new_weight2 = weight2[:value]

        for i in range(value, len(weight1)):
            new_weight1.append(weight2[i])
            new_weight2.append(weight1[i])

        return np.asarray([new_weight1, new_weight2])

    def mutate(self, weight):
        # Pick random weights/bias and random its value
        for i in range(len(weight)):
            value = random.uniform(0, 1)
            if value < 0.1:
                weight[i] = random.uniform(-1, 1)
        return weight


class GeneticAlgorithm:
    def __init__(self):
        # Numbers ot models in each generations
        self.modelsNumber = 1000
        # Process List
        self.processList = []
        # parent/child pipe connections
        self.parentConn = []
        self.childConn = []
        # processes Number
        self.processNumber = multiprocessing.cpu_count()
        # c++ connection
        self.simulator = Simulation.Simulation2()

    def setModelsNumber(self, modelNumber):
        self.modelsNumber = modelNumber

    def setProcessesNumber(self, processesNumber):
        self.processNumber = processesNumber

    def loadData(self, loadModels):
        # Create pipelines
        for i in range(self.modelsNumber):
            child_conn, parent_conn = multiprocessing.Pipe()
            self.childConn.append(child_conn)
            self.parentConn.append(parent_conn)

        modelsPerProcess = int(self.modelsNumber / self.processNumber)

        # Create processes
        for i in range(self.processNumber):
            if i != self.processNumber - 1:
                process = MyProcess(i, self.childConn[i], modelsPerProcess, loadModels, modelsPerProcess * i)
                self.processList.append(process)
            else:
                modelsPerProcess = self.modelsNumber - (self.processNumber - 1) * modelsPerProcess
                process = MyProcess(i, self.childConn[i], modelsPerProcess, loadModels,
                                    self.modelsNumber - modelsPerProcess)
                self.processList.append(process)

        # Start processes
        for i in range(self.processNumber):
            self.processList[i].start()

        # Wait for keras library to load
        time.sleep(15)

    def predictData(self, inputList):
        results = []

        modelsPerProcess = int(self.modelsNumber / self.processNumber)

        # Send sliced inputList to processes
        for i in range(self.processNumber):
            if i != self.processNumber - 1:
                self.parentConn[i].send([0] + inputList[modelsPerProcess * i:modelsPerProcess * (i + 1)])
            else:
                self.parentConn[i].send([0] + inputList[modelsPerProcess * i:self.modelsNumber])

        for i in range(self.processNumber):
            results.append(self.parentConn[i].recv())
        return [item for sublist in results for item in sublist]

    def saveModel(self):
        # Send opeartion number of saveModels
        for i in range(self.processNumber):
            self.parentConn[i].send([1])
        # Wait for all processes to end their job
        for i in range(self.processNumber):
            self.processList[i].join()

    def modifyPopulation(self,parentsKept, parentsNumber):
        fitnessList = self.simulator.getFiness()
        bestSnakes = []
        if parentsKept != 0:
            # Pick parents with best fitness that will survive till next generation
            bestSnakes = bestFitness(fitnessList, parentsKept)
        for i in range(len(bestSnakes)):
            print("Snake number",bestSnakes[i],"Fitness score: ",fitnessList[bestSnakes[i]])
        # Get new generation parent population by using roulettWheel
        parentsList = rouletteWheel(fitnessList, parentsNumber)
        print("parents")
        print(parentsList)
        weight_list = []
        # Get from processes parents weight (it is neccessary to give other processes acess to parents weights
        for i in range(self.processNumber):
            self.parentConn[i].send([2] + parentsList)
        for i in range(self.processNumber):
            weight_list.append(self.parentConn[i].recv())

        # Convert list of lists of lists to list of weight
        parents_weights = []
        for element in weight_list:
            for element2 in element:
                parents_weights.append(element2)
        # Send choosen parents
        for i in range(self.processNumber):
            self.parentConn[i].send(parents_weights)
        # Send parents that should survive mutation
        for i in range(self.processNumber):
            self.parentConn[i].send(bestSnakes)
        for i in range(self.processNumber):
            weight_list.append(self.parentConn[i].recv())


if __name__ == '__main__':
    genetic = GeneticAlgorithm()
    genetic.setModelsNumber(1000)
    genetic.setProcessesNumber(6)
    genetic.loadData(False)
    for i in range(200):
        print("Generation Number{}".format(i))
        start_time = time.time()
        genetic.simulator.resetPopulation()
        while(genetic.simulator.isAlive()):
            # Get input from c++ code
            predictData = genetic.predictData(genetic.simulator.getVision())
            # Return to c++ predicted values
            genetic.simulator.makeMove(predictData)

        genetic.modifyPopulation(200, 40)
        print('That took finally {} seconds'.format(time.time() - start_time))
    genetic.saveModel()

