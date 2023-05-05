#pragma once

//#define DO_QEM_DEBUG
#ifdef DO_QEM_DEBUG
#define QEM_DEBUG_CPP(x) do { std::cout << "[QEM-DEBUG]" << x << std::endl; } while (0)
#define QEM_DEBUG(x, ...) printf("[QEM-DEBUG]: " x "\n", ##__VA_ARGS__)
#define QEM_INFO(x, ...) printf("[QEM-INFO]: " x "\n", ##__VA_ARGS__)
#else
#define QEM_DEBUG_CPP(x) 
#define QEM_DEBUG(x, ...)  
#define QEM_INFO(x, ...)  
#endif