# linq v0.1.1

Wannabe fast and simple in use LINQ implementation in C++,


## Supported containers
- [x] vector
- [ ] array
- [ ] list

## Features
* basic type checking for passed lambda expresions

## Operations
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

## Usage

```
std::vector<int> v = {1, 2, 3, 4, 5};

auto where_result = v >> where([&](const int& value){ return value > 2; });
auto select_result = v >> select([&](const int& value){ return value % 2 == 0; });
v >> foreach([](int& x){ x *= 2; })
```
