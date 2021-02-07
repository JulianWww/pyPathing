#include <vector>

namespace jce{
    namespace vector {
        template <typename T, int n>
        struct NDVector {
            typedef std::vector<typename NDVector<T, n - 1>::type> type;
        };

        template <typename T>
        struct NDVector<T, 0> {
            typedef T type;
        };

        template <typename T>
        std::vector<T> make_vector(std::size_t size) {
            return std::vector<T>(size);
        }

        template <typename T>
        std::vector<T> make_vector(int x) {
            std::size_t size = x;
            return std::vector<T>(size);
        }

        template <typename T, typename... Args>
        typename NDVector<T, sizeof...(Args) + 1>::type make_vector(int first, Args... sizes) {
            std::size_t x = first;
            typedef typename NDVector<T, sizeof...(Args) + 1>::type Result;
            return Result(x, make_vector<T>(sizes...));
        }

        template <typename T, typename... Args>
        typename NDVector<T, sizeof...(Args) + 1>::type make_vector(std::size_t first, Args... sizes) {
            typedef typename NDVector<T, sizeof...(Args) + 1>::type Result;
            return Result(first, make_vector<T>(sizes...));
        }

        template <typename T>
        std::vector<T> slice(std::vector<T>& vec, std::pair<std::size_t, std::size_t> size) {
            // slice a vector
         
            std::vector<T> result = make_vector<T>(size.second - size.first);
            std::copy(vec.begin() + size.first, vec.begin() + size.second, result.begin());
            return result;
        }

       template<typename T, typename... Args>
       std::vector<T> slice(std::vector<T> vec, std::pair<std::size_t, std::size_t> pos,  Args... keys){
           std::vector<T> res;
           for (auto iter = vec.begin() + pos.first; iter != vec.begin() + (pos.second) && iter != vec.end(); iter++) {
               auto v = slice(*iter, keys...);
               res.push_back(v);
           }
           return res;

        }
    
    
       template <typename T>
       std::vector<T> add(std::vector<T>a, std::vector<T>b) {
           if (a.size() != b.size()) {
               throw "size mismatched";
           }
           std::vector<T> res(a.size());
           auto a_iter = a.begin();
           auto res_iter = res.begin();
           for (auto b_iter = b.begin(); b_iter != b.end();) {
               *res_iter = *a_iter + *b_iter;
               res_iter++; a_iter++; b_iter++;
           }
           return res;
       }
    
        template<typename T>
        std::vector<T> mod(std::vector<T> vec, T v) {
            for (auto iter = vec.begin(); iter != vec.end(); iter++) {
                (*iter) = (*iter) % v;
            }
            return vec;
        }

        template<typename T>
        std::vector<T> div(std::vector<T> vec, T v) {
            for (auto iter = vec.begin(); iter != vec.end(); iter++) {
                (*iter) = (*iter) / v;
            }
            return vec;
        }

        template<typename T>
        std::vector<T> sub(std::vector<T> vec, std::vector<T> oter) {
            auto v = oter.begin();
            for (auto iter = vec.begin(); iter != vec.end(); iter++) {
                (*iter) = (*iter) - (*v);
                v++;
            }
            return vec;
        }
    }
}