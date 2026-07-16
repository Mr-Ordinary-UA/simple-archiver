#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
using namespace std;

class RLE
{
public:
    void compress(const string& inputFile, const string& outputFile){
        ifstream in(inputFile, ios::binary);
        if (!in){
            cout << "Cannot open input file\n";
            return;
        }
        ofstream out(outputFile, ios::binary);
        if (!out){
            cout << "Cannot create output file\n";
            return;
        }
        char current;
        char next;
        unsigned char count = 1;
        if (!in.get(current)){
            cout << "Empty file\n";
            return;
        }
        while (in.get(next)){
            if (current == next && count < 255){
                count++;
            }
            else{
                out.write((char*)&count, 1);
                out.write(&current, 1);
                current = next;
                count = 1;
            }
        }
        out.write((char*)&count, 1);
        out.write(&current, 1);
        in.close();
        out.close();
        cout << "File compressed successfully\n";
    }

    void decompress(const string& inputFile, const string& outputFile){
        ifstream in(inputFile, ios::binary);
        if (!in){
            cout << "Cannot open archive\n";
            return;
        }
        ofstream out(outputFile, ios::binary);
        if (!out){
            cout << "Cannot create output file\n";
            return;
        }
        unsigned char count;
        char symbol;
        while (in.read((char*)&count, 1)){
            if (!in.read(&symbol, 1)){
                cout << "Corrupted archive\n";
                return;
            }
            for (int i = 0; i < count; i++){
                out.put(symbol);
            }
        }
        in.close();
        out.close();
        cout << "File decompressed successfully\n";
    }
};

int main(){
    RLE rle;
    int choice;
    while (true)
    {
        cout << "\n===== RLE ARCHIVER =====\n";
        cout << "1. Compress file\n";
        cout << "2. Decompress file\n";
        cout << "3. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();
        if (choice == 1){
            string input;
            string output;
            cout << "Input file: ";
            getline(cin, input);

            cout << "Output archive: ";
            getline(cin, output);

            rle.compress(input, output);
        }
        else if (choice == 2){
            string input;
            string output;

            cout << "Input archive: ";
            getline(cin, input);

            cout << "Output file: ";
            getline(cin, output);

            rle.decompress(input, output);
        }
        else if (choice == 3){
            break;
        }
        else{
            cout << "Invalid choice\n";
        }
    }
    return 0;
}