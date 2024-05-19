//brilliantadam
//19/5/2024
//Attempt to bruteforce protected winrar file
#include <iostream>
#include <string>
#include <rar.hpp>

using namespace std;

int main() {
    string filename = "your_file.rar";
    string password = "";
    string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:,.<>?";

    // Define the minimum and maximum lengths for the password
    int minPasswordLength = 5;
    int maxPasswordLength = 10;

    // Function to generate all possible combinations of passwords
    void generatePasswords(string& password, int passwordLength) {
        for (char c1 : charset) {
            password += c1;
            for (char c2 : charset) {
                password += c2;
                // ...
                for (char cn : charset) {
                    password += cn;

                    // Attempt to extract the RAR file using the current password
                    RARArchive archive(filename);
                    if (archive.Open(password.c_str())) {
                        cout << "Found password: " << password << endl;
                        return;
                    }

                    // Reset the password for the next iteration
                    password.pop_back();
                }
                password.pop_back();
            }
            password.pop_back();
        }
    }

    // Generate all possible combinations of passwords
    for (int passwordLength = minPasswordLength; passwordLength <= maxPasswordLength; passwordLength++) {
        generatePasswords(password, passwordLength);
    }

    return 0;
}
