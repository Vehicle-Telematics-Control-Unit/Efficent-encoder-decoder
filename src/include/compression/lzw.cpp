#include <iostream>
#include <vector>
#include <map>

using namespace std;

void compression(string s,vector<int> &output_code)
{
    cout << "Compression" << endl;
    map<string, int> table;
    for (int i = 0; i < 256; i++) {
        string str = "";
        char ch = i;
        str+= ch;
        table[str] = i;
    }
 
    string str1 = "", str2 = "";
    str1 += s[0];
    int code = 256;
    cout << "String\tOutput_Code\t Extended_Code\n";
    for (int i = 0; i < s.size(); i++) 
    {
        if (i != s.size() - 1)
            str2 += s[i + 1];
        if (table.find(str1 + str2) != table.end()) 
        {
            str1 = str1 + str2;
        }
        else {
            cout << str1 << "\t\t\t" << table[str1] << "\t\t\t"
                 << str1 + str2 << ": " << code << endl;
            output_code.push_back(table[str1]);
            table[str1 + str2] = code;
            code++;
            str1 = str2;
        }
        str2 = "";
    }
    cout << str1 << "\t\t\t" << table[str1] << endl;
    output_code.push_back(table[str1]);
    cout << "Output Codes: ";
    int n=output_code.size();
    for (int i = 0; i < n; i++) 
    {
        cout << output_code[i] << " ";
    }
    cout << endl;
}
 
void decompression(vector<int> output_code)
{
    cout << endl << "Decompression" <<endl;
    map<int, string> table;
    for (int i = 0; i < 256; i++) {
        string str = "";
        char ch=i;
        str += ch;
        table[i] = str;
    }
    int old_code = output_code[0], n;
    string str1 = table[old_code];
    string str2 = "";
    str2 += str1[0];
    cout << str1;
    int count = 256;
    for (int i = 0; i < output_code.size() - 1; i++) {
        n = output_code[i + 1];
        if (table.find(n) == table.end()) 
        {
            str1 = table[old_code];
            str1 = str1 + str2;
        }
        else 
        {
            str1 = table[n];
        }
        // printf("%s", str1);
        cout << str1 ;
        str2 = "";
        str2 += str1[0];
        table[count] = table[old_code] + str2;
        count++;
        old_code = n;
    }
}
int main()
{
    string str= "ABD-ABKABD-ABDABDK";
    vector<int> output_code;
    compression(str,output_code);
    decompression(output_code);
}