#include <iostream>
#include <bitset>

std::string floatToIEEE754(float num) {
    std::bitset<32> bits(*reinterpret_cast<unsigned int*>(&num));
    std::string ieee754 = bits.to_string();

    // 切分符号位、指数位和尾数位
    std::string sign = ieee754.substr(0, 1);
    std::string exponent = ieee754.substr(1, 8);
    std::string mantissa = ieee754.substr(9);

    // 将符号位、指数位和尾数位组合成IEEE 754标准的十进制表示
    std::string decimal = sign + " " + exponent + " " + mantissa;
    return decimal;
}
float IEEE754ToFloat(const std::string& decimal) {
    std::string binary = "";
    
    // 移除空格
    for (char c : decimal) {
        if (c != ' ') {
            binary += c;
        }
    }

    // 重新组合符号位、指数位和尾数位
    std::string sign = binary.substr(0, 1);
    std::string exponent = binary.substr(1, 8);
    std::string mantissa = binary.substr(9);
    
    // 将符号位、指数位和尾数位组合为二进制表示
    std::string bitRepresentation = sign + exponent + mantissa;

    // 将二进制表示转换为float
    unsigned int num = std::bitset<32>(bitRepresentation).to_ulong();
    float result = *reinterpret_cast<float*>(&num);
    return result;
}

