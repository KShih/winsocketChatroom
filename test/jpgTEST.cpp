#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
using namespace std;

int main()
{/*
    ifstream fin("test2.jpg",ios::binary);
    ofstream fout("out.jpg",ios::binary);
    ostringstream ostrm;
    //ostrm << fin.rdbuf();
    //fout << ostrm;

    while( fin )
        ostrm << fin;
    fout << ostrm ;

    fin.close();
    fout.close();*/
/*    ofstream file;
    file.open("test.jpg",ios::binary);

    ostringstream ostrm;
    ostrm << file.rdbuf();
    cout << ostrm << endl;*/
/*
    ifstream file;
    ofstream out;
    file.open("test2.jpg", ios::binary);
    out.open("out.jpg",ios::binary);
    unsigned char buffer[1000000] = "";
    int BytesSent = 0;
    int BytesIndex = 0;
    while( !file.eof() ) {
        file.read(buffer, 1);
//        int BytesLeft = strlen(buffer);
        /*while(BytesLeft != 0){
            BytesSent = send(sock, &buffer[BytesIndex], BytesLeft, 0);
            BytesLeft -= BytesSent;
            BytesIndex +=BytesSent;
            cout << buffer << endl;
        }
        cout << buffer;
        out << buffer;
    }
    out.close();
    file.close();*/
    ////////////////////////////////////////////////////////////
    // Define file stream object, and open the file
    std::ifstream file("test2.jpg", ios::binary);

    // Prepare iterator pairs to iterate the file content!
    istream_iterator<unsigned char> begin(file), end;

    // Reading the file content using the iterator!
    std::vector<unsigned char> buffer(begin,end);

//    std::copy(buffer.begin(), buffer.end() , std::ostream_iterator<unsigned int>(std::cout, ","));

    ofstream out("out.jpg",ios::binary);
    for(vector<unsigned char>::iterator it = buffer.begin();it!=buffer.end();++it){
        out  << (int)*(it) ;
    }

    out.close();
    file.close();

}

