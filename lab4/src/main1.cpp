#include "func1.h"
#include <iostream>
#include <string>
int main(int argc, char** argv){
    if (argc < 2){
        std::cout << "Please provide a number as an argument." << std::endl;
        return 1;
    }  
    
    if (std::string(argv[1]) == "1"){
        if (argc == 4){
            std::cout<<Derivative( std::stod(argv[2]), std::stod(argv[3]))<<std::endl;
            return 0;
        }
        else{
            std::cout << "неверное количетво парамтров" << std::endl;
            return 1;
        }
    }
    
    else if (std::string(argv[1]) == "2"){
         if (argc == 3){
            std::cout<< E( std::stod(argv[2]))<<std::endl;
            return 0;
        }
        else{
            std::cout << "неверное количетво парамтров" << std::endl;
            return 1;
        }
    }
    else{
        std::cout << "Invalid argument. Please provide '1' or '2'." << std::endl;
        return 1;
    }

}