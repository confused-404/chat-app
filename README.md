# chat-app

To compile, using g++:

```$ g++ src/main.cpp src/network/NetworkUtils.cpp src/client/ClientManager.cpp -o bin/main -lWs2_32```

Then run the resulting bin/main.exe file to host the server

To test:

```$ python src/tester.py```