# SnakeAI

Snake training using gentic algorithm connected to neural network to predict future moves based on snake vision.Snake can see in 8 directions. Each direction gives 3 informations. First is distance to wall. Second is there an apple on the way. Third is there a part of his body. Neuron netowrk input consist of 28 input neurons. 24 of them give vision informations, rest are one hot encoded variable descibing snake head direction. Neuron network output is 4 values each for one direction. Snake move is decided by picking direction with highest value.   

Colors:   
- Red - Food   
- Green - Body  
- Blue - Tail   
- Yellow - Head   

![Animation](https://github.com/SlawekSt/SnakeAI/blob/main/Animation.gif)
