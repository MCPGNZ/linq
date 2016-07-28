# mcplinq

Wannabe fast and simple in use LINQ implementation in C++,


## Supported containers
- [x] vector
- [ ] array
- [ ] list

## Features
* where
* select
* foreach
* any
* all
* contains
* sum
* min
* max
* count
* skip
* take 
* range

## Usage

```
std::vector<int> v = {1, 2, 3, 4, 5, 6};

auto result = v >> where([](int x){ return x % 2; });     
auto result = v >> select([](int x){ return x * 2.0f;});  
v >> foreach([](int& x){ x *= 2;})
```
  
