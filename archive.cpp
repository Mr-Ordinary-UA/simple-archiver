#include "archive_core.h"
#include <iostream>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

int readChoice()
{
    int choice;
    while (!(cin >> choice))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input, enter a number: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return choice;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    RLE rle;

    while (true)
    {
        cout << "\n===== RLE ARCHIVER =====\n";
        cout << "1. Compress file\n";
        cout << "2. Decompress file\n";
        cout << "3. Exit\n";
        cout << "Choice: ";

        int choice = readChoice();

        if (choice == 1)
        {
            string input;
            string output;
            cout << "Input file: ";
            getline(cin, input);

            cout << "Output archive: ";
            getline(cin, output);

            CompressStats stats;
            Result r = rle.compress(input, output, stats);
            cout << resultToString(r) << "\n";

            if (r == Result::Ok)
            {
                cout << "Original size: " << stats.originalSize << " bytes\n";
                cout << "Compressed size: " << stats.compressedSize << " bytes\n";
                cout << "Ratio: " << stats.ratio() << "\n";
            }
        }
        else if (choice == 2)
        {
            string input;
            string output;

            cout << "Input archive: ";
            getline(cin, input);

            cout << "Output file: ";
            getline(cin, output);

            Result r = rle.decompress(input, output);
            cout << resultToString(r) << "\n";
        }
        else if (choice == 3)
        {
            break;
        }
        else
        {
            cout << "Invalid choice\n";
        }
    }

    return 0;
}