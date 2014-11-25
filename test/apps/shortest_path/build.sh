g++ -g -std=c++11 -O2 -Wall -pthread -c main.cpp
g++ -g -std=c++11 -O2 -Wall -pthread -o main main.o -pthread -ltbb
#c++ -g -std=c++11 -O2 -Wall -pthread -c main.cpp  
#c++ -g -std=c++11 -O2 -Wall -pthread -o main main.o -pthread -ltbb
