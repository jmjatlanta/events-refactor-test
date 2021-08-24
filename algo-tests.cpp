#include <iostream>
#include <vector>
#include <algorithm>
//#include <sstream>
#include <iterator>

int main() {

    std::vector<int> vw = { 12, 15, 51, 17, 17, 19, 20, 22, 23, 26, 29, 35, 40 };
    //sort(vw.begin(), vw.end(), std::greater<int>());
    sort(vw.begin(), vw.end());
    
    std::ostream_iterator<int> out(std::cout, " ");
    copy(vw.begin(), vw.end(), out);
    std::cout << std::endl;

    int value = 19;
    if (std::binary_search(vw.begin(), vw.end(), value))
        std::cout << "binary_search(" << value << ") = true" << std::endl;
    
    for (value = 21; value < 23; ++value) {
        std::vector<int>::iterator it = std::lower_bound(vw.begin(), vw.end(), value);
        if (it != vw.end()) {
            std::cout << "lower_bound(" << value << ") = " << (*it) << " ";
            if ((*it) == value)
                std::cout << "(exact value)" << std::endl;
            else
                std::cout << "(equivalent value, position to insert)" << std::endl;
        }
    }

    // https://cpp.com.ru/meyers/ch7.html#t65
    for (value = 10; value < 53; ++value) {
        std::pair<std::vector<int>::iterator, std::vector<int>::iterator> it = std::equal_range(vw.begin(), vw.end(), value);
        if (it.first != it.second) {
            // value found
            std::cout << "Value (" << value << ") found! it.first = " << 
                        *(it.first) << ", it.second = " << 
                        (it.second != vw.end() ? std::to_string(*(it.second)) : std::string("end")) <<
                        " [" << std::distance(it.first, it.second) << "]" <<
                        std::endl;
        }
        else {
            // value not found
        }
    }

    std::vector<int> vw_other(vw);

    copy(vw_other.begin(), vw_other.end(), out); std::cout << std::endl;
    vw_other.erase(vw_other.begin(), std::upper_bound(vw_other.begin(), vw_other.end(), 17));
    copy(vw_other.begin(), vw_other.end(), out); std::cout << std::endl;
    return 0;
}