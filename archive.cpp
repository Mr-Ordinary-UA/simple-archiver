#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <cctype>
#include <iterator>
#include <cstdio>
using namespace std;

const char SIGNATURE[4] = { 'R', 'L', 'E', '1' };

const unsigned char MODE_COMPRESSED = 0;
const unsigned char MODE_RAW = 1;

enum class Result
{
    Ok,
    CannotOpenInput,
    CannotCreateOutput,
    EmptyFile,
    InvalidFormat,
    CorruptedArchive,
    SameFile
};

struct CompressStats
{
    unsigned long long originalSize = 0;
    unsigned long long compressedSize = 0;

    double ratio() const
    {
        if (originalSize == 0) return 0.0;
        return static_cast<double>(compressedSize) / static_cast<double>(originalSize);
    }
};

string normalizePath(const string& path)
{
    string result = path;
    for (char& c : result)
    {
        if (c == '/') c = '\\';
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

bool samePath(const string& a, const string& b)
{
    return normalizePath(a) == normalizePath(b);
}

class RLE
{
public:
    Result compress(const string& inputFile, const string& outputFile, CompressStats& stats)
    {
        if (samePath(inputFile, outputFile))
        {
            return Result::SameFile;
        }

        ifstream in(inputFile, ios::binary);
        if (!in) return Result::CannotOpenInput;

        string data((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
        in.close();

        if (data.empty()) return Result::EmptyFile;

        string encoded;
        encoded.reserve(data.size());

        size_t i = 0;
        while (i < data.size())
        {
            char current = data[i];
            unsigned short count = 1;
            while (i + count < data.size() &&
                   data[i + count] == current &&
                   count < numeric_limits<unsigned short>::max())
            {
                count++;
            }
            encoded.append(reinterpret_cast<char*>(&count), sizeof(count));
            encoded.push_back(current);
            i += count;
        }

        ofstream out(outputFile, ios::binary);
        if (!out) return Result::CannotCreateOutput;

        out.write(SIGNATURE, sizeof(SIGNATURE));

        stats.originalSize = data.size();

        if (encoded.size() < data.size())
        {
            unsigned char mode = MODE_COMPRESSED;
            out.write(reinterpret_cast<char*>(&mode), 1);
            out.write(encoded.data(), static_cast<streamsize>(encoded.size()));
            stats.compressedSize = sizeof(SIGNATURE) + 1 + encoded.size();
        }
        else
        {
            unsigned char mode = MODE_RAW;
            out.write(reinterpret_cast<char*>(&mode), 1);
            out.write(data.data(), static_cast<streamsize>(data.size()));
            stats.compressedSize = sizeof(SIGNATURE) + 1 + data.size();
        }

        return Result::Ok;
    }

    Result decompress(const string& inputFile, const string& outputFile)
    {
        if (samePath(inputFile, outputFile))
        {
            return Result::SameFile;
        }

        ifstream in(inputFile, ios::binary);
        if (!in) return Result::CannotOpenInput;

        char signature[4];
        if (!in.read(signature, sizeof(signature)))
        {
            return Result::InvalidFormat;
        }
        for (int j = 0; j < 4; j++)
        {
            if (signature[j] != SIGNATURE[j]) return Result::InvalidFormat;
        }

        unsigned char mode;
        if (!in.read(reinterpret_cast<char*>(&mode), 1))
        {
            return Result::CorruptedArchive;
        }

        ofstream out(outputFile, ios::binary);
        if (!out) return Result::CannotCreateOutput;

        if (mode == MODE_RAW)
        {
            out << in.rdbuf();
            return Result::Ok;
        }

        if (mode != MODE_COMPRESSED)
        {
            return Result::InvalidFormat;
        }

        unsigned short count;
        char symbol;
        while (in.read(reinterpret_cast<char*>(&count), sizeof(count)))
        {
            if (!in.read(&symbol, 1))
            {
                return Result::CorruptedArchive;
            }
            for (unsigned short k = 0; k < count; k++)
            {
                out.put(symbol);
            }
        }

        return Result::Ok;
    }
};

void printResult(Result r)
{
    switch (r)
    {
        case Result::Ok:
            cout << "Success\n";
            break;
        case Result::CannotOpenInput:
            cout << "Cannot open input file\n";
            break;
        case Result::CannotCreateOutput:
            cout << "Cannot create output file\n";
            break;
        case Result::EmptyFile:
            cout << "Input file is empty\n";
            break;
        case Result::InvalidFormat:
            cout << "File is not a valid RLE archive\n";
            break;
        case Result::CorruptedArchive:
            cout << "Archive is corrupted\n";
            break;
        case Result::SameFile:
            cout << "Input and output paths must differ\n";
            break;
    }
}

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
            printResult(r);

            if (r == Result::Ok)
            {
                cout << "Original size: " << stats.originalSize << " bytes\n";
                cout << "Compressed size: " << stats.compressedSize << " bytes\n";
                cout << "Ratio: " << stats.ratio() << "\n";

                if (remove(input.c_str()) != 0)
                {
                    cout << "Warning: could not delete original file\n";
                }
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
            printResult(r);

            if (r == Result::Ok)
            {
                if (remove(input.c_str()) != 0)
                {
                    cout << "Warning: could not delete archive\n";
                }
            }
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