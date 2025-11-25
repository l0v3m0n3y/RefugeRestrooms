# RefugeRestrooms
api for refugerestrooms.org Users can search for restrooms by proximity to a search location, add new restroom listings, as well as comment and rate existing listings.
# main
```cpp
#include "RefugeRestrooms.h"
#include <iostream>

int main() {
   RefugeRestrooms api;
    auto restrooms = api.get_all_restrooms().then([](json::value result) {
        std::cout << result<< std::endl;
    });
    restrooms.wait();
    
    return 0;
}
```

# Launch (your script)
```
g++ -std=c++11 -o main main.cpp -lcpprest -lssl -lcrypto -lpthread -lboost_system -lboost_chrono -lboost_thread
./main
```
