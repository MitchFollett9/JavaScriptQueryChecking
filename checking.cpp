#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

// Define a class to hold the object data
class MyObject {
public:
    int value1;
    std::string value2;
};

void clearCSVFile() {
    std::ofstream file("out.csv", std::ofstream::out | std::ofstream::trunc);
    
    if (file.is_open()) {
        file.close();
        std::cout << "out.csv has been cleared." << std::endl;
    } else {
        std::cerr << "Unable to open the file." << std::endl;
    }
}

void writeToCSV(int input1, int input2, float input3, int input4, int input5, float input6, float input7) {
    std::ofstream file("out.csv", std::ios::app);
    
    if (file.is_open()) {
        file << input1 << "," << input2 << "," << input3 <<  "," << input4 <<  "," << input5 << "," << input6 << "," << input7 << "\n";
        file.close();
        // std::cout << "Data written to out.csv successfully." << std::endl;
    } else {
        std::cerr << "Unable to open the file." << std::endl;
    }
}

int countLengthOccurrences(const std::string& str, int length) {
    int count = 0;
    std::istringstream iss(str);

    std::string word;
    while (iss >> word) {
        // Remove any non-alphabetic characters from the word
        std::string cleanWord;
        for (char c : word) {
            if (isalpha(c) || c == '_') {
                cleanWord += c;
            }
        }

        if (cleanWord.length() >= length) {
            count++;
        }
    }

    return count;
}

int countOccurrences(std::string str, std::string findString) {
    int count = 0;
    size_t pos = 0;

    while ((pos = str.find(findString, pos)) != std::string::npos) {
        count++;
        pos += findString.length();
    }

    return count;
}

int removeNonDigitsAndConvertToInt(std::string str) {
    std::string digits;

    for (char c : str) {
        if (std::isdigit(c) || c == '-') {
            digits += c;
        }
    }

    if(digits.length() == 0 || digits.length() > 5 || digits == "-" || (digits.length() > 1 && digits[0] == '-' && digits[1] == '-')){ digits = "0"; }

    // std::cout << "digits" << digits << "\n";

    return std::stoi(digits);
}

int countLines(const std::string str) {
    int count = 1; // Start with 1 to account for the last line
    
    for (char c : str) {
        if (c == '\n') {
            count++;
        }
    }

    return count;
}

int rateCodeOneToTen(std::string str) {

    int comments = countOccurrences(str, "// ");
    int blockComments = countOccurrences(str, "/* ");
    int linesOfCode = countLines(str);
    int longerThan = countLengthOccurrences(str, 20);

    return comments;
}

std::string removeNonCodeTags(std::string str) {
    std::string result;
    bool insideCode = false;

    for (size_t i = 0; i < str.length(); ++i) {
        if (str.substr(i, 6) == "<code>") {
            insideCode = true;
            i += 5;  // Skip to the end of "<code>"
        } else if (str.substr(i, 7) == "</code>") {
            insideCode = false;
            i += 6;  // Skip to the end of "</code>"
        } else if (insideCode) {
            // result += '\n';
            result += str[i];
        }
    }

    return result;
}

int main(int argc, char* argv[]) {
    // Check if a filename is provided as an argument
    if (argc < 2) {
        std::cout << "Please provide a CSV filename as an argument.\n";
        return 1;
    }

    std::string filename = argv[1];

    // Open the CSV file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open the file: " << filename << "\n";
        return 1;
    }

    std::vector<MyObject> objects;

    std::string value1, value2;
    bool readingValue2 = false;
    char c;

    while (file.get(c)) {
        if (c == ',') {
            if (!readingValue2) {
                readingValue2 = true;
            } else {
                size_t tildePos = value2.find('~');
                if (tildePos != std::string::npos) {
                    value1 += value2.substr(0, tildePos);
                    MyObject obj;
                    obj.value1 = removeNonDigitsAndConvertToInt(value1);
                    obj.value2 = value2.substr(tildePos + 1);
                    objects.push_back(obj);
                    value1.clear();
                    value2.clear();
                    readingValue2 = false;
                } else {
                    value2 += ',';
                }
            }
        } else if (c == '~') {
            if (readingValue2) {
                MyObject obj;
                obj.value1 = removeNonDigitsAndConvertToInt(value1);
                obj.value2 = value2;
                objects.push_back(obj);
                value1.clear();
                value2.clear();
                readingValue2 = false;
            } else {
                value1 += '~';
            }
        } else if (c == '\n') {
            if (readingValue2) {
                value2 += '\n';
            } else {
                value1 += '\n';
            }
        } else {
            if (!readingValue2) {
                value1 += c;
            } else {
                value2 += c;
            }
        }
    }

    clearCSVFile();

//     int rateCodeOneToTen(std::string str) {

    int comments;
    int blockComments;
    int linesOfCode;
    int longerThan;
    float commentsPerCode;
    float blockCommentsPerCode;
    float singleCommentsPerCode;
    int unanswered =0;

    // Print the array of objects
    for (const auto& obj : objects) {
        std::string str = obj.value2; 
        comments = countOccurrences(str, "// ");
        blockComments = countOccurrences(str, "/* ");
        linesOfCode = countLines(str);
        longerThan = countLengthOccurrences(str, 20);
        commentsPerCode = ((float)comments + (float)blockComments)/(float)linesOfCode;
        blockCommentsPerCode = (float)blockComments/(float)linesOfCode;
        singleCommentsPerCode = (float)comments/(float)linesOfCode;

        if (obj.value1 == 1) { unanswered++ ; }
        if ((comments + blockComments) != 0){
            writeToCSV(obj.value1, comments + blockComments, commentsPerCode, linesOfCode, longerThan, singleCommentsPerCode, blockCommentsPerCode);
        }
    }
    std::cout << "not answered: " << unanswered <<  "\n";



    return 0;
}
