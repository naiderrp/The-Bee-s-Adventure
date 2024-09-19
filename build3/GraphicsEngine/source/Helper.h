#pragma once
#include <exception>

#define KBs(x) (x) * 1024

#define API_EVAL_HR(hr, error) if (hr != S_OK) { std::cout << "API Error: " << error << " In file: " << __FILE__ << '\n'; }
#define PRINT_W_N(message) std::wcout << message << '\n';
#define PRINT_N(message) std::cout << message << '\n';

#ifdef _DEBUG
#define API_ASSERT(exp) if (!(exp)) { std::cout << "Critical error in file: " << __FILE__ << '\n'; throw; }
#else
#define API_ASSERT(exp)
#endif